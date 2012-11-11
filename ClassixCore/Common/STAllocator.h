//
//  STAllocator.h
//  pefdump
//
//  Created by Félix on 2012-11-10.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__STAllocator__
#define __pefdump__STAllocator__

#include "IAllocator.h"
#include <cstdint>
#include <string>

namespace Common
{
	template<typename T>
	class STAllocator
	{
		template<typename U>
		friend class STAllocator;
		
		IAllocator* allocator;
		const std::string* forcedNextName;
		
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef const T* const_pointer;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		
		std::string NextName;
		
		STAllocator(IAllocator* allocator)
		: allocator(allocator), forcedNextName(nullptr)
		{ }
		
		template<typename U>
		STAllocator(const STAllocator<U>& that)
		: allocator(that.allocator)
		{
			forcedNextName = that.forcedNextName != nullptr ? that.forcedNextName : &that.NextName;
		}
		
		pointer address(reference x) const { return &x; }
		const_pointer address(const_reference x) const { return &x; }
		
		pointer allocate(size_type n, void* hint = nullptr)
		{
			const std::string* toUse = forcedNextName ? forcedNextName : &NextName;
			pointer result = reinterpret_cast<pointer>(allocator->Allocate(*toUse, sizeof(T) * n));
			return result;
		}
		
		void deallocate(pointer p, size_type n)
		{
			allocator->Deallocate(p);
		}
		
		void construct(pointer p, const_reference copyFrom)
		{
			new (p) T(copyFrom);
		}
		
		void destroy(pointer p)
		{
			p->~T();
		}
	};
}

#endif /* defined(__pefdump__STAllocator__) */
