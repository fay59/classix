//
//  VirtualAddressSpaceAllocator.cpp
//  Classix
//
//  Created by Félix on 2013-02-11.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#include "VirtualAddressSpaceAllocator.h"

#include <new>
#include <cstring>

#include <sys/mman.h>

namespace
{
	size_t addressSpaceSize = 0x100000000;
	
	inline int AllocationDegree(size_t size)
	{
		int degree = 63 - __builtin_clzll(size);
		if (size & ((1 << degree) - 1))
			degree++;
		
		return std::max(0, degree - 5);
	}
}

namespace Common
{
	struct VASAllocator::VASAllocationDetails : public Common::AllocationDetails
	{
		static uint64_t magicValue;
		
		std::list<VASAllocationDetails*>::iterator location;
		size_t size;
		uint64_t magic;
		uint8_t bytes[0];
		
		VASAllocationDetails(const std::string& reason, size_t size)
		: AllocationDetails(reason)
		{
			this->magic = magicValue;
			this->size = size;
		}
		
		VASAllocationDetails(size_t size)
		: AllocationDetails("")
		{
			this->magic = magicValue;
			this->size = size;
		}
		
		inline void SetName(const std::string& name)
		{
			AllocationDetails::allocationName = name;
		}
		
		inline size_t TotalSize() const
		{
			return sizeof *this + size;
		}
		
		inline uint8_t* BaseAddress()
		{
			return reinterpret_cast<uint8_t*>(this);
		}
		
		inline uint8_t* begin()
		{
			return bytes;
		}
		
		inline uint8_t* end()
		{
			return bytes + size;
		}
		
		virtual ~VASAllocationDetails()
		{ }
	};
	
	uint64_t VASAllocator::VASAllocationDetails::magicValue = 0x56415341;
	
	VASAllocator::VASAllocator()
	{
#ifndef __LP64__
		throw std::runtime_error("Cannot use the virtual address space allocator in 32-bits mode");
#else
		addressSpace = (MemoryPage*)mmap(nullptr, addressSpaceSize, PROT_NONE, MAP_ANON | MAP_PRIVATE, -1, 0);
		if (addressSpace == MAP_FAILED)
		{
			throw std::runtime_error("Could not allocate virtual address space");
		}
		
		nextPage = addressSpace + 0x4000; // 16 MB between "null" and the first allocations
#endif
	}
	
	VASAllocator::VASAllocationDetails* VASAllocator::GetSmallestMatchingBlock(size_t size, int checkedDegree)
	{
		if (checkedDegree == freeLists.size())
		{
			int result = mprotect(nextPage, sizeof *nextPage, PROT_READ | PROT_WRITE);
			if (result != 0)
				throw std::runtime_error("mprotect failed to allocate new page for read-write access");
			
			uint8_t* buffer = &nextPage[0][0];
			nextPage++;
			
			return new (buffer) VASAllocationDetails(0x1000 - sizeof (VASAllocationDetails));
		}
		
		std::list<VASAllocationDetails*>& list = freeLists.at(checkedDegree);
		if (list.size() > 0)
		{
			VASAllocationDetails* alloc = list.front();
			list.pop_front();
			freeItems.erase(alloc);
			return alloc;
		}
		
		int expectedMinimumSize = 1 << (checkedDegree + 5);
		
		VASAllocationDetails* biggerAlloc = GetSmallestMatchingBlock(size, checkedDegree + 1);
		while (biggerAlloc->TotalSize() >= expectedMinimumSize * 2)
		{
			size_t halfChunkSize = expectedMinimumSize - sizeof(VASAllocationDetails);
			uint8_t* halfChunkStart = biggerAlloc->bytes + biggerAlloc->size - halfChunkSize;
			VASAllocationDetails* halfAlloc = new (halfChunkStart) VASAllocationDetails(expectedMinimumSize);
			MarkFree(halfAlloc);
			
			biggerAlloc->size -= halfChunkSize;
		}
		return biggerAlloc;
	}
	
	void VASAllocator::Coalesce(std::set<VASAllocationDetails *>::iterator iter)
	{
		VASAllocationDetails* current = *iter;
		iter++;
		VASAllocationDetails* next = *iter;
		
		int newDegree = AllocationDegree(current->size + next->TotalSize());
		if (newDegree < freeLists.size())
		{
			int currentDegree = AllocationDegree(current->size);
			
			freeItems.erase(iter);
			current->size += next->TotalSize();
			next->~VASAllocationDetails();
			
			if (currentDegree != newDegree)
			{
				freeLists.at(currentDegree).erase(current->location);
				
				auto newList = freeLists.at(newDegree);
				current->location = newList.insert(newList.end(), current);
			}
		}
	}
	
	void VASAllocator::MarkFree(Common::VASAllocator::VASAllocationDetails *allocation)
	{
		allocation->SetName("<free>");
		auto pair = freeItems.insert(allocation);
		
		auto currentLocation = pair.first;
		auto rewindIter = pair.first;
		while (rewindIter != freeItems.begin())
		{
			rewindIter--;
			VASAllocationDetails* previousAllocation = *rewindIter;
			VASAllocationDetails* coalescedAllocation = *currentLocation;
			if (previousAllocation->end() == coalescedAllocation->BaseAddress())
			{
				Coalesce(rewindIter);
				currentLocation = rewindIter;
			}
			else break;
		}
		
		auto forwardIter = currentLocation;
		forwardIter++;
		while (forwardIter != freeItems.end())
		{
			VASAllocationDetails* currentAllocation = *currentLocation;
			VASAllocationDetails* coalescedAllocation = *forwardIter;
			if (currentAllocation->end() == coalescedAllocation->BaseAddress())
			{
				Coalesce(forwardIter);
				forwardIter = currentLocation;
			}
			else break;
			
			forwardIter++;
		}
	}
	
	uint8_t* VASAllocator::null() const
	{
		return &addressSpace[0][0];
	}
	
	void* VASAllocator::IntPtrToPointer(uint32_t address) const
	{
		return &addressSpace[address >> 12][address & 0xfff];
	}
	
	uint32_t VASAllocator::PointerToIntPtr(const void *address) const
	{
		const uint8_t* root = &addressSpace[0][0];
		const uint8_t* addressAsUint8 = static_cast<const uint8_t*>(address);
		return static_cast<uint32_t>(addressAsUint8 - root);
	}
	
	uint8_t* VASAllocator::Allocate(const Common::AllocationDetails &details, size_t size)
	{
		if (size == 0)
			return null();
		
		int degree = AllocationDegree(size);
		if (degree >= freeLists.size())
			return AllocateLarge(details, size);
		
		VASAllocationDetails* allocation = GetSmallestMatchingBlock(1 << (degree + 5), degree);
		allocation->SetName(details.GetAllocationName());
		return allocation->begin();
	}
	
	VASAllocator::~VASAllocator()
	{
		munmap(addressSpace, addressSpaceSize);
	}
}
