//
// ThreadsLibFunctions.h
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

#ifndef __Classix__ThreadsLibFunctions__
#define __Classix__ThreadsLibFunctions__

namespace PPCVM
{
	struct MachineState;
}

namespace ThreadsLib
{
	struct Globals;
}

extern "C"
{
	void ThreadsLib_GetThreadCurrentTaskRef(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_GetDefaultThreadStackSize(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_SetThreadTerminator(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_DisposeThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_GetFreeThreadCount(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_SetThreadSwitcher(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_YieldToThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_GetSpecificFreeThreadCount(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_ThreadBeginCritical(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_ThreadCurrentStackSpace(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_CreateThreadPool(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_SetDebuggerNotificationProcs(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_GetThreadStateGivenTaskRef(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_SetThreadState(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_GetThreadState(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_NewThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_SetThreadStateEndCritical(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_SetThreadScheduler(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_GetCurrentThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_ThreadEndCritical(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_SetThreadReadyGivenTaskRef(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
	void ThreadsLib_YieldToAnyThread(ThreadsLib::Globals* globals, PPCVM::MachineState* state);
}

#endif /* defined(__Classix__ThreadsLibFunctions__) */
