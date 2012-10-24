//
//  MemoryManagerAllocator.h
//  pefdump
//
//  Created by Félix on 2012-10-24.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__MemoryManagerAllocator__
#define __pefdump__MemoryManagerAllocator__

#include "IAllocator.h"
#include "MemoryManager.h"

namespace PPCVM
{
	class MemoryManagerAllocator : public Common::IAllocator
	{
		MemoryManager& manager;
		
	public:
		MemoryManagerAllocator(MemoryManager& manager);
		
		virtual uint8_t* Allocate(size_t size);
		virtual void Deallocate(void* address);
		virtual ~MemoryManagerAllocator();
	};
}

#endif /* defined(__pefdump__MemoryManagerAllocator__) */
