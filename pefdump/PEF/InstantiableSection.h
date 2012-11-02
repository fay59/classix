//
//  PEFSection.h
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__PEFSection__
#define __pefdump__PEFSection__

#include <string>
#include "Structures.h"
#include "IAllocator.h"

namespace PEF
{
	class InstantiableSection
	{
		const SectionHeader* header;
		Common::IAllocator* allocator;
		
	public:
		std::string Name;
		uint8_t* Data;
		
		InstantiableSection(Common::IAllocator* allocator, const SectionHeader* header, const std::string& name, const uint8_t* base, const uint8_t* end);
		InstantiableSection(const InstantiableSection& that) = delete;
		InstantiableSection(InstantiableSection&& that);
		
		SectionType GetSectionType() const;
		ShareType GetShareType() const;
		size_t Size() const;
		
		~InstantiableSection();
	};
}

#endif /* defined(__pefdump__PEFSection__) */
