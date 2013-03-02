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
#include <deque>
#include <list>
#include <algorithm>

namespace Classix
{
	class MainStub;
	class ProgramControlHandle;
	
	class VirtualMachine
	{
		friend class MainStub;
		friend class ProgramControlHandle;
		
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
	
	class ProgramControlHandle
	{
		friend class MainStub;
		
		VirtualMachine& vm;
		Common::AutoAllocation stack;
		Common::StackPreparator::StackInfo stackInfo;
		std::deque<CFM::ResolvedSymbol> initSymbols;
		std::deque<CFM::ResolvedSymbol> termSymbols;
		
		template<typename TArgumentIterator, typename TEnvironIterator>
		ProgramControlHandle(VirtualMachine& vm, uint32_t stackSize, TArgumentIterator argBegin, TArgumentIterator argEnd, TEnvironIterator envBegin, TEnvironIterator envEnd)
		: vm(vm), stack(vm.allocator->AllocateAuto("Stack", stackSize))
		{
			Common::StackPreparator stackPrep;
			stackPrep.AddArguments(argBegin, argEnd);
			stackPrep.AddEnvironmentVariables(envBegin, envEnd);
			
			stackInfo = stackPrep.WriteStack(static_cast<char*>(*stack), stack.GetVirtualAddress(), stackSize);
			
			for (auto& pair : vm.fragmentManager)
			{
				CFM::SymbolResolver* resolver = pair.second;
				auto entryPoints = resolver->GetEntryPoints();
				for (auto& entryPoint : entryPoints)
				{
					if (entryPoint.Name == CFM::SymbolResolver::InitSymbolName)
						initSymbols.push_back(entryPoint);
					else if (entryPoint.Name == CFM::SymbolResolver::TermSymbolName)
						termSymbols.push_back(entryPoint);
				}
			}
			
			for (auto& symbol : initSymbols)
				RunSymbol(symbol);
		}
		
	public:
		uint32_t RunSymbol(CFM::ResolvedSymbol& symbol);
		
		uint32_t pc;
		
		void BeginTransition(const PEF::TransitionVector& vector);
		void StepOver();
		void StepInto();
		void RunTo(uint32_t address);
	};
	
	class MainStub
	{
		friend class VirtualMachine;
		
		VirtualMachine& vm;
		CFM::ResolvedSymbol mainSymbol;
		
		MainStub(VirtualMachine& vm, CFM::ResolvedSymbol mainSymbol);
		
	public:
		uint32_t StackSize;
		
		template<typename TArgumentIterator, typename TEnvironIterator>
		ProgramControlHandle Instantiate(TArgumentIterator argBegin, TArgumentIterator argEnd, TEnvironIterator envBegin, TEnvironIterator envEnd)
		{
			return ProgramControlHandle(vm, StackSize, argBegin, argEnd, envBegin, envEnd);
		}
		
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
			return Instantiate(argBegin, argEnd, envBegin, envEnd).RunSymbol(mainSymbol);
		}
	};
}

#endif /* defined(__Classix__VirtualMachine__) */
