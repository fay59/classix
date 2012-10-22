//
//  PEFLoaderSection.cpp
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
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
			auto& libraryHeader = libraries[i];
			this->libraries.emplace_back(&libraryHeader, nameTable, symbols);
		}
		
		const uint8_t* relocationBase = reinterpret_cast<const uint8_t*>(header) + header->RelocInstructionOffset;
		const RelocationHeader* relocations = reinterpret_cast<const RelocationHeader*>(relocationBase) - header->RelocSectionCount;
		for (uint32_t i = 0; i < header->RelocSectionCount; i++)
		{
			auto& relocation = relocations[i];
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
	
	LoaderSection::relocation_iterator LoaderSection::RelocationsBegin() const
	{
		return relocations.begin();
	}
	
	LoaderSection::relocation_iterator LoaderSection::RelocationsEnd() const
	{
		return relocations.end();
	}
}
