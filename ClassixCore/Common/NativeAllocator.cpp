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

namespace
{
	std::ostream& PrintAddress(std::ostream& into, uint32_t address)
	{
		into << "0x";
		into << std::setw(8) << std::setfill('0') << std::hex << address;
		return into;
	}
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
	
	NativeAllocator* NativeAllocator::instance = new NativeAllocator();
	
	NativeAllocator* NativeAllocator::GetInstance()
	{
		// gotta love bit paranoia
		if (sizeof(void*) != sizeof(uint32_t))
			throw std::logic_error("NativeAllocator32 is only usable in 32-bits mode");
		
		return instance;
	}
	
	void* NativeAllocator::IntPtrToPointer(uint32_t value) const
	{
		return reinterpret_cast<void*>(value);
	}
	
	uint32_t NativeAllocator::PointerToIntPtr(const void *address) const
	{
		return reinterpret_cast<uint32_t>(address);
	}
	
	uint8_t* NativeAllocator::Allocate(const AllocationDetails& reason, size_t size)
	{
		uint8_t* allocation = static_cast<uint8_t*>(malloc(size));
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
		NativeAllocator* self = const_cast<NativeAllocator*>(this);
		for (auto iter = ranges.begin(); iter != ranges.end(); iter++)
		{
			const auto& range = iter->second;
			if (self->ToIntPtr(range.end) < address)
				continue;
			
			if (self->ToIntPtr(range.start) > address)
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
			throw AccessViolationException(this, address, 0);
		
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
		uint32_t intAddress = ToIntPtr(const_cast<void*>(address));
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
	{ }
}