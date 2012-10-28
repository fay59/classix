//
//  ImportedSymbol.cpp
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "ImportedSymbol.h"

namespace PEF
{
	ImportedSymbol::ImportedSymbol(const std::string& libraryName, const std::string& name, bool isStronglyLinked, SymbolClasses::Enum symbolClass)
	: LibraryName(libraryName), Name(name)
	{
		if (LibraryName.length() == 0 || Name.length() == 0)
			throw std::logic_error("cannot initialize imported symbol with blank names");
		
		Address = 0;
		IsStronglyLinked = isStronglyLinked;
		Class = symbolClass;
	}
}