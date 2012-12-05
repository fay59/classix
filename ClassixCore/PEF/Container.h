//
// Container.h
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

#ifndef __pefdump__PEFContainer__
#define __pefdump__PEFContainer__

#include <cstdint>
#include <string>
#include <vector>

#include "Structures.h"
#include "InstantiableSection.h"
#include "LoaderSection.h"
#include "IAllocator.h"

namespace PEF
{
	class Container
	{
		const ContainerHeader* header;
		std::vector<InstantiableSection> sections;
		std::vector<int32_t> instantiableSectionIndices;
		LoaderSection* loader;
		Common::IAllocator* allocator;
		
	public:
		typedef std::vector<InstantiableSection>::iterator iterator;
		typedef std::vector<InstantiableSection>::const_iterator const_iterator;
		
		const uint8_t* Base;
		
		Container(Common::IAllocator* allocator, const void* base, const void* end);
		
		iterator SectionsBegin();
		iterator SectionsEnd();
		const_iterator SectionsBegin() const;
		const_iterator SectionsEnd() const;
		size_t Size() const;
		
		Common::IAllocator* GetAllocator();
		
		InstantiableSection& GetSection(uint32_t index);
		const InstantiableSection& GetSection(uint32_t index) const;
		
		LoaderSection* LoaderSection();
		const class LoaderSection* LoaderSection() const;
	};
}

#endif /* defined(__pefdump__PEFContainer__) */
