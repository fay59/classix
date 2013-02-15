//
//  VirtualAddressSpaceAllocator.h
//  Classix
//
//  Created by Félix on 2013-02-11.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#ifndef __Classix__VirtualAddressSpaceAllocator__
#define __Classix__VirtualAddressSpaceAllocator__

#include <set>
#include <list>
#include <array>
#include "IAllocator.h"

namespace Common
{
	class VASAllocator : public Common::IAllocator
	{
		struct VASAllocationDetails;
		typedef uint8_t MemoryPage[0x1000];
		
		MemoryPage* addressSpace;
		MemoryPage* nextPage;
		
		std::set<VASAllocationDetails*> freeItems;
		std::array<std::list<VASAllocationDetails*>, 7> freeLists;
		
		uint8_t* AllocateLarge(const Common::AllocationDetails& details, size_t size);
		VASAllocationDetails* GetSmallestMatchingBlock(size_t size, int checkedDegree);
		void Coalesce(std::set<VASAllocationDetails*>::iterator iter);
		void MarkFree(VASAllocationDetails* allocation);
		
	protected:
		virtual void* IntPtrToPointer(uint32_t value) const = 0;
		virtual uint32_t PointerToIntPtr(const void* address) const = 0;
		
	public:
		VASAllocator();
		
		uint8_t* null() const;
		
		virtual uint8_t* Allocate(const Common::AllocationDetails& details, size_t size) override;
		virtual void Deallocate(void* address) override;
		
		virtual const Common::AllocationDetails* GetDetails(uint32_t address) const override;
		virtual uint32_t GetAllocationOffset(uint32_t address) const override;
		
		virtual ~VASAllocator();
	};
}

#endif /* defined(__Classix__VirtualAddressSpaceAllocator__) */
