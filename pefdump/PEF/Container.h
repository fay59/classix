//
//  PEFContainer.h
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
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
		typedef std::vector<InstantiableSection>::const_iterator iterator;
		
		const uint8_t* Base;
		
		Container(Common::IAllocator* allocator, const void* base, const void* end);
		
		iterator SectionsBegin() const;
		iterator SectionsEnd() const;
		size_t Size() const;
		
		InstantiableSection& GetSection(uint32_t index);
		
		LoaderSection* LoaderSection();
		const class LoaderSection* LoaderSection() const;
	};
}

#endif /* defined(__pefdump__PEFContainer__) */
