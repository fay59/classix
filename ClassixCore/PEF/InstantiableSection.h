//
// InstantiableSection.h
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

#ifndef __pefdump__PEFSection__
#define __pefdump__PEFSection__

#include <string>
#include "Structures.h"
#include "IAllocator.h"

namespace PEF
{
	class InstantiableSection
	{
		Common::IAllocator& allocator;
		const SectionHeader* header;
		
	public:
		std::string Name;
		uint8_t* Data;
		
		InstantiableSection(Common::IAllocator& allocator, const SectionHeader* header, const std::string& name, const uint8_t* base, const uint8_t* end);
		InstantiableSection(const InstantiableSection& that) = delete;
		InstantiableSection(InstantiableSection&& that);
		
		bool IsExecutable() const;
		SectionType GetSectionType() const;
		ShareType GetShareType() const;
		size_t Size() const;
		size_t AbsoluteOffset() const;
		uint32_t GetDataLocation() const;
		
		~InstantiableSection();
	};
}

#endif /* defined(__pefdump__PEFSection__) */
