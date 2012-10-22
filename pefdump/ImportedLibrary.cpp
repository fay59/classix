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
		this->header = header;
		auto first = symbolTable + header->FirstImportedSymbol;
		for (uint32_t i = 0; i < header->ImportedSymbolCount; i++)
		{
			auto& header = first[i];
			std::string symbolName = nameTable + header.GetNameOffset();
			bool isStronglyLinked = !header.GetClass().HasFlag(SymbolFlags::Weak);
			auto symbolClass = static_cast<SymbolClasses::Enum>(header.GetClass().Class);
			symbols.emplace_back(symbolName, isStronglyLinked, symbolClass);
		}
	}
}
