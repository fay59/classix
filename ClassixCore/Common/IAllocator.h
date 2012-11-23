//
// IAllocator.h
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

#ifndef __pefdump__IAllocator__
#define __pefdump__IAllocator__

#include <cstdint>
#include <cstddef>
#include <string>

namespace Common
{
	class IAllocator;
	
	class AutoAllocation
	{
		void* address;
		IAllocator* allocator;
		
	public:
		AutoAllocation(IAllocator* allocator, size_t size, const std::string& zoneName);
		AutoAllocation(const AutoAllocation& that) = delete;
		AutoAllocation(AutoAllocation&& that);
		
		void* operator*();
		const void* operator*() const;
		
		uint32_t GetVirtualAddress() const;
		
		~AutoAllocation();
	};
	
	// we're not using C++ allocators because we're really more interested in creating big flat memory zones
	// than actualy creating objects; and besides, we don't want to virally add templates to anything that needs
	// to allocate memory.
	class IAllocator
	{
	public:
		virtual uint8_t* GetBaseAddress() = 0;
		inline const uint8_t* GetBaseAddress() const
		{
			return const_cast<IAllocator*>(this)->GetBaseAddress();
		}
		
		virtual uint8_t* Allocate(const std::string& zoneName, size_t size) = 0;
		virtual void Deallocate(void* address) = 0;
		
		AutoAllocation AllocateAuto(const std::string& zoneName, size_t size);
		
		template<typename T, typename ...TParams>
		T* Allocate(const std::string& zoneName, TParams... params)
		{
			T* object = reinterpret_cast<T*>(Allocate(zoneName, sizeof(T)));
			return new (object) T(params...);
		}
		
		template<typename T>
		void Deallocate(T* object)
		{
			object->~T();
			Deallocate(static_cast<void*>(object));
		}
		
		template<typename T>
		T* ToPointer(intptr_t value)
		{
			return reinterpret_cast<T*>(GetBaseAddress() + value);
		}
		
		template<typename T>
		T ToFunctionPointer(intptr_t value)
		{
			return reinterpret_cast<T>(GetBaseAddress() + value);
		}
		
		template<typename T>
		intptr_t ToIntPtr(T* value)
		{
			uint8_t* asUint8 = reinterpret_cast<uint8_t*>(value);
			return static_cast<intptr_t>(asUint8 - GetBaseAddress());
		}
		
		template<typename T>
		intptr_t ToIntPtr(const T* value)
		{
			const uint8_t* asUInt8 = reinterpret_cast<const uint8_t*>(value);
			return static_cast<intptr_t>(asUInt8 - GetBaseAddress());
		}
		
		virtual ~IAllocator();
	};
}

#endif /* defined(__pefdump__IAllocator__) */
