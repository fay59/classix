//
// FourCharCode.h
// Classix
//
// Copyright (C) 2013 Félix Cloutier
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

#ifndef __Classix__FourCharCode__
#define __Classix__FourCharCode__

#include <iostream>
#include <cassert>
#include <cstdint>

namespace Common
{
	struct FourCharCode
	{
		uint32_t code;
		
		inline FourCharCode(uint32_t code) : code(code)
		{}
		
		inline FourCharCode(const char (&array)[5]) : code(0)
		{
			assert(array[4] == 0 && "Expected a four-char code");
			for (size_t i = 0; i < 4; i++)
			{
				code <<= 8;
				code |= array[i];
			}
		}
	};
	
	std::ostream& operator<<(std::ostream& into, const FourCharCode& code);
}

#endif /* defined(__Classix__FourCharCode__) */
