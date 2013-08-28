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

#ifndef __Classix__DebugThreadManager__
#define __Classix__DebugThreadManager__

#include <mutex>
#include <thread>
#include <memory>
#include <vector>
#include <condition_variable>

#include "ThreadManager.h"
#include "MachineState.h"
#include "Interpreter.h"
#include "StackPreparator.h"
#include "Structures.h"
#include "WaitQueue.h"

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

class DebugThreadManager;

struct ThreadContext
{
	friend class DebugThreadManager;
	
	std::unique_ptr<Common::AutoAllocation> stack;
	PPCVM::MachineState machineState;
	uint32_t pc; // valid only when thread is stopped
	
	// wrapped in atomics for visibility between threads
	std::atomic<ThreadState> executionState;
	std::atomic<StopReason> stopReason;
	
	void Interrupt();
	void Resume();
	void Kill();
	
	std::thread::native_handle_type GetThreadId();
	
private:
	PPCVM::Execution::Interpreter interpreter;
	std::thread thread;
	std::mutex mShouldResume;
	std::condition_variable cvShouldResume;
	
	ThreadContext(Common::Allocator& allocator, DebugThreadManager& manager, size_t stackSize);
	
	ThreadContext(const ThreadContext&) = delete;
	ThreadContext(ThreadContext&&) = delete;
	void operator=(const ThreadContext&) = delete;
	void operator=(ThreadContext&&) = delete;
};

struct ThreadUpdate
{
	ThreadContext& context;
	ThreadState state;
	
	explicit ThreadUpdate(ThreadContext& context);
};

class DebugThreadManager : public OSEnvironment::ThreadManager
{
	friend class ThreadContext;
	
	// needs to be a recursive mutex so EnterCriticalSection doesn't
	Common::Allocator& allocator;
	unsigned inCriticalSection;
	
	mutable std::recursive_mutex threadsLock;
	std::unordered_map<std::thread::native_handle_type, std::unique_ptr<ThreadContext>> threads;
	uint32_t lastExitCode;
	
	// wait queues
	std::shared_ptr<WaitQueue<std::string>> sink;
	WaitQueue<ThreadUpdate> changingContexts;
	
	void DebugLoop(ThreadContext& context, bool autostart);
	
public:
	DebugThreadManager(Common::Allocator& allocator);
	
	virtual bool IsThreadExecuting() const override;
	virtual void MarkThreadAsExecuting() override;
	virtual void UnmarkThreadAsExecuting() override;
	
	virtual void EnterCriticalSection() noexcept override;
	virtual void ExitCriticalSection() noexcept override;
	
	std::shared_ptr<WaitQueue<std::string>> GetCommandSink();
	void SetCommandSink(std::shared_ptr<WaitQueue<std::string>>& sink);
	
	void ConsumeThreadEvents(); // expected to run on a dedicated thread
	uint32_t GetLastExitCode() const; // exit code of the last thread to complete
	
	ThreadContext& StartThread(const Common::StackPreparator& stack, size_t stackSize, const PEF::TransitionVector& entryPoint, bool startNow = false);

	size_t ThreadCount() const;
	bool GetThread(std::thread::native_handle_type handle, ThreadContext*& context);
	
	template<typename TAction>
	void ForEachThread(TAction&& action)
	{
		std::lock_guard<std::recursive_mutex> guard(threadsLock);
		for (auto& pair : threads)
		{
			action(*pair.second.get());
		}
	}
};

#endif /* defined(__Classix__DebugThreadManager__) */
