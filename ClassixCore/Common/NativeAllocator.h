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

#include "IAllocator.h"
#include <map>

namespace Common
{
	class NativeAllocator : public IAllocator
	{
		static NativeAllocator* instance;
		struct AllocatedRange
		{
			void* start;
			void* end;
			AllocationDetails* details;
			
			AllocatedRange();
			AllocatedRange(void* start, void* end, const AllocationDetails& details);
			AllocatedRange(const AllocatedRange& that) = delete;
			AllocatedRange(AllocatedRange&& that);
			
			~AllocatedRange();
		};
		
		std::map<intptr_t, AllocatedRange> ranges;
		
		const AllocatedRange* GetAllocationRange(uint32_t address);
		
	protected:
		virtual void* IntPtrToPointer(uint32_t value);
		virtual uint32_t PointerToIntPtr(void* address);
		
	public:
		static NativeAllocator* GetInstance();
		
		virtual uint8_t* Allocate(const AllocationDetails& details, size_t size);
		virtual void Deallocate(void* address);
		virtual const AllocationDetails* GetDetails(const void* address);
		virtual const AllocationDetails* GetDetails(uint32_t address);
		
		void PrintMemoryMap();
		void PrintParentZone(const void* address);
		
		virtual ~NativeAllocator();
	};
}

#endif /* defined(__pefdump__NativeAllocator__) */
