//
// LoaderSection.h
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

#ifndef __pefdump__PEFLoaderSection__
#define __pefdump__PEFLoaderSection__

#include <vector>
#include "Structures.h"
#include "ImportedLibrary.h"
#include "Relocation.h"
#include "Export.h"

namespace PEF
{
	class LoaderSection
	{
		std::vector<ImportedLibrary> libraries;
		std::vector<Relocation> relocations;
		
	public:
		typedef std::vector<ImportedLibrary>::const_iterator library_iterator;
		typedef std::vector<Relocation>::const_iterator relocation_iterator;
		
		const LoaderHeader* Header;
		const ExportHashTable ExportTable;
		
		LoaderSection(const LoaderHeader* header);
		
		library_iterator LibrariesBegin() const;
		library_iterator LibrariesEnd() const;
		
		const ImportedSymbol& GetSymbol(uint32_t index) const;
		
		relocation_iterator RelocationsBegin() const;
		relocation_iterator RelocationsEnd() const;
	};
}

#endif /* defined(__pefdump__PEFLoaderSection__) */
