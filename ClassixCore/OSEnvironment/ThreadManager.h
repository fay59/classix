//
// ThreadManager.h
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

#ifndef __Classix__ThreadManager__
#define __Classix__ThreadManager__

#include <mach/thread_act.h>
#include <unordered_map>
#include <cstdint>
#include <mutex>

namespace OSEnvironment
{
	class ThreadManager
	{
	public:
		class ExecutionMarker
		{
			friend class ThreadManager;
			ThreadManager& manager;
			ExecutionMarker(ThreadManager& manager);
			
		public:
			ExecutionMarker(const ExecutionMarker& that) = delete;
			ExecutionMarker(ExecutionMarker&& that);
			~ExecutionMarker();
		};
		
		virtual bool IsThreadExecuting() const = 0;
		virtual void MarkThreadAsExecuting() = 0;
		virtual void UnmarkThreadAsExecuting() = 0;
		ExecutionMarker CreateExecutionMarker();
		
		virtual void EnterCriticalSection() noexcept = 0;
		virtual void ExitCriticalSection() noexcept = 0;
		
		virtual ~ThreadManager();
	};
	
	class NativeThreadManager : public ThreadManager
	{
		// needs to be a recursive mutex so EnterCriticalSection doesn't
		mutable std::recursive_mutex usedThreadsLock;
		unsigned inCriticalSection;
		std::unordered_map<thread_act_t, size_t> usedThreads;
		
	public:
		NativeThreadManager();
		
		virtual bool IsThreadExecuting() const override;
		virtual void MarkThreadAsExecuting() override;
		virtual void UnmarkThreadAsExecuting() override;
		
		virtual void EnterCriticalSection() noexcept override;
		virtual void ExitCriticalSection() noexcept override;
	};
}

#endif /* defined(__Classix__ThreadManager__) */
