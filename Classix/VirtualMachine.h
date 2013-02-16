//
// VirtualMachine.h
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

#ifndef __Classix__VirtualMachine__
#define __Classix__VirtualMachine__

#include "MachineState.h"
#include "FragmentManager.h"
#include "Interpreter.h"
#include "LibraryResolver.h"
#include "PEFLibraryResolver.h"
#include "StackPreparator.h"
#include <list>
#include <algorithm>

namespace Classix
{
	class MainStub;
	
	class VirtualMachine
	{
		friend class MainStub;
		
		Common::IAllocator* allocator;
		PPCVM::MachineState state;
		CFM::PEFLibraryResolver pefResolver;
		CFM::FragmentManager fragmentManager;
		PPCVM::Execution::Interpreter interpreter;
		
	public:
		VirtualMachine(Common::IAllocator* allocator);
		
		void AddLibraryResolver(CFM::LibraryResolver& resolver);
		
		MainStub LoadMainContainer(const std::string& path);
	};
	
	class MainStub
	{
		friend class VirtualMachine;
		
		VirtualMachine& vm;
		CFM::ResolvedSymbol mainSymbol;
		
		MainStub(VirtualMachine& vm, CFM::ResolvedSymbol mainSymbol);
		
		static void AppendInteger(std::string& string, uint32_t integer);
		
		void InitIntEnv();
		
	public:
		uint32_t StackSize;
		
		uint32_t operator()(const std::string& argv0);
		uint32_t operator()(int argc, const char** argv);
		uint32_t operator()(int argc, const char** argv, const char** envp);
		
		template<typename TArgumentIterator>
		inline uint32_t operator()(TArgumentIterator argBegin, TArgumentIterator argEnd)
		{
			return this->operator()(argBegin, argEnd, static_cast<char**>(nullptr), static_cast<char**>(nullptr));
		}
		
		template<typename TArgumentIterator, typename TEnvironIterator>
		uint32_t operator()(TArgumentIterator argBegin, TArgumentIterator argEnd, TEnvironIterator envBegin, TEnvironIterator envEnd)
		{
			Common::AutoAllocation stack = vm.allocator->AllocateAuto("Stack", StackSize);
			
			Common::StackPreparator stackPrep;
			stackPrep.AddArguments(argBegin, argEnd);
			stackPrep.AddEnvironmentVariables(envBegin, envEnd);
			
			auto mainVector = vm.allocator->ToPointer<const PEF::TransitionVector>(mainSymbol.Address);
			auto result = stackPrep.WriteStack(static_cast<char*>(*stack), stack.GetVirtualAddress(), StackSize);
			
			vm.state.r0 = 0;
			vm.state.r1 = vm.allocator->ToIntPtr(result.sp - 8);
			vm.state.r3 = stackPrep.ArgumentCount();
			vm.state.r4 = vm.allocator->ToIntPtr(result.argv);
			vm.state.r5 = vm.allocator->ToIntPtr(result.envp);
			
			InitIntEnv();
			
			vm.state.r2 = mainVector->TableOfContents;
			const void* entryPoint = vm.allocator->ToPointer<const void>(mainVector->EntryPoint);
			vm.interpreter.Execute(entryPoint);
			return vm.state.r3;
		}
	};
}

#endif /* defined(__Classix__VirtualMachine__) */
