//
// AccessViolationException.h
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

#ifndef __Classix__AccessViolationException__
#define __Classix__AccessViolationException__

#include "PPCRuntimeException.h"
#include <cstdint>
#include <string>

namespace Common
{
	class Allocator;
	
	class AccessViolationException : public PPCRuntimeException
	{
		const Allocator& allocator;
		uint32_t address;
		size_t size;
		std::string description;
		
	public:
		static void Check(const Allocator& allocator, uint32_t address, size_t size);
		
		AccessViolationException(const Allocator& allocator, uint32_t address, size_t size);
		
		const Allocator& GetAllocator() const;
		uint32_t GetAddress() const;
		size_t GetSize() const;
		
		virtual PPCRuntimeException* ToHeapAlloc() const override;
		virtual const char* what() const noexcept override;
		
		virtual ~AccessViolationException() override;
	};
}

#endif /* defined(__Classix__AccessViolationException__) */
