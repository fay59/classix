//
//  ThreadManager.h
//  Classix
//
//  Created by Félix on 2013-06-01.
//  Copyright (c) 2013 Félix. All rights reserved.
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
