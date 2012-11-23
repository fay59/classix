//
// LoaderSection.cpp
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

#include "LoaderSection.h"

namespace PEF
{
	LoaderSection::LoaderSection(const LoaderHeader* header)
	: ExportTable(header)
	{
		Header = header;
		const ImportedLibraryHeader* libraries = reinterpret_cast<const ImportedLibraryHeader*>(header + 1);
		const char* nameTable = reinterpret_cast<const char*>(header) + header->LoaderStringsOffset;
		const ImportedSymbolHeader* symbols = reinterpret_cast<const ImportedSymbolHeader*>(libraries + header->ImportedLibraryCount);
		
		for (uint32_t i = 0; i < header->ImportedLibraryCount; i++)
		{
			const ImportedLibraryHeader& libraryHeader = libraries[i];
			this->libraries.emplace_back(&libraryHeader, nameTable, symbols);
		}
		
		const uint8_t* relocationBase = reinterpret_cast<const uint8_t*>(header) + header->RelocInstructionOffset;
		const RelocationHeader* relocations = reinterpret_cast<const RelocationHeader*>(relocationBase) - header->RelocSectionCount;
		for (uint32_t i = 0; i < header->RelocSectionCount; i++)
		{
			const RelocationHeader& relocation = relocations[i];
			this->relocations.emplace_back(&relocation, relocationBase);
		}
	}
	
	LoaderSection::library_iterator LoaderSection::LibrariesBegin() const
	{
		return libraries.begin();
	}
	
	LoaderSection::library_iterator LoaderSection::LibrariesEnd() const
	{
		return libraries.end();
	}
	
	const ImportedSymbol& LoaderSection::GetSymbol(uint32_t index) const
	{
		for (const ImportedLibrary& library : libraries)
		{
			uint32_t beginImport = library.Header->FirstImportedSymbol;
			uint32_t endImport = beginImport + library.Header->ImportedSymbolCount;
			if (beginImport <= index && endImport > index)
				return library.Symbols.at(index - beginImport);
		}
		
		throw std::logic_error("symbol out of bounds");
	}
	
	LoaderSection::relocation_iterator LoaderSection::RelocationsBegin() const
	{
		return relocations.begin();
	}
	
	LoaderSection::relocation_iterator LoaderSection::RelocationsEnd() const
	{
		return relocations.end();
	}
}
