//
//  IAllocationDetails.h
//  Classix
//
//  Created by Félix on 2012-12-01.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__IAllocationDetails__
#define __Classix__IAllocationDetails__

#include <string>

namespace Common
{
	class AllocationDetails
	{
		std::string allocationName;
		
	public:
		AllocationDetails(const std::string& name);
		
		const std::string& GetAllocationName() const;
		virtual std::string GetAllocationDetails(uint32_t offset) const;
		virtual AllocationDetails* ToHeapAlloc() const;
		
		virtual ~AllocationDetails();
	};
}

#endif /* defined(__Classix__IAllocationDetails__) */
