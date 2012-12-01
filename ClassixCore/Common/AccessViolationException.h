//
//  AccessViolationException.h
//  Classix
//
//  Created by Félix on 2012-12-01.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__AccessViolationException__
#define __Classix__AccessViolationException__

#include "PPCRuntimeException.h"
#include <cstdint>
#include <string>

namespace Common
{
	class IAllocator;
	
	class AccessViolationException : public PPCRuntimeException
	{
		IAllocator* allocator;
		uint32_t address;
		size_t size;
		std::string description;
		
	public:
		static void Check(IAllocator* allocator, uint32_t address, size_t size);
		
		AccessViolationException(IAllocator* allocator, uint32_t address, size_t size);
		
		IAllocator* GetAllocator();
		uint32_t GetAddress() const;
		size_t GetSize() const;
		
		virtual PPCRuntimeException* ToHeapAlloc() const;
		virtual const char* what() const noexcept;
		
		~AccessViolationException();
	};
}

#endif /* defined(__Classix__AccessViolationException__) */
