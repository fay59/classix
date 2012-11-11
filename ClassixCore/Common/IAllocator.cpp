//
//  IAllocator.cpp
//  pefdump
//
//  Created by Félix on 2012-10-24.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "IAllocator.h"

Common::AutoAllocation::AutoAllocation(IAllocator* allocator, size_t size, const std::string& zoneName)
: allocator(allocator)
{
	address = allocator->Allocate(size, zoneName);
}

Common::AutoAllocation::AutoAllocation(AutoAllocation&& that)
: allocator(that.allocator)
{
	address = that.address;
	allocator = nullptr;
	address = nullptr;
}

void* Common::AutoAllocation::operator*()
{
	return address;
}

const void* Common::AutoAllocation::operator*() const
{
	return address;
}

Common::AutoAllocation::~AutoAllocation()
{
	if (allocator != nullptr && address != nullptr)
		allocator->Deallocate(address);
}

Common::AutoAllocation Common::IAllocator::AllocateAuto(size_t size, const std::string& zoneName)
{
	return AutoAllocation(this, size, zoneName);
}

Common::IAllocator::~IAllocator()
{ }