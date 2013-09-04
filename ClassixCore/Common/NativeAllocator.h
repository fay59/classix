//
// NativeAllocator.h
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

#ifndef __pefdump__NativeAllocator__
#define __pefdump__NativeAllocator__

#include "Allocator.h"
#include <map>
#include <deque>
#include <memory>

namespace Common
{
	class NativeAllocator : public Allocator
	{
		struct AllocatedRange
		{
			void* start;
			void* end;
			std::shared_ptr<AllocationDetails> details;
			
			AllocatedRange();
			AllocatedRange(void* start, void* end, const AllocationDetails& details);
			AllocatedRange(const AllocatedRange& that) = delete;
			AllocatedRange(AllocatedRange&& that);
		};
		
		std::map<uint32_t, AllocatedRange> ranges;
		std::deque<unsigned char*> invalidPages;
		unsigned char* invalidPageBegin;
		unsigned char* invalidPageEnd;
		
		const AllocatedRange* GetAllocationRange(uint32_t address) const;
		
	protected:
		virtual void* IntPtrToPointer(uint32_t value) const override;
		virtual uint32_t PointerToIntPtr(const void* address) const override;
		
	public:
		NativeAllocator();
		
		virtual uint32_t CreateInvalidAddress(const AllocationDetails& reason) override;
		virtual uint8_t* Allocate(const AllocationDetails& details, size_t size) override;
		virtual void Deallocate(void* address) override;
		virtual std::shared_ptr<const AllocationDetails> GetDetails(uint32_t address) const override;
		virtual uint32_t GetNextAllocation(uint32_t address) const override;
		virtual uint32_t GetAllocationOffset(uint32_t address) const override;
		
		void PrintMemoryMap() const;
		void PrintParentZone(const void* address) const;
		
		virtual ~NativeAllocator() override;
	};
}

#endif /* defined(__pefdump__NativeAllocator__) */
