//
//  AccessViolationException.cpp
//  Classix
//
//  Created by Félix on 2012-12-01.
//  Copyright (c) 2012 Félix. All rights reserved.
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
