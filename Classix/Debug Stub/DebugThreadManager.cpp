//
// DebugThreadManager.cpp
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

#include <pthread.h>

#include "Todo.h"
#include "DebugThreadManager.h"
#include "InvalidInstructionException.h"
#include "AccessViolationException.h"
#include "TrapException.h"

ThreadContext::ThreadContext(Common::Allocator& allocator, DebugThreadManager& threads, size_t stackSize)
	: interpreter(allocator, machineState)
{
	executionState = ThreadState::NotReady;
	stack.reset(new Common::AutoAllocation(allocator.AllocateAuto("Thread Stack", stackSize)));
}

void ThreadContext::Interrupt()
{
	interpreter.Interrupt();
}

void ThreadContext::Resume()
{
	if (executionState != ThreadState::Stopped)
		throw std::logic_error("Threads can only be resumed from the Stopped state");
	
	std::unique_lock<std::mutex> guard(mShouldResume);
	executionState = ThreadState::Executing;
	cvShouldResume.notify_one();
}

void ThreadContext::Kill()
{
	std::unique_lock<std::mutex> guard(mShouldResume);
	if (executionState == ThreadState::Executing)
	{
		Interrupt();
	}
	
	executionState = ThreadState::Completed;
	cvShouldResume.notify_one();
}

std::thread::native_handle_type ThreadContext::GetThreadId()
{
	return thread.native_handle();
}

ThreadUpdate::ThreadUpdate(ThreadContext& ctx)
: context(ctx), state(context.executionState)
{ }

void DebugThreadManager::DebugLoop(ThreadContext& context, bool autostart)
{
	context.stopReason = StopReason::InterruptTrap;
	context.executionState = autostart ? ThreadState::Executing : ThreadState::Stopped;
	changingContexts.PutOne(ThreadUpdate(context));
	
	while (true)
	{
		{
			std::unique_lock<std::mutex> guard(context.mShouldResume);
			context.cvShouldResume.wait(guard, [&context] { return context.executionState != ThreadState::Stopped; });
		}
		
		if (context.executionState == ThreadState::Completed)
			break;
		
		context.stopReason = StopReason::Executing;
		const void* location = allocator.ToPointer<void>(context.pc);
		try
		{
			context.interpreter.Execute(location);
			break;
		}
		catch (PPCVM::Execution::InterpreterException& ex)
		{
			context.pc = ex.GetPC();
			auto cause = ex.GetReason().get();
			if (dynamic_cast<PPCVM::Execution::InvalidInstructionException*>(cause))
				context.stopReason = StopReason::InvalidInstruction;
			else if (dynamic_cast<Common::AccessViolationException*>(cause))
				context.stopReason = StopReason::AccessViolation;
			else if (dynamic_cast<PPCVM::TrapException*>(cause))
				context.stopReason = StopReason::InterruptTrap;
			
			changingContexts.PutOne(ThreadUpdate(context));
		}
	}
	
	context.stack.reset();
	context.executionState = ThreadState::Completed;
	changingContexts.PutOne(ThreadUpdate(context));
}

DebugThreadManager::DebugThreadManager(Common::Allocator& allocator)
: allocator(allocator), sink(new WaitQueue<std::string>)
{ }

bool DebugThreadManager::IsThreadExecuting() const
{
	std::lock_guard<std::recursive_mutex> lock(threadsLock);
	auto iter = threads.find(pthread_self());
	return iter != threads.end();
}

void DebugThreadManager::MarkThreadAsExecuting()
{
	// does nothing. This should probably be redesigned.
}

void DebugThreadManager::UnmarkThreadAsExecuting()
{
	std::lock_guard<std::recursive_mutex> lock(threadsLock);
	threads.erase(pthread_self());
}

void DebugThreadManager::EnterCriticalSection() noexcept
{
	TODO("Implement critical sections");
}
			
void DebugThreadManager::ExitCriticalSection() noexcept
{
	TODO("Implement critical sections");
}

std::shared_ptr<WaitQueue<std::string>> DebugThreadManager::GetCommandSink()
{
	return sink;
}

void DebugThreadManager::SetCommandSink(std::shared_ptr<WaitQueue<std::string>>& sink)
{
	this->sink = sink;
}

void DebugThreadManager::ConsumeThreadEvents()
{
	bool run = true;
	while (run)
	{
		ThreadUpdate update = changingContexts.TakeOne();
		std::thread::native_handle_type threadId = update.context.GetThreadId();
		
		if (update.state == ThreadState::Completed)
		{
			update.context.thread.join();
			
			std::lock_guard<std::recursive_mutex> guard(threadsLock);
			threads.erase(threadId);
			if (threads.size() == 0)
			{
				run = false;
			}
		}
		
		sink->PutOne("qThreadStopInfo");
	}
}

ThreadContext& DebugThreadManager::StartThread(const Common::StackPreparator& stack, size_t stackSize, const PEF::TransitionVector& entryPoint, bool startNow)
{
	ThreadContext* context = new ThreadContext(allocator, *this, stackSize);
	auto info = stack.WriteStack(static_cast<char*>(**context->stack), context->stack->GetVirtualAddress(), stackSize);
	context->machineState.r1 = allocator.ToIntPtr(info.sp);
	context->machineState.r2 = entryPoint.TableOfContents;
	context->machineState.r3 = context->machineState.r27 = info.argc;
	context->machineState.r4 = context->machineState.r28 = allocator.ToIntPtr(info.argv);
	context->machineState.r5 = context->machineState.r29 = allocator.ToIntPtr(info.envp);
	context->pc = entryPoint.EntryPoint;
	
	context->thread = std::thread(std::bind(&DebugThreadManager::DebugLoop, this, std::ref(*context), startNow));
	
	// this should stay at the end of the method or be scoped
	std::lock_guard<std::recursive_mutex> lock(threadsLock);
	threads[context->GetThreadId()].reset(context);
	
	return *context;
}

size_t DebugThreadManager::ThreadCount() const
{
	std::lock_guard<std::recursive_mutex> lock(threadsLock);
	return threads.size();
}

bool DebugThreadManager::GetThread(std::thread::native_handle_type handle, ThreadContext*& context)
{
	std::lock_guard<std::recursive_mutex> lock(threadsLock);
	auto iter = threads.find(handle);
	if (iter == threads.end())
		return false;
	
	context = iter->second.get();
	return true;
}
