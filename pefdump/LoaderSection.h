//
//  PEFLoaderSection.h
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
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
		
		relocation_iterator RelocationsBegin() const;
		relocation_iterator RelocationsEnd() const;
	};
}

#endif /* defined(__pefdump__PEFLoaderSection__) */
