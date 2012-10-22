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

namespace PEF
{
	class Container
	{
		const uint8_t* base;
		const ContainerHeader* header;
		std::vector<InstantiableSection> sections;
		LoaderSection* loader;
		
	public:
		typedef std::vector<InstantiableSection>::const_iterator iterator;
		
		Container(const void* base, const void* end);
		
		iterator SectionsBegin() const;
		iterator SectionsEnd() const;
		size_t Size() const;
		
		LoaderSection* LoaderSection();
		const class LoaderSection* LoaderSection() const;
	};
}

#endif /* defined(__pefdump__PEFContainer__) */
