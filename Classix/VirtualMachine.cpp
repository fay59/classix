//
//  VirtualMachine.cpp
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "VirtualMachine.h"

namespace Classix
{
	MainStub::MainStub(VirtualMachine& vm, CFM::ResolvedSymbol mainSymbol)
	: vm(vm), mainSymbol(mainSymbol)
	{
		StackSize = 0x100000;
	}
	
	void MainStub::AppendInteger(std::string &string, uint32_t integer)
	{
		char digit = integer % 10 + '0';
		int leftover = integer / 10;
		if (leftover > 0)
			AppendInteger(string, leftover);
		
		string += digit;
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
			(*envpEnd)++;
		
		return this->operator()(argv, argv + argc, envp, envpEnd);
	}
	
	VirtualMachine::VirtualMachine(Common::IAllocator* allocator)
	: allocator(allocator), interpreter(allocator, &state), pefResolver(allocator, fragmentManager)
	{
		MachineStateInit(&state);
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
		auto main = resolver->GetMainAddress();
		if (main.Universe != CFM::SymbolUniverse::PowerPC)
			throw std::logic_error("Container successfully resolved, but main symbol is not a PPC symbol");
		
		return MainStub(*this, main);
	}
}
