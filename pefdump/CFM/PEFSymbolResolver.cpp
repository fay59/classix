//
//  PEFSymbolResolver.cpp
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <iostream>
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
			PEFRelocator relocator(cfm, *loaderSection, section);
			auto address = &(*loaderSection->LibrariesBegin()).Symbols[0];
			std::cout << "address is " << address << std::endl;
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
		
		const uint8_t* address = container.GetSection(sectionWithOffset.Section).Data + sectionWithOffset.Offset;
		return Symbolize(address);
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
