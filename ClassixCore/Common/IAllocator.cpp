//
// IAllocator.cpp
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

#include "IAllocator.h"

namespace Common
{
	AutoAllocation::AutoAllocation(IAllocator* allocator, size_t size, const std::string& zoneName)
	: allocator(allocator)
	{
		address = allocator->Allocate(zoneName, size);
	}

	AutoAllocation::AutoAllocation(AutoAllocation&& that)
	: allocator(that.allocator)
	{
		address = that.address;
		that.allocator = nullptr;
		that.address = nullptr;
	}

	void* AutoAllocation::operator*()
	{
		return address;
	}

	const void* AutoAllocation::operator*() const
	{
		return address;
	}

	uint32_t AutoAllocation::GetVirtualAddress() const
	{
		return allocator->ToIntPtr(address);
	}

	AutoAllocation::~AutoAllocation()
	{
		if (allocator != nullptr && address != nullptr)
			allocator->Deallocate(address);
	}

	AutoAllocation IAllocator::AllocateAuto(const std::string& zoneName, size_t size)
	{
		return AutoAllocation(this, size, zoneName);
	}
	
	bool IAllocator::IsAllocated(const void *address)
	{
		return GetRegionOfAllocation(address) != nullptr;
	}

	IAllocator::~IAllocator()
	{ }
}