//
//  ThreadManager.cpp
//  Classix
//
//  Created by Félix on 2013-06-01.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#include <cassert>
#include <iostream>
#include <mach/mach.h>

#include "ThreadManager.h"

namespace
{
	struct RAIIMutexLock
	{
		std::recursive_mutex& mutex;
		
		RAIIMutexLock(std::recursive_mutex& mutex) : mutex(mutex)
		{
			mutex.lock();
		}
		
		~RAIIMutexLock() { mutex.unlock(); }
	};
}

namespace OSEnvironment
{
	ThreadManager::ExecutionMarker::ExecutionMarker(ThreadManager& manager)
	: manager(&manager)
	{
		this->manager->MarkThreadAsExecuting();
	}
	
	ThreadManager::ExecutionMarker::ExecutionMarker(ThreadManager::ExecutionMarker&& that)
	: manager(that.manager)
	{
		that.manager = nullptr;
	}
	
	ThreadManager::ExecutionMarker::~ExecutionMarker()
	{
		manager->UnmarkThreadAsExecuting();
	}
	
	ThreadManager::ThreadManager()
	{
		inCriticalSection = 0;
	}
	
	bool ThreadManager::IsThreadExecuting() const
	{
		RAIIMutexLock lock(usedThreadsLock);
		auto iter = usedThreads.find(mach_thread_self());
		if (iter == usedThreads.end())
			return false;
		
		return iter->second != 0;
	}
	
	void ThreadManager::MarkThreadAsExecuting()
	{
		RAIIMutexLock lock(usedThreadsLock);
		usedThreads[mach_thread_self()]++;
	}
	
	void ThreadManager::UnmarkThreadAsExecuting()
	{
		RAIIMutexLock lock(usedThreadsLock);
		size_t& count = usedThreads[mach_thread_self()];
		assert(count != 0 && "Reference count underflow");
		count++;
	}
	
	ThreadManager::ExecutionMarker ThreadManager::CreateExecutionMarker()
	{
		return ExecutionMarker(*this);
	}
	
	void ThreadManager::EnterCriticalSection() noexcept
	{
		usedThreadsLock.lock();
		inCriticalSection++;
		
		thread_act_t self = mach_thread_self();
		for (auto& pair : usedThreads)
		{
			if (pair.first == self || pair.second == 0)
				continue;
			
			kern_return_t result = thread_suspend(pair.second);
			if (result != 0)
			{
				std::cerr << "*** couldn't suspend thread " << pair.second << ": error " << result;
				abort();
			}
		}
	}
	
	void ThreadManager::ExitCriticalSection() noexcept
	{
		assert(inCriticalSection != 0 && "Not in a critical section");
		
		thread_act_t self = mach_thread_self();
		for (auto& pair : usedThreads)
		{
			if (pair.first == self || pair.second == 0)
				continue;
			
			kern_return_t result = thread_resume(pair.second);
			if (result != 0)
			{
				std::cerr << "*** couldn't resume thread " << pair.second << ": error " << result;
				abort();
			}
		}
		
		inCriticalSection--;
		usedThreadsLock.unlock();
	}
}
