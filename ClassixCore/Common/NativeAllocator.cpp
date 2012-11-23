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
		intptr_t address = ToIntPtr(allocation);
		ranges[address] = AllocatedRange(allocation, allocation + size, reason);
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
	
	const NativeAllocator::AllocatedRange* NativeAllocator::GetAllocationRange(intptr_t address)
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
	
	const std::string* NativeAllocator::GetRegionOfAllocation(const void* address)
	{
		return GetRegionOfAllocation(ToIntPtr(address));
	}
	
	const std::string* NativeAllocator::GetRegionOfAllocation(intptr_t address)
	{
		auto range = GetAllocationRange(address);
		return range == nullptr ? nullptr : &range->name;
	}
	
	void NativeAllocator::PrintMemoryMap()
	{
		for (auto iter = ranges.begin(); iter != ranges.end(); iter++)
		{
			const auto& range = iter->second;
			std::cout << range.start << " - " << range.end << ": " << range.name << std::endl;
		}
	}
	
	void NativeAllocator::PrintParentZone(intptr_t address)
	{
		auto range = GetAllocationRange(address);
		if (range == nullptr)
			std::cout << address << " was not allocated" << std::endl;
		else
			std::cout << range->start << " - " << range->end << ": " << range->name << std::endl;
	}
	
	NativeAllocator::~NativeAllocator()
	{ }
}