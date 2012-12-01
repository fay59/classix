//
//  IAllocationDetails.cpp
//  Classix
//
//  Created by Félix on 2012-12-01.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "AllocationDetails.h"
#include <sstream>

namespace Common
{
	AllocationDetails::AllocationDetails(const std::string& name)
	: allocationName(name)
	{ }
	
	const std::string& AllocationDetails::GetAllocationName() const
	{
		return allocationName;
	}
	
	std::string AllocationDetails::GetAllocationDetails(uint32_t offset) const
	{
		std::stringstream ss;
		ss << allocationName << " +" << offset;
		return ss.str();
	}
	
	AllocationDetails* AllocationDetails::ToHeapAlloc() const
	{
		if (typeid(AllocationDetails) != typeid(*this))
			throw std::logic_error("ToHeapAlloc() was not implemented");
		
		return new AllocationDetails(*this);
	}
	
	AllocationDetails::~AllocationDetails()
	{ }
}
