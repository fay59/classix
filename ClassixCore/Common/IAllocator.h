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
#include <cassert>
#include <string>
#include <utility>
#include "AllocationDetails.h"
#include "AccessViolationException.h"
#include "BigEndian.h"

namespace Common
{
	class IAllocator;
	
	class AutoAllocation
	{
		uint32_t address;
		IAllocator& allocator;
		
	public:
		AutoAllocation(IAllocator& allocator, size_t size, const AllocationDetails& details);
		AutoAllocation(const AutoAllocation& that) = delete;
		AutoAllocation(AutoAllocation&& that);
		
		void* operator*();
		const void* operator*() const;
		
		template<typename T> T* ToPointer();
		template<typename T> T* ToArray(size_t count);
		
		uint32_t GetVirtualAddress() const;
		
		~AutoAllocation();
	};
	
#pragma mark -
	// we're not using C++ allocators because we're really more interested in creating big flat memory zones
	// than actualy creating objects; and besides, we don't want to virally add templates to anything that needs
	// to allocate memory.
	class IAllocator
	{
	public:
#pragma mark Virtual Interface
		virtual uint8_t* Allocate(const AllocationDetails& details, size_t size) = 0;
		virtual void Deallocate(void* address) = 0;
		
		virtual const AllocationDetails* GetDetails(uint32_t address) const = 0;
		virtual uint32_t GetAllocationOffset(uint32_t address) const = 0;
		
		virtual ~IAllocator();
		
	protected:
		virtual void* IntPtrToPointer(uint32_t value) const = 0;
		virtual uint32_t PointerToIntPtr(const void* address) const = 0;
		
#pragma mark -
	public:
		enum IAllocatorConstants
		{
			ScribbleAllocPattern = 0xaf,
			ScribbleFreePattern = 0xbe,
			ScribbleStackPattern = 0xcd
		};
		
		virtual uint32_t CreateInvalidAddress(const AllocationDetails& details) = 0;
		
		uint8_t* Allocate(const std::string& zoneName, size_t size);
		AutoAllocation AllocateAuto(const std::string& zoneName, size_t size);
		AutoAllocation AllocateAuto(const AllocationDetails& details, size_t size);
		
		inline const AllocationDetails* GetDetails(const void* address) const
		{
			return GetDetails(ToIntPtr(address));
		}
		
		inline uint32_t GetAllocationOffset(const void* address) const
		{
			return GetAllocationOffset(ToIntPtr(address));
		}
		
		inline uint32_t CreateInvalidAddress(const std::string& name)
		{
			return CreateInvalidAddress(AllocationDetails(name, 1));
		}
		
		inline bool IsAllocated(uint32_t address) const
		{
			return GetDetails(address) != nullptr;
		}
		
		template<typename T>
		T* Bless(T* pointer)
		{
			AccessViolationException::Check(*this, ToIntPtr(pointer), sizeof(T));
			return pointer;
		}
		
		template<typename T>
		const T* Bless(const T* pointer)
		{
			AccessViolationException::Check(*this, ToIntPtr(pointer), sizeof(T));
			return pointer;
		}
		
		template<typename T, typename ...TParams>
		T* Allocate(const std::string& zoneName, TParams&&... params)
		{
			return Allocate<T>(AllocationDetails(zoneName, sizeof(T)), std::forward<TParams>(params)...);
		}
		
		template<typename T, typename ...TParams>
		T* Allocate(const AllocationDetails& details, TParams&&... params)
		{
			T* object = reinterpret_cast<T*>(Allocate(details, sizeof(T)));
			return new (object) T(std::forward<TParams>(params)...);
		}
		
		template<typename T>
		void Deallocate(T* object)
		{
			object->~T();
			Deallocate(static_cast<void*>(object));
		}
		
		template<typename T>
		T* ToPointer(uint32_t value, bool allowNullptr = false) const
		{
#ifdef DEBUG
			if (!allowNullptr || value != 0)
			{
				AccessViolationException::Check(*this, value, sizeof(T));
			}
#endif
			return reinterpret_cast<T*>(IntPtrToPointer(value));
		}
		
		template<typename T>
		T* ToArray(uint32_t value, size_t count, bool allowNullptr = false) const
		{
#ifdef DEBUG
			if (!allowNullptr || value != 0)
			{
				AccessViolationException::Check(*this, value, sizeof(T) * count);
			}
#endif
			return reinterpret_cast<T*>(IntPtrToPointer(value));
		}
		
		template<typename T>
		uint32_t ToIntPtr(T* value) const
		{
			return PointerToIntPtr(value);
		}
	};
	
	template<typename T>
	T* AutoAllocation::ToPointer()
	{
		return allocator.ToPointer<T>(address);
	}
	
	template<typename T>
	T* AutoAllocation::ToArray(size_t count)
	{
		return allocator.ToArray<T>(address, count);
	}
}

#endif /* defined(__pefdump__IAllocator__) */
