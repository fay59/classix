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
#include <unordered_set>

namespace Classix
{
	uint32_t ProgramControlHandle::RunSymbol(CFM::ResolvedSymbol& symbol)
	{
		auto vector = vm.allocator.ToPointer<const PEF::TransitionVector>(symbol.Address);
		BeginTransition(*vector);
		vm.interpreter.Execute(vm.allocator.ToPointer<void>(pc));
		return vm.state.r3;
	}
	
	void ProgramControlHandle::BeginTransition(const PEF::TransitionVector &vector)
	{
		vm.state.r0 = 0;
		vm.state.r1 = vm.allocator.ToIntPtr(stackInfo.sp - 8);
		vm.state.r3 = vm.state.r27 = stackInfo.argc;
		vm.state.r4 = vm.state.r28 = vm.allocator.ToIntPtr(stackInfo.argv);
		vm.state.r5 = vm.state.r29 = vm.allocator.ToIntPtr(stackInfo.envp);
		
		vm.state.r2 = vector.TableOfContents;
		pc = vector.EntryPoint;
	}
	
	void ProgramControlHandle::StepInto()
	{
		const void* newPC = vm.interpreter.ExecuteOne(vm.allocator.ToPointer<void>(pc));
		pc = vm.allocator.ToIntPtr(newPC);
	}
	
	void ProgramControlHandle::StepOver()
	{
		Common::UInt32 word = *vm.allocator.ToPointer<Common::UInt32>(pc);
		PPCVM::Instruction inst = word.Get();
		if (inst.OPCD == 18 && inst.LK == 1)
		{
			uint32_t sp = vm.state.r1;
			uint32_t desiredPC = pc + 4;
			do
			{
				RunTo(desiredPC);
			} while (vm.state.r1 != sp);
		}
		else
			StepInto();
	}
	
	void ProgramControlHandle::RunTo(uint32_t address)
	{
		std::unordered_set<const void*> until = {vm.allocator.ToPointer<void>(address)};
		const void* eip = vm.allocator.ToPointer<const void>(pc);
		eip = vm.interpreter.ExecuteUntil(eip, until);
		pc = vm.allocator.ToIntPtr(eip);
	}
	
	MainStub::MainStub(VirtualMachine& vm, CFM::ResolvedSymbol mainSymbol)
	: vm(vm), mainSymbol(mainSymbol)
	{
		StackSize = 0x100000;
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
	
	VirtualMachine::VirtualMachine(Common::IAllocator& allocator)
	: allocator(allocator), interpreter(allocator, state), pefResolver(allocator, fragmentManager)
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
