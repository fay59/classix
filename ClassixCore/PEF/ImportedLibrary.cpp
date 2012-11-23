//
// ImportedLibrary.cpp
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
