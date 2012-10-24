//
//  PFEImportedLibrary.h
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__PFEImportedLibrary__
#define __pefdump__PFEImportedLibrary__

#include <string>
#include <vector>

#include "Structures.h"
#include "ImportedSymbol.h"

namespace PEF
{
	class ImportedLibrary
	{
	public:
		std::string Name;
		std::vector<ImportedSymbol> Symbols;
		const ImportedLibraryHeader* Header;
		
		ImportedLibrary(const ImportedLibraryHeader* header, const char* nameTable, const ImportedSymbolHeader* symbolTable);
	};
}

#endif /* defined(__pefdump__PFEImportedLibrary__) */
