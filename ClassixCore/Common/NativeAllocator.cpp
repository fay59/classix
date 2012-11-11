//
//  NativeAllocator.cpp
//  pefdump
//
//  Created by Félix on 2012-10-24.
//  Copyright (c) 2012 Félix. All rights reserved.
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
	
	uint8_t* NativeAllocator::Allocate(size_t size, const std::string& reason)
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
	
	void NativeAllocator::PrintMemoryMap() const
	{
		for (auto iter = ranges.begin(); iter != ranges.end(); iter++)
		{
			const auto& range = iter->second;
			std::cout << range.start << " - " << range.end << ": " << range.name << std::endl;
		}
	}
	
	NativeAllocator::~NativeAllocator()
	{ }
}