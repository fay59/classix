//
// VirtualAddressSpaceAllocator.h
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
