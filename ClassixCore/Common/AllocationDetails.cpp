//
// AllocationDetails.cpp
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
