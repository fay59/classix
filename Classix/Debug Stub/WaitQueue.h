//
// WaitQueue.h
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

#ifndef __Classix__WaitQueue__
#define __Classix__WaitQueue__

#include <mutex>
#include <queue>
#include <chrono>
#include <atomic>
#include <utility>
#include <condition_variable>
#include <iostream>

template<typename T>
class WaitQueue
{
	std::queue<T> queued;
	std::mutex waitMutex;
	std::condition_variable waitCv;
	
public:
	T TakeOne()
	{
		std::unique_lock<std::mutex> guard(waitMutex);
		waitCv.wait(guard, [this] { return !queued.empty(); });
		T element = std::move(queued.front());
		queued.pop();
		return std::move(element);
	}
	
	template<typename TRep, typename TPeriod>
	bool TakeOne(T& element, const std::chrono::duration<TRep, TPeriod>& time = std::chrono::milliseconds(0))
	{
		std::unique_lock<std::mutex> guard(waitMutex);
		if (waitCv.wait_for(guard, time, [this] { return !queued.empty(); }))
		{
			element = std::move(queued.front());
			queued.pop();
			return true;
		}
		return false;
	}
	
	template<typename U>
	void PutOne(U&& element)
	{
		std::unique_lock<std::mutex> guard(waitMutex);
		queued.push(std::forward<U>(element));
		waitCv.notify_one();
	}
	
	void Clear()
	{
		std::lock_guard<std::mutex> guard(waitMutex);
		while (queued.size() > 0)
			queued.pop();
	}
};

#endif /* defined(__Classix__WaitQueue__) */
