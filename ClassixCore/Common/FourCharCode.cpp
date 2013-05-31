//
// FourCharCode.cpp
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

#include "FourCharCode.h"

namespace Common
{
	std::ostream& operator<<(std::ostream& into, const FourCharCode& code)
	{
		into << '\'';
		into << (char)((code.code >> 24) & 0xff);
		into << (char)((code.code >> 16) & 0xff);
		into << (char)((code.code >> 8) & 0xff);
		into << (char)(code.code & 0xff);
		into << '\'';
		return into;
	}
}