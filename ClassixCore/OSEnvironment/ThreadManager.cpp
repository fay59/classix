//
// ThreadManager.cpp
// Classix
//
// Copyright (C) 2013 Félix Cloutier
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
