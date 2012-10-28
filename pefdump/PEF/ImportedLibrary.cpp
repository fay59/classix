//
//  PFEImportedLibrary.cpp
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "ImportedLibrary.h"

namespace PEF
{
	ImportedLibrary::ImportedLibrary(const ImportedLibraryHeader* header, const char* nameTable, const ImportedSymbolHeader* symbolTable)
	: Name(nameTable + header->NameOffset)
	{
		Header = header;
		const PEF::ImportedSymbolHeader* libraryHeaders = symbolTable + header->FirstImportedSymbol;
		Symbols.reserve(header->ImportedSymbolCount);
		for (uint32_t i = 0; i < header->ImportedSymbolCount; i++)
		{
			const ImportedSymbolHeader& header = libraryHeaders[i];
			std::string symbolName = nameTable + header.GetNameOffset();
			bool isStronglyLinked = !header.GetClass().HasFlag(SymbolFlags::Weak);
			auto symbolClass = static_cast<SymbolClasses::Enum>(header.GetClass().Class);
			Symbols.emplace_back(Name, symbolName, isStronglyLinked, symbolClass);
		}
	}
}
