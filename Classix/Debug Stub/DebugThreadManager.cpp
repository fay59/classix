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

#include "DebugThreadManager.h"
#include "ThreadContext.h"
#include "InvalidInstructionException.h"
#include "AccessViolationException.h"
#include "TrapException.h"
#include "Todo.h"

using namespace Common;
using namespace PPCVM;

const uint32_t BreakpointTrap = 0x7C000008;

bool DebugThreadManager::GetRealInstruction(Common::UInt32 *location, PPCVM::Instruction &output)
{
	auto iter = breakpoints.find(location);
	if (iter == breakpoints.end())
	{
		output.hex = *location;
		return false;
	}
	else
	{
		output = iter->second.first;
		return true;
	}
}

void DebugThreadManager::DebugLoop(ThreadContext& context, bool autostart)
{
	context.stopReason = StopReason::InterruptTrap;
	context.executionState = ThreadState::Stopped;
	changingContexts.PutOne(ThreadUpdate(context));
	
	if (autostart)
	{
		context.Perform(RunCommand::Continue);
	}
	
	Instruction initialInstruction;
	while (context.executionState != ThreadState::Completed)
	{
		RunCommand action = context.GetNextAction();
		UInt32* location = allocator.ToPointer<UInt32>(context.pc);
		try
		{
			if (action == RunCommand::Kill)
			{
				context.pc = allocator.ToIntPtr(context.interpreter.GetEndAddress());
			}
			else if (action == RunCommand::Continue)
			{
				GetRealInstruction(location, initialInstruction);
				location = context.interpreter.ExecuteOne(location, initialInstruction);
				context.interpreter.Execute(location);
				context.pc = allocator.ToIntPtr(context.interpreter.GetEndAddress());
			}
			else if (action == RunCommand::SingleStep)
			{
				GetRealInstruction(location, initialInstruction);
				location = context.interpreter.ExecuteOne(location, initialInstruction);
				context.pc = allocator.ToIntPtr(location);
			}
			else if (action == RunCommand::StepOver)
			{
				Instruction instruction(location->Get());
				if (instruction.OPCD != 18 || instruction.LK == 0)
				{
					// step over is the same as single step, unless we step over a call instruction...
					location = context.interpreter.ExecuteOne(location);
					context.pc = allocator.ToIntPtr(location);
				}
				else
				{
					// ...then we should set a breakpoint on the next instruction, and execute until we reach it,
					// and make sure the stack is the same size (otherwise we're doing recursion).
					Breakpoint stopAtNext = CreateBreakpoint(&location[1]);
					uint32_t stopPC = allocator.ToIntPtr(stopAtNext.GetLocation());
					uint32_t sp = context.machineState.r1;
					do
					{
						try
						{
							// don't be stuck on the breakpoint if it's the first instruction we execute
							GetRealInstruction(location, initialInstruction);
							location = context.interpreter.ExecuteOne(location, stopAtNext.GetInstruction());
							context.interpreter.Execute(location);
							context.pc = allocator.ToIntPtr(context.interpreter.GetEndAddress());
							break;
						}
						catch (Execution::InterpreterException& ex)
						{
							context.pc = ex.GetPC();
							auto cause = ex.GetReason().get();
							if (dynamic_cast<TrapException*>(cause) == nullptr || context.pc != stopPC)
								throw;
						}
					} while (context.machineState.r1 != sp);
				}
			}
			
			context.executionState = context.pc == allocator.ToIntPtr(context.interpreter.GetEndAddress())
				? ThreadState::Completed
				: ThreadState::Stopped;
		}
		catch (Execution::InterpreterException& ex)
		{
			context.pc = ex.GetPC();
			context.executionState = ThreadState::Stopped;
			auto cause = ex.GetReason().get();
			if (dynamic_cast<Execution::InvalidInstructionException*>(cause))
				context.stopReason = StopReason::InvalidInstruction;
			else if (dynamic_cast<Common::AccessViolationException*>(cause))
				context.stopReason = StopReason::AccessViolation;
			else if (dynamic_cast<TrapException*>(cause))
				context.stopReason = StopReason::InterruptTrap;
		}
		
		changingContexts.PutOne(ThreadUpdate(context));
	}
}

DebugThreadManager::DebugThreadManager(Common::Allocator& allocator)
: allocator(allocator), sink(new WaitQueue<std::string>), lastExitCode(0xeeeeeeee)
{ }

bool DebugThreadManager::IsThreadExecuting() const
{
	throw std::logic_error("Not implemented");
}

void DebugThreadManager::MarkThreadAsExecuting()
{
	// does nothing. This should probably be redesigned.
}

void DebugThreadManager::UnmarkThreadAsExecuting()
{
	// does nothing, this should probably be redesigned
}

void DebugThreadManager::EnterCriticalSection() noexcept
{
	TODO("Implement critical sections");
}
			
void DebugThreadManager::ExitCriticalSection() noexcept
{
	TODO("Implement critical sections");
}

void DebugThreadManager::SetBreakpoint(UInt32 *location)
{
	std::lock_guard<std::mutex> guard(breakpointsLock);
	auto iter = breakpoints.find(location);
	if (iter == breakpoints.end())
	{
		iter = breakpoints.insert(std::make_pair(location, std::make_pair(Instruction(location->Get()), 0))).first;
		*location = BreakpointTrap;
	}
	
	iter->second.second++;
}

bool DebugThreadManager::RemoveBreakpoint(UInt32 *location)
{
	std::lock_guard<std::mutex> guard(breakpointsLock);
	auto iter = breakpoints.find(location);
	if (iter == breakpoints.end())
		return false;
	
	iter->second.second--;
	if (iter->second.second == 0)
	{
		*iter->first = iter->second.first.hex;
		breakpoints.erase(iter);
	}
	return true;
}

DebugThreadManager::Breakpoint DebugThreadManager::CreateBreakpoint(UInt32 *location)
{
	return Breakpoint(*this, location);
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
		ThreadId threadId = update.context.GetThreadId();
		
		if (update.state == ThreadState::Completed)
		{
			update.context.thread.join();
			
			std::lock_guard<std::recursive_mutex> guard(threadsLock);
			lastExitCode = update.context.machineState.r3;
			threads.erase(threadId);
			if (threads.size() == 0)
			{
				run = false;
			}
		}
		
		std::string message = "$ThreadStatusChanged;";
		char threadIdString[] = "00000000";
		sprintf(threadIdString, "%08x", threadId);
		message += threadIdString;
		sink->PutOne(message);
	}
}

uint32_t DebugThreadManager::GetLastExitCode() const
{
	return lastExitCode;
}

ThreadContext& DebugThreadManager::StartThread(const Common::StackPreparator& stack, size_t stackSize, const PEF::TransitionVector& entryPoint, bool startNow)
{
	// lldb enforces some alignment constraints on the stack, so align it correctly by allocating some additional memory
	ThreadContext* context = new ThreadContext(allocator, (threads.size() + 1) * 16, stackSize + 0x200);
	uint32_t stackAddress = allocator.ToIntPtr(*context->stack);
	stackAddress += 0x200;
	stackAddress &= ~0x1ff;
	auto info = stack.WriteStack(allocator.ToPointer<char>(stackAddress), stackAddress, stackSize);
	context->machineState.r1 = allocator.ToIntPtr(info.sp);
	context->machineState.r2 = entryPoint.TableOfContents;
	context->machineState.r3 = context->machineState.r27 = info.argc;
	context->machineState.r4 = context->machineState.r28 = allocator.ToIntPtr(info.argv);
	context->machineState.r5 = context->machineState.r29 = allocator.ToIntPtr(info.envp);
	context->machineState.lr = allocator.ToIntPtr(context->interpreter.GetEndAddress());
	context->pc = entryPoint.EntryPoint;
	
	context->thread = std::thread(&DebugThreadManager::DebugLoop, this, std::ref(*context), startNow);
	
	// this should stay at the end of the method or be scoped
	std::lock_guard<std::recursive_mutex> lock(threadsLock);
	threads[context->GetThreadId()].reset(context);
	
	return *context;
}

bool DebugThreadManager::HasCompleted() const
{
	std::lock_guard<std::recursive_mutex> lock(threadsLock);
	return threads.size() == 0;
}

ThreadContext* DebugThreadManager::GetThread(ThreadId handle)
{
	std::lock_guard<std::recursive_mutex> lock(threadsLock);
	auto iter = threads.find(handle);
	if (iter == threads.end())
		return nullptr;
	
	return iter->second.get();
}

DebugThreadManager::Breakpoint::Breakpoint(DebugThreadManager& manager, UInt32* instruction)
: threads(manager), location(instruction), instruction(*instruction)
{
	threads.SetBreakpoint(instruction);
}

DebugThreadManager::Breakpoint::Breakpoint(Breakpoint&& that)
: threads(that.threads), instruction(that.instruction), location(that.location)
{
	that.location = nullptr;
}

const UInt32* DebugThreadManager::Breakpoint::GetLocation() const
{
	return location;
}

PPCVM::Instruction DebugThreadManager::Breakpoint::GetInstruction() const
{
	return instruction;
}

DebugThreadManager::Breakpoint::~Breakpoint()
{
	if (location != nullptr)
	{
		threads.RemoveBreakpoint(location);
	}
}
