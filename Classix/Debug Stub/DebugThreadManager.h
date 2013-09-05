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
#include <functional>
#include <condition_variable>

#include "ThreadManager.h"
#include "MachineState.h"
#include "Interpreter.h"
#include "StackPreparator.h"
#include "Structures.h"
#include "Breakpoint.h"
#include "WaitQueue.h"

class DebugThreadManager;
class BreakpointSet;
class ThreadContext;
enum class ThreadState;

struct ThreadUpdate
{
	ThreadContext& context;
	ThreadState state;
	
	explicit ThreadUpdate(ThreadContext& context);
};

class ThreadContextPointer
{
	friend class DebugThreadManager;
	
	std::unique_lock<std::mutex> threadLock;
	ThreadContext* context;
	
	ThreadContextPointer();
	ThreadContextPointer(std::unique_lock<std::mutex>&& threadLock, ThreadContext* context);
	ThreadContextPointer(const ThreadContextPointer& that) = delete;
	
public:
	ThreadContextPointer(ThreadContextPointer&& that);
	
	operator bool() const;
	bool operator==(std::nullptr_t) const;
	bool operator!=(std::nullptr_t) const;
	
	ThreadContext* operator->();
	const ThreadContext* operator->() const;
	ThreadContext& operator*();
	const ThreadContext& operator*() const;
};

class DebugThreadManager : public OSEnvironment::ThreadManager
{
	friend class ThreadContext;
	friend class Breakpoint;
	
public:
	typedef uint32_t ThreadId;
	
	DebugThreadManager(Common::Allocator& allocator);
	
	virtual bool IsThreadExecuting() const override;
	virtual void MarkThreadAsExecuting() override;
	virtual void UnmarkThreadAsExecuting() override;
	
	virtual void EnterCriticalSection() noexcept override;
	virtual void ExitCriticalSection() noexcept override;
	
	std::shared_ptr<WaitQueue<std::string>>& CommandSink();
	std::shared_ptr<BreakpointSet>& BreakpointSet();
	
	void ConsumeThreadEvents(); // expected to run on a dedicated thread
	uint32_t GetLastExitCode() const; // exit code of the last thread to complete
	
	ThreadContext& StartThread(const Common::StackPreparator& stack, size_t stackSize, const PEF::TransitionVector& entryPoint, bool startNow = false);

	bool HasCompleted() const;
	ThreadContextPointer GetThread(ThreadId handle);
	
	template<typename TAction>
	void ForEachThread(TAction&& action)
	{
		std::lock_guard<std::mutex> guard(threadsLock);
		for (auto& pair : threads)
		{
			action(*pair.second.get());
		}
	}
	
private:
	Common::Allocator& allocator;
	unsigned inCriticalSection;
	
	mutable std::mutex threadsLock;
	ThreadId nextId;
	std::unordered_map<ThreadId, std::unique_ptr<ThreadContext>> threads;
	uint32_t lastExitCode;
	
	std::shared_ptr<::BreakpointSet> breakpoints;
	
	// wait queues
	std::shared_ptr<WaitQueue<std::string>> sink;
	WaitQueue<ThreadUpdate> changingContexts;
	
	void DebugLoop(ThreadContext& context, bool autostart);
};

#endif /* defined(__Classix__DebugThreadManager__) */
