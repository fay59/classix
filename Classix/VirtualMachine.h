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
			uint32_t argIndex = 0;
			std::list<Common::AutoAllocation> allocations;
			
			// arguments
			std::string argumentName = "Argument #";
			size_t eraseBegin = argumentName.length();
			for (TArgumentIterator iter = argBegin; iter != argEnd; iter++)
			{
				std::string argument = *iter;
				AppendInteger(argumentName, argIndex);
				argIndex++;
				Common::AutoAllocation allocation = vm.allocator->AllocateAuto(argumentName, argument.length());
				std::copy(argument.begin(), argument.end(), static_cast<char*>(*allocation));
				
				argumentName.erase(eraseBegin);
				allocations.emplace_back(std::move(allocation));
			}
			
			uint32_t argc = allocations.size();
			Common::AutoAllocation argvAlloc = vm.allocator->AllocateAuto("Argument Array", argc * sizeof(uint32_t));
			Common::UInt32* argv = static_cast<Common::UInt32*>(*argvAlloc);
			argIndex = 0;
			for (auto& allocation : allocations)
			{
				argv[argIndex] = allocation.GetVirtualAddress();
				argIndex++;
			}
			
			// environment
			auto envAllocsBegin = allocations.end();
			std::string environName = "Environment String #";
			eraseBegin = environName.length();
			for (TEnvironIterator iter = envBegin; iter != envEnd; iter++)
			{
				std::string environmentSetting = *iter;
				AppendInteger(environName, argIndex);
				argIndex++;
				Common::AutoAllocation allocation = vm.allocator->AllocateAuto(argumentName, environmentSetting.length());
				std::copy(environmentSetting.begin(), environmentSetting.end(), static_cast<char*>(*allocation));
				
				environName.erase(eraseBegin);
				allocations.emplace_back(std::move(allocation));
			}
			
			uint32_t envSize = allocations.size() - argc;
			Common::AutoAllocation envpAlloc = vm.allocator->AllocateAuto("Environment Array", envSize * sizeof(uint32_t));
			Common::UInt32* envp = static_cast<Common::UInt32*>(*argvAlloc);
			argIndex = 0;
			for (auto iter = envAllocsBegin; iter != allocations.end(); iter++)
			{
				envp[argIndex] = iter->GetVirtualAddress();
				argIndex++;
			}
			envp[argIndex] = 0;
			
			// prepare to start
			auto mainVector = vm.allocator->ToPointer<const PEF::TransitionVector>(mainSymbol.Address);
			Common::AutoAllocation stack = vm.allocator->AllocateAuto("Stack", StackSize);
			
			// according to http://opensource.apple.com/source/Csu/Csu-47/start.s the following registers should be set to:
			// r0: 0
			// r1: stack ptr
			// r2: (not set, but we set it to the executable's TOC anyways)
			// r3: argc
			// r4: argv
			// r5: envp
			// r27: (argc+1) * sizeof(char*), but that shouldn't matter
			
			vm.state.r0 = 0;
			vm.state.r1 = stack.GetVirtualAddress() + StackSize - 12;
			vm.state.r2 = mainVector->TableOfContents;
			vm.state.r3 = argc;
			vm.state.r4 = vm.allocator->ToIntPtr(argv);
			vm.state.r5 = vm.allocator->ToIntPtr(envp);
			vm.state.r27 = (argc + 1) * sizeof(uint32_t);
			
			const void* entryPoint = vm.allocator->ToPointer<const void>(mainVector->EntryPoint);
			vm.interpreter.Execute(entryPoint);
			return vm.state.r3;
		}
	};
}

#endif /* defined(__Classix__VirtualMachine__) */
