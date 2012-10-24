//
//  MemoryManager.h
//  pefdump
//
//  Created by Félix on 2012-10-23.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__MemoryManager__
#define __pefdump__MemoryManager__

#include <cstdint>
#include <unordered_map>
#include <map>
#include <list>
#include "IAllocator.h"

namespace PPCVM
{
	class MemoryManager
	{
		struct Allocation
		{
			uint8_t* address;
			size_t size;
			uint32_t lockCount;
			
			Allocation();
			Allocation(uint8_t* address, size_t size);
			
			inline bool IsLocked() { return lockCount != 0; }
		};
		
		uint8_t* begin;
		uint8_t* nextEmptyBlock;
		
		std::unordered_map<uint32_t, Allocation> allocations;
		std::map<uint8_t*, uint32_t> allocationsByAddress;
		std::list<uint32_t> recycledHandles;
		
		bool sizeLocked;
		size_t currentSize;
		
		void CompactHeap();
		
	public:
		class AddressLock
		{
			Allocation* allocation;
		public:
			AddressLock(Allocation& allocation);
			AddressLock(const AddressLock& that) = delete;
			AddressLock(AddressLock&& that);
			
			uint8_t* GetAddress();
			const uint8_t* GetAddress() const;
			
			~AddressLock();
		};
		
		MemoryManager();
		
		bool ScribbleFreedMemory;
		
		const uint8_t* GetBaseAddress() const;
		
		size_t GetReservedSize() const;
		void Reserve(size_t size);
		void ReserveAdditional(size_t size);
		void LockReservedSize();
		
		uint32_t Allocate(size_t size);
		void Deallocate(uint32_t handle);
		void Deallocate(uint8_t* address);
		
		uint8_t* Lock(uint32_t handle);
		void Unlock(uint32_t handle);
		AddressLock AcquireLock(uint32_t handle);
		
		~MemoryManager();
	};
}

#endif /* defined(__pefdump__MemoryManager__) */
