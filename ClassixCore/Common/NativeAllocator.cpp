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

namespace Common
{
	NativeAllocator::AllocatedRange::AllocatedRange(void* start, void* end, const std::string& name)
	: start(start), end(end), name(name)
	{ }
	
	NativeAllocator::AllocatedRange::AllocatedRange()
	: start(0), end(0)
	{ }
	
	NativeAllocator* NativeAllocator::Instance = new NativeAllocator();
	
	uint8_t* NativeAllocator::GetBaseAddress()
	{
		return nullptr;
	}
	
	uint8_t* NativeAllocator::Allocate(const std::string& reason, size_t size)
	{
		uint8_t* allocation = static_cast<uint8_t*>(malloc(size));
		ranges[allocation] = AllocatedRange(allocation, allocation + size, reason);
		return allocation;
	}
	
	void NativeAllocator::Deallocate(void* address)
	{
		ranges.erase(address);
		free(address);
	}
	
	const NativeAllocator::AllocatedRange* NativeAllocator::GetAllocationRange(const void *address) const
	{
		for (auto iter = ranges.begin(); iter != ranges.end(); iter++)
		{
			const auto& range = iter->second;
			if (range.end < address)
				continue;
			
			if (range.start > address)
				break;
			
			return &range;
		}
		return nullptr;
	}
	
	const std::string* NativeAllocator::GetRegionOfAllocation(const void *address)
	{
		auto range = GetAllocationRange(address);
		return range == nullptr ? nullptr : &range->name;
	}
	
	void NativeAllocator::PrintMemoryMap() const
	{
		for (auto iter = ranges.begin(); iter != ranges.end(); iter++)
		{
			const auto& range = iter->second;
			std::cout << range.start << " - " << range.end << ": " << range.name << std::endl;
		}
	}
	
	void NativeAllocator::PrintParentZone(intptr_t address) const
	{
		auto range = GetAllocationRange(const_cast<NativeAllocator*>(this)->ToPointer<const void>(address));
		if (range == nullptr)
			std::cout << address << " was not allocated" << std::endl;
		else
			std::cout << range->start << " - " << range->end << ": " << range->name << std::endl;
	}
	
	NativeAllocator::~NativeAllocator()
	{ }
}