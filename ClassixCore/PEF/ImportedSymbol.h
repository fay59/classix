//
// ImportedSymbol.h
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

#ifndef __pefdump__ImportedSymbol__
#define __pefdump__ImportedSymbol__

#include <string>
#include "Structures.h"

namespace PEF
{
	class ImportedSymbol
	{
	public:
		ImportedSymbol(const std::string& libName, const std::string& name, bool isStronglyLinked, SymbolClasses::Enum symbolClass);
		
		const std::string LibraryName;
		const std::string Name;
		intptr_t Address;
		bool IsStronglyLinked;
		SymbolClasses::Enum Class;
	};
}

#endif /* defined(__pefdump__ImportedSymbol__) */
