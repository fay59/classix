//
// ThreadContext.h
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

#ifndef __Classix__ThreadContext__
#define __Classix__ThreadContext__

#include <thread>
#include "Allocator.h"
#include "MachineState.h"
#include "Interpreter.h"
#include "DebugThreadManager.h"

enum class ThreadState
{
	NotReady,
	Stopped,
	Executing,
	Completed
};

enum class StopReason
{
	Executing,
	InterruptTrap,
	AccessViolation,
	InvalidInstruction,
};

enum class RunCommand
{
	Kill = -1,
	None = 0,
	
	// Commands <= 0 cannot be passed to Perform
	SingleStep,
	StepOver,
	Continue,
};

struct ThreadContext
{
	friend class DebugThreadManager;
	
	Common::AutoAllocation stack;
	PPCVM::MachineState machineState;
	uint32_t pc; // valid only when thread is stopped
	
	ThreadState GetThreadState() const;
	StopReason GetStopReason() const;
	void Perform(RunCommand command);
	
	void Interrupt();
	void Kill();
	
	DebugThreadManager::ThreadId GetThreadId();
	
private:
	PPCVM::Execution::Interpreter interpreter;
	std::thread thread;
	DebugThreadManager::ThreadId id;
	ThreadState executionState;
	StopReason stopReason;
	
	std::mutex mShouldResume;
	std::condition_variable cvShouldResume;
	std::atomic<RunCommand> nextAction;
	
	ThreadContext(Common::Allocator& allocator, DebugThreadManager::ThreadId id, size_t stackSize);
	
	RunCommand GetNextAction();
	
	ThreadContext(const ThreadContext&) = delete;
	ThreadContext(ThreadContext&&) = delete;
	void operator=(const ThreadContext&) = delete;
	void operator=(ThreadContext&&) = delete;
};

#endif /* defined(__Classix__ThreadContext__) */
