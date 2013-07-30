//
// STAllocator.h
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

#ifndef __pefdump__STAllocator__
#define __pefdump__STAllocator__

#include "Allocator.h"
#include <cstdint>
#include <string>
#include <memory>

namespace Common
{
	template<typename T>
	class STAllocator
	{
		template<typename U>
		friend class STAllocator;
		
		Allocator& allocator;
		std::shared_ptr<std::string> nextName;
		
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef const T* const_pointer;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		
		void SetNextName(const std::string& str)
		{
			*nextName = str;
		}
		
		STAllocator(Allocator& allocator)
		: allocator(allocator), nextName(new std::string)
		{ }
		
		template<typename U>
		STAllocator(const STAllocator<U>& that)
		: allocator(that.allocator), nextName(that.nextName)
		{ }
		
		pointer address(reference x) const { return &x; }
		const_pointer address(const_reference x) const { return &x; }
		
		pointer allocate(size_type n, void* hint = nullptr)
		{
			pointer result = reinterpret_cast<pointer>(allocator.Allocate(*nextName, sizeof(T) * n));
			return result;
		}
		
		void deallocate(pointer p, size_type n)
		{
			allocator.Deallocate(p);
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
