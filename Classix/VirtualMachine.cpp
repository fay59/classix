//
// VirtualMachine.cpp
// Classix
//
// Copyright (C) 2012 Félix Cloutier
//
// This file is part of Classix.
//
// Classix is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Classix is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Classix. If not, see http://www.gnu.org/licenses/.
//

#include "VirtualMachine.h"
#include "DlfcnSymbolResolver.h"

namespace Classix
{
	MainStub::MainStub(VirtualMachine& vm, CFM::ResolvedSymbol mainSymbol)
	: vm(vm), mainSymbol(mainSymbol)
	{
		StackSize = 0x100000;
	}
	
	void MainStub::InitIntEnv()
	{
		// arguments also go to _IntEnv
		if (CFM::SymbolResolver* resolver = vm.fragmentManager.GetSymbolResolver("StdCLib"))
		{
			auto symbol = resolver->ResolveSymbol("__StdCLib_IntEnvInit");
			assert(symbol.Universe != CFM::SymbolUniverse::LostInTimeAndSpace && "Found StdCLib but couldn't find __StdCLib_IntEnvInit!");
			
			PEF::TransitionVector* vector = reinterpret_cast<PEF::TransitionVector*>(symbol.Address);
			void* intEnvInit = vm.allocator->ToPointer<void>(vector->EntryPoint);
			vm.state.r2 = vector->TableOfContents;
			vm.interpreter.Execute(intEnvInit);
		}
	}
	
	uint32_t MainStub::operator()(const std::string& argv0)
	{
		return this->operator()(&argv0, (&argv0) + 1);
	}
	
	uint32_t MainStub::operator()(int argc, const char** argv)
	{
		return this->operator()(argv, argv + argc);
	}
	
	uint32_t MainStub::operator()(int argc, const char** argv, const char** envp)
	{
		const char** envpEnd = envp;
		while (*envpEnd != nullptr)
			envpEnd++;
		
		return this->operator()(argv, argv + argc, envp, envpEnd);
	}
	
	VirtualMachine::VirtualMachine(Common::IAllocator* allocator)
	: allocator(allocator), interpreter(allocator, &state), pefResolver(allocator, fragmentManager)
	{
		AddLibraryResolver(pefResolver);
	}
	
	void VirtualMachine::AddLibraryResolver(CFM::LibraryResolver &resolver)
	{
		fragmentManager.LibraryResolvers.push_back(&resolver);
	}
	
	MainStub VirtualMachine::LoadMainContainer(const std::string &path)
	{
		if (!fragmentManager.LoadContainer(path))
			throw std::logic_error("Could not load specified container");
		
		auto resolver = fragmentManager.GetSymbolResolver(path);
		auto entryPoints = resolver->GetEntryPoints();
		for (const auto& entryPoint : entryPoints)
		{
			if (entryPoint.Name == CFM::SymbolResolver::MainSymbolName)
			{
				if (entryPoint.Universe == CFM::SymbolUniverse::PowerPC)
					return MainStub(*this, entryPoint);
				else
					throw std::logic_error("Container successfully resolved, but main symbol is not a PPC symbol");
			}
		}
		throw std::logic_error("Container does not contain a main symbol");
	}
}
