//
//  NativeAllocator.cpp
//  pefdump
//
//  Created by Félix on 2012-10-24.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "NativeAllocator.h"

#include <cstdlib>

namespace Common
{
	// nice hack
	NativeAllocator* NativeAllocator::Instance = new NativeAllocator();
	
	uint8_t* NativeAllocator::Allocate(size_t size)
	{
		return static_cast<uint8_t*>(malloc(size));
	}
	
	void NativeAllocator::Deallocate(void* address)
	{
		free(address);
	}
	
	NativeAllocator::~NativeAllocator()
	{ }
}