//
//  Container.cpp
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "Container.h"

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
				instantiableSectionIndices.push_back(static_cast<int32_t>(this->sections.size()));
				this->sections.emplace_back(allocator, sectionHeader, sectionName, Base, static_cast<const uint8_t*>(end));
			}
		}
		
		if (loader == nullptr)
			throw std::logic_error("container has no loader section");
	}
	
	Container::iterator Container::SectionsBegin() const
	{
		return sections.begin();
	}
	
	Container::iterator Container::SectionsEnd() const
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