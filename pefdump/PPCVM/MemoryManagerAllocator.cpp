//
//  MemoryManagerAllocator.cpp
//  pefdump
//
//  Created by Félix on 2012-10-24.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "MemoryManagerAllocator.h"

namespace PPCVM
{
	MemoryManagerAllocator::MemoryManagerAllocator(MemoryManager& manager)
	: manager(manager)
	{ }
	
	uint8_t* MemoryManagerAllocator::Allocate(size_t size)
	{
		uint32_t handle = manager.Allocate(size);
		return manager.Lock(handle);
	}
	
	void MemoryManagerAllocator::Deallocate(void *address)
	{
		manager.Deallocate(static_cast<uint8_t*>(address));
	}
	
	MemoryManagerAllocator::~MemoryManagerAllocator()
	{ }
}
