//
// ThreadsLib.cpp
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

#include <unordered_map>
#include <dlfcn.h>

#include "ThreadsLib.h"
#include "MachineState.h"
#include "Managers.h"
#include "NotImplementedException.h"
#include "IExecutionEngine.h"

namespace ThreadsLib
{
	struct Globals
	{
		Common::IAllocator& allocator;
		OSEnvironment::ThreadManager& threadManager;
		
		Globals(Common::IAllocator& allocator, OSEnvironment::ThreadManager& threadManager)
		: allocator(allocator), threadManager(threadManager)
		{ }
	};
	
	struct ThreadInfo
	{
		uint16_t state;
	};
}

using ThreadsLib::Globals;
using PPCVM::MachineState;
using PPCVM::Execution::IExecutionEngine;

extern "C"
{
	ThreadsLib::Globals* LibraryLoad(Common::IAllocator* allocator, OSEnvironment::Managers* managers)
	{
		managers->Gestalt().SetValue("thds", 3);
		return allocator->Allocate<Globals>("ThreadsLib Globals", *allocator, managers->ThreadManager());
	}
	
	SymbolType LibraryLookup(Globals* globals, const char* name, void** result)
	{
		char functionName[40] = "ThreadsLib_";
		char* end = stpncpy(functionName + 11, name, 29);
		if (*end == 0)
		{
			if (void* symbol = dlsym(RTLD_SELF, functionName))
			{
				*result = symbol;
				return CodeSymbol;
			}
		}
		
		*result = nullptr;
		return SymbolNotFound;
	}
	
	void LibraryUnload(Globals* globals)
	{
		globals->allocator.Deallocate(globals);
	}
	
#pragma mark -
	void ThreadsLib_GetThreadCurrentTaskRef(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_GetDefaultThreadStackSize(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		Common::SInt32& out = *globals->allocator.ToPointer<Common::SInt32>(state->r4);
		out = 512 * 1024;
		state->r3 = 0;
	}
	
	void ThreadsLib_SetThreadTerminator(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_DisposeThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_GetFreeThreadCount(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_SetThreadSwitcher(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_YieldToThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_GetSpecificFreeThreadCount(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_ThreadBeginCritical(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		globals->threadManager.EnterCriticalSection();
	}
	
	void ThreadsLib_ThreadCurrentStackSpace(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_CreateThreadPool(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_SetDebuggerNotificationProcs(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_GetThreadStateGivenTaskRef(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_SetThreadState(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_GetThreadState(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_NewThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_SetThreadStateEndCritical(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_SetThreadScheduler(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_GetCurrentThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_ThreadEndCritical(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		globals->threadManager.ExitCriticalSection();
	}
	
	void ThreadsLib_SetThreadReadyGivenTaskRef(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ThreadsLib_YieldToAnyThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
}