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
		struct AllocatedRange
		{
			void* start;
			void* end;
			std::string name;
			
			AllocatedRange();
			AllocatedRange(void* start, void* end, const std::string& name);
		};
		
		std::map<void*, AllocatedRange> ranges;
		
		const AllocatedRange* GetAllocationRange(const void* address) const;
		
	public:
		static NativeAllocator* Instance;
		
		virtual uint8_t* GetBaseAddress();
		
		virtual uint8_t* Allocate(const std::string& reason, size_t size);
		virtual void Deallocate(void* address);
		virtual const std::string* GetRegionOfAllocation(const void* address);
		
		void PrintMemoryMap() const;
		void PrintParentZone(intptr_t address) const;
		
		virtual ~NativeAllocator();
	};
}

#endif /* defined(__pefdump__NativeAllocator__) */
