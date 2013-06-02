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
		mutable std::recursive_mutex usedThreadsLock;
		unsigned inCriticalSection;
		std::unordered_map<thread_act_t, size_t> usedThreads;
		
	public:
		class ExecutionMarker
		{
			friend class ThreadManager;
			ThreadManager* manager;
			ExecutionMarker(ThreadManager& manager);
			
		public:
			ExecutionMarker(const ExecutionMarker& that) = delete;
			ExecutionMarker(ExecutionMarker&& that);
			~ExecutionMarker();
		};
		
		ThreadManager();
		
		bool IsThreadExecuting() const;
		void MarkThreadAsExecuting();
		void UnmarkThreadAsExecuting();
		ExecutionMarker CreateExecutionMarker();
		
		void EnterCriticalSection() noexcept;
		void ExitCriticalSection() noexcept;
	};
}

#endif /* defined(__Classix__ThreadManager__) */
