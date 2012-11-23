//
// Container.cpp
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

#include "Container.h"
#include <sstream>

namespace PEF
{
	Container::Container(Common::IAllocator* allocator, const void* base, const void* end)
	: Base(static_cast<const uint8_t*>(base))
	, header(ContainerHeader::FromPointer(base))
	, loader(nullptr)
	, allocator(allocator)
	{
		if (header == nullptr)
			throw std::logic_error("invalid header");
		
		const uint32_t sections = header->InstSectionCount;
		const char* const nameTableOffset = reinterpret_cast<const char*>
			(Base + sizeof(ContainerHeader) + sizeof(SectionHeader) * sections);
		
		// instantiate sections
		this->sections.reserve(sections);
		for (uint16_t i = 0; i < header->SectionCount; i++)
		{
			std::string sectionName;
			const SectionHeader* sectionHeader = SectionHeader::FromContainer(header, i);
			if (sectionHeader->NameOffset != -1)
				sectionName = nameTableOffset + sectionHeader->NameOffset;
			
			if (sectionHeader->SectionType == SectionType::Loader)
			{
				instantiableSectionIndices.push_back(-1);
				if (loader != nullptr)
					throw std::logic_error("container has more than one loader section");
				
				const LoaderHeader* header = LoaderHeader::FromSectionHeader(sectionHeader, base);
				loader = new class LoaderSection(header);
			}
			else
			{
				int32_t id = static_cast<int32_t>(this->sections.size());
				instantiableSectionIndices.push_back(id);
				if (sectionName.length() == 0)
				{
					std::stringstream ss;
					ss << "Instantiable section #" << id;
					sectionName = ss.str();
				}
				this->sections.emplace_back(allocator, sectionHeader, sectionName, Base, static_cast<const uint8_t*>(end));
			}
		}
		
		if (loader == nullptr)
			throw std::logic_error("container has no loader section");
	}
	
	Container::iterator Container::SectionsBegin()
	{
		return sections.begin();
	}
	
	Container::iterator Container::SectionsEnd()
	{
		return sections.end();
	}
	
	Container::const_iterator Container::SectionsBegin() const
	{
		return sections.begin();
	}
	
	Container::const_iterator Container::SectionsEnd() const
	{
		return sections.end();
	}
	
	size_t Container::Size() const
	{
		return sections.size();
	}
	
	InstantiableSection& Container::GetSection(uint32_t index)
	{
		int32_t realIndex = instantiableSectionIndices.at(index);
		return sections.at(realIndex);
	}
	
	LoaderSection* Container::LoaderSection()
	{
		return loader;
	}
	
	const LoaderSection* Container::LoaderSection() const
	{
		return loader;
	}
}