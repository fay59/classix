//
// PEFSymbolResolver.cpp
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

#include <iostream>
#include <cassert>
#include "PEFSymbolResolver.h"
#include "Relocation.h"
#include "PEFRelocator.h"
#include "LibraryResolutionException.h"

namespace CFM
{
	PEFSymbolResolver::PEFSymbolResolver(Common::IAllocator* allocator, FragmentManager& cfm, const std::string& filePath)
	: PEFSymbolResolver(allocator, cfm, Common::FileMapping(filePath))
	{ }
	
	PEFSymbolResolver::PEFSymbolResolver(Common::IAllocator* allocator, FragmentManager& cfm, Common::FileMapping&& mapping)
	: mapping(std::move(mapping))
	, allocator(allocator)
	, container(allocator, this->mapping.begin(), this->mapping.end())
	, cfm(cfm)
	{
		// perform fixup
		const LoaderSection* loaderSection = container.LoaderSection();
		for (auto iter = loaderSection->LibrariesBegin(); iter != loaderSection->LibrariesEnd(); iter++)
		{
			bool loaded = cfm.LoadContainer(iter->Name);
			if (!loaded)
				throw LibraryResolutionException(iter->Name);
		}
		
		for (auto iter = loaderSection->RelocationsBegin(); iter != loaderSection->RelocationsEnd(); iter++)
		{
			InstantiableSection& section = container.GetSection(iter->GetSectionIndex());
			PEFRelocator relocator(cfm, container, section);
			relocator.Execute(iter->begin(), iter->end());
		}
	}
	
	ResolvedSymbol PEFSymbolResolver::Symbolize(const uint8_t *address)
	{
		if (address == nullptr)
			return ResolvedSymbol::Invalid;
		
		return ResolvedSymbol::PowerPCSymbol(reinterpret_cast<intptr_t>(address));
	}
	
	ResolvedSymbol PEFSymbolResolver::Symbolize(const PEF::LoaderHeader::SectionWithOffset &sectionWithOffset)
	{
		if (sectionWithOffset.Section == -1)
			return ResolvedSymbol::Invalid;
		
		auto& section = container.GetSection(sectionWithOffset.Section);
		const uint8_t* address = section.Data + sectionWithOffset.Offset;
		return Symbolize(address);
	}
	
	PEF::Container& PEFSymbolResolver::GetContainer()
	{
		return container;
	}
	
	const PEF::Container& PEFSymbolResolver::GetContainer() const
	{
		return container;
	}
	
	ResolvedSymbol PEFSymbolResolver::GetInitAddress()
	{
		const LoaderHeader::SectionWithOffset& initInfo = container.LoaderSection()->Header->Init;
		return Symbolize(initInfo);
	}
	
	ResolvedSymbol PEFSymbolResolver::GetMainAddress()
	{
		const LoaderHeader::SectionWithOffset& mainInfo = container.LoaderSection()->Header->Main;
		return Symbolize(mainInfo);
	}
	
	ResolvedSymbol PEFSymbolResolver::GetTermAddress()
	{
		const LoaderHeader::SectionWithOffset& termInfo = container.LoaderSection()->Header->Term;
		return Symbolize(termInfo);
	}
	
	ResolvedSymbol PEFSymbolResolver::ResolveSymbol(const std::string &symbolName)
	{
		const ExportedSymbol* symbol = container.LoaderSection()->ExportTable.Find(symbolName);
		if (symbol != nullptr)
		{
			// section 0-n: address relative to section
			if (symbol->SectionIndex > -1)
			{
				const uint8_t* address = container.GetSection(symbol->SectionIndex).Data + symbol->Offset;
				return Symbolize(address);
			}
			
			// section -2: address absolute to container
			if (symbol->SectionIndex == -2)
			{
				const uint8_t* address = container.Base + symbol->Offset;
				return Symbolize(address);
			}
			
			// section -3: reexported symbol
			if (symbol->SectionIndex == -3)
			{
				const ImportedSymbol& importedSymbol = container.LoaderSection()->GetSymbol(symbol->Offset);
				return cfm.ResolveSymbol(importedSymbol.LibraryName, importedSymbol.Name);
			}
		}
		
		std::cerr << "Could not find expected symbol " << symbolName << " in this fragment" << std::endl;
		return ResolvedSymbol::Invalid;
	}
	
	PEFSymbolResolver::~PEFSymbolResolver()
	{ }
}
