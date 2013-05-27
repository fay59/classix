//
// NativeAllocator.cpp
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

#include "NativeAllocator.h"

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <unistd.h>
#include <sys/mman.h>

namespace
{
	std::ostream& PrintAddress(std::ostream& into, uint32_t address)
	{
		into << "0x";
		into << std::setw(8) << std::setfill('0') << std::hex << address;
		return into;
	}
	
	const int pageSize = getpagesize();
}

namespace Common
{
	NativeAllocator::AllocatedRange::AllocatedRange()
	: start(nullptr), end(nullptr), details(nullptr)
	{ }
	
	NativeAllocator::AllocatedRange::AllocatedRange(void* start, void* end, const AllocationDetails& details)
	: start(start), end(end), details(details.ToHeapAlloc())
	{ }
	
	NativeAllocator::AllocatedRange::AllocatedRange(AllocatedRange&& that)
	: start(that.start), end(that.end), details(that.details)
	{
		that.start = nullptr;
		that.end = nullptr;
		that.details = nullptr;
	}
	
	NativeAllocator::AllocatedRange::~AllocatedRange()
	{
		delete details;
	}
	
	NativeAllocator::NativeAllocator()
	: invalidPageBegin(nullptr), invalidPageEnd(nullptr)
	{
		if (sizeof(void*) != sizeof(uint32_t))
			throw std::runtime_error("Cannot use the native allocator in a 64-bits environment");
	}
	
	void* NativeAllocator::IntPtrToPointer(uint32_t value) const
	{
		return reinterpret_cast<void*>(value);
	}
	
	uint32_t NativeAllocator::PointerToIntPtr(const void *address) const
	{
		return reinterpret_cast<uint32_t>(address);
	}
	
	uint32_t NativeAllocator::CreateInvalidAddress(const AllocationDetails& reason)
	{
		if (invalidPageBegin == invalidPageEnd)
		{
			invalidPageBegin = (unsigned char*)mmap(nullptr, pageSize, PROT_NONE, MAP_ANON | MAP_PRIVATE, 0, 0);
			assert(invalidPageBegin != MAP_FAILED && "Couldn't map invalid page");
			
			invalidPageEnd = invalidPageBegin + pageSize;
			invalidPages.push_back(invalidPageBegin);
		}
		
		uint8_t* address = invalidPageBegin;
		invalidPageBegin++;
		
		ranges.emplace(std::make_pair(ToIntPtr(address), AllocatedRange(address, invalidPageBegin, reason)));
		return PointerToIntPtr(address);
	}
	
	uint8_t* NativeAllocator::Allocate(const AllocationDetails& reason, size_t size)
	{
		uint8_t* allocation = static_cast<uint8_t*>(malloc(size));
		memset(allocation, ScribbleAllocPattern, size);
		uint32_t address = ToIntPtr(allocation);
		ranges.emplace(std::make_pair(address, AllocatedRange(allocation, allocation + size, reason)));
		return allocation;
	}
	
	void NativeAllocator::Deallocate(void* address)
	{
		auto iter = ranges.find(ToIntPtr(address));
		if (iter != ranges.end())
		{
			free(iter->second.start);
			ranges.erase(iter);
		}
	}
	
	const NativeAllocator::AllocatedRange* NativeAllocator::GetAllocationRange(uint32_t address) const
	{
		for (auto iter = ranges.begin(); iter != ranges.end(); iter++)
		{
			const auto& range = iter->second;
			if (ToIntPtr(range.end) < address)
				continue;
			
			if (ToIntPtr(range.start) > address)
				break;
			
			return &range;
		}
		return nullptr;
	}
	
	const AllocationDetails* NativeAllocator::GetDetails(uint32_t address) const
	{
		auto range = GetAllocationRange(address);
		return range == nullptr ? nullptr : range->details;
	}
	
	uint32_t NativeAllocator::GetAllocationOffset(uint32_t address) const
	{
		auto range = GetAllocationRange(address);
		if (range == nullptr)
			throw AccessViolationException(*this, address, 0);
		
		return address - ToIntPtr(range->start);
	}
	
	void NativeAllocator::PrintMemoryMap() const
	{
		for (auto iter = ranges.begin(); iter != ranges.end(); iter++)
		{
			const auto& range = iter->second;
			uint32_t start = ToIntPtr(range.start);
			uint32_t end = ToIntPtr(range.end);
			
			PrintAddress(std::cout, start);
			std::cout << " - ";
			PrintAddress(std::cout, end);
			std::cout << ": " << range.details->GetAllocationName() << std::endl;
		}
	}
	
	void NativeAllocator::PrintParentZone(const void* address) const
	{
		uint32_t intAddress = ToIntPtr(address);
		auto range = GetAllocationRange(intAddress);
		if (range == nullptr)
			std::cout << address << " was not allocated" << std::endl;
		else
		{
			uint32_t start = ToIntPtr(range->start);
			uint32_t end = ToIntPtr(range->end);
			
			PrintAddress(std::cout, start);
			std::cout << " - ";
			PrintAddress(std::cout, end);
			std::cout << ": " << range->details->GetAllocationName();
			
			std::cout << " (" << range->details->GetAllocationDetails(intAddress - start) << ')' << std::endl;
		}
	}
	
	NativeAllocator::~NativeAllocator()
	{
		for (unsigned char* page : invalidPages)
			munmap(page, pageSize);
	}
}