//
// AccessViolationException.cpp
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

#include "AccessViolationException.h"
#include "IAllocator.h"
#include <sstream>
#include <iomanip>

namespace Common
{
	void AccessViolationException::Check(Common::IAllocator *allocator, uint32_t address, size_t size)
	{
		if (!(allocator->IsAllocated(address) && allocator->IsAllocated(address + size - 1)))
			throw AccessViolationException(allocator, address, size);
	}
	
	AccessViolationException::AccessViolationException(IAllocator* allocator, uint32_t address, size_t size)
	: allocator(allocator), address(address), size(size)
	{
		std::stringstream ss;
		ss << "Cannot access memory location at address 0x";
		ss << std::hex << std::setw(8) << std::setfill('0') << address;
		ss << " - 0x";
		ss << std::hex << std::setw(8) << std::setfill('0') << address + size;
		description = ss.str();
	}
	
	IAllocator* AccessViolationException::GetAllocator()
	{
		return allocator;
	}
	
	uint32_t AccessViolationException::GetAddress() const
	{
		return address;
	}
	
	size_t AccessViolationException::GetSize() const
	{
		return size;
	}
	
	PPCRuntimeException* AccessViolationException::ToHeapAlloc() const
	{
		return new AccessViolationException(*this);
	}
	
	const char* AccessViolationException::what() const noexcept
	{
		return description.c_str();
	}
	
	AccessViolationException::~AccessViolationException()
	{ }
}
