//
// ThreadContext.cpp
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

#include "ThreadContext.h"

ThreadContext::ThreadContext(Common::Allocator& allocator, DebugThreadManager::ThreadId id, size_t stackSize)
: id(id), interpreter(allocator, machineState), stack(allocator.AllocateAuto("Thread Stack", stackSize))
{
	executionState = ThreadState::NotReady;
	nextAction = RunCommand::None;
}

void ThreadContext::Interrupt()
{
	interpreter.Interrupt();
}

ThreadState ThreadContext::GetThreadState() const
{
	return executionState;
}

StopReason ThreadContext::GetStopReason() const
{
	return stopReason;
}

void ThreadContext::Perform(RunCommand command)
{
	if (executionState != ThreadState::Stopped)
		throw std::logic_error("Threads can only be resumed from the Stopped state");
	
	if (static_cast<int>(command) < 0)
		throw std::logic_error("Cannot use private command");
	
	std::unique_lock<std::mutex> guard(mShouldResume);
	nextAction = command;
	cvShouldResume.notify_one();
}

void ThreadContext::Kill()
{
	std::unique_lock<std::mutex> guard(mShouldResume);
	if (executionState == ThreadState::Executing)
	{
		Interrupt();
	}
	
	nextAction = RunCommand::Kill;
	cvShouldResume.notify_one();
}

DebugThreadManager::ThreadId ThreadContext::GetThreadId()
{
	return id;
}

RunCommand ThreadContext::GetNextAction()
{
	std::unique_lock<std::mutex> guard(mShouldResume);
	cvShouldResume.wait(guard, [this] { return nextAction.load() != RunCommand::None; });
	return nextAction.exchange(RunCommand::None);
}

ThreadUpdate::ThreadUpdate(ThreadContext& ctx)
: context(ctx), state(context.GetThreadState())
{ }