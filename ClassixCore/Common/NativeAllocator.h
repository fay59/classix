//
//  NativeAllocator.h
//  pefdump
//
//  Created by Félix on 2012-10-24.
//  Copyright (c) 2012 Félix. All rights reserved.
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
		
	public:
		static NativeAllocator* Instance;
		
		virtual uint8_t* GetBaseAddress();
		
		virtual uint8_t* Allocate(const std::string& reason, size_t size);
		virtual void Deallocate(void* address);
		
		void PrintMemoryMap() const;
		
		virtual ~NativeAllocator();
	};
}

#endif /* defined(__pefdump__NativeAllocator__) */
