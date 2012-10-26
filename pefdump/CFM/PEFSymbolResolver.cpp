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

namespace CFM
{
	PEFSymbolResolver::PEFSymbolResolver(PPCVM::MemoryManager& memMan, FragmentManager& cfm, const std::string& filePath)
	: PEFSymbolResolver(memMan, cfm, FileMapping(filePath))
	{ }
	
	PEFSymbolResolver::PEFSymbolResolver(PPCVM::MemoryManager& memMan, FragmentManager& cfm, FileMapping&& mapping)
	: mapping(std::move(mapping))
	, allocator(memMan)
	, memoryManager(memMan)
	, container(&allocator, this->mapping.begin(), this->mapping.end())
	, cfm(cfm)
	{
		// perform fixup
		auto loaderSection = container.LoaderSection();
		for (auto iter = loaderSection->LibrariesBegin(); iter != loaderSection->LibrariesEnd(); iter++)
			cfm.LoadContainer(iter->Name);
		
		for (auto iter = loaderSection->RelocationsBegin(); iter != loaderSection->RelocationsEnd(); iter++)
		{
			auto& section = container.GetSection(iter->GetSectionIndex());
			PEFRelocator relocator(cfm, *loaderSection, section);
			relocator.Execute(iter->begin(), iter->end());
		}
	}
	
	ResolvedSymbol PEFSymbolResolver::Symbolize(const uint8_t *address)
	{
		return ResolvedSymbol::PowerPCSymbol(address - memoryManager.GetBaseAddress());
	}
	
	SymbolResolver::MainSymbol PEFSymbolResolver::GetMainSymbol()
	{
		auto mainInfo = container.LoaderSection()->Header->Main;
		if (mainInfo.Section == -1)
			return nullptr;
		
		// TODO
		std::cerr << "This code fragment has a main symbol, but the PowerPC VM is not implemented." << std::endl;
		return nullptr;
	}
	
	ResolvedSymbol PEFSymbolResolver::ResolveSymbol(const std::string &symbolName)
	{
		auto symbol = container.LoaderSection()->ExportTable.Find(symbolName);
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
				auto importedSymbol = container.LoaderSection()->GetSymbol(symbol->Offset);
				return cfm.ResolveSymbol(importedSymbol.LibraryName, importedSymbol.Name);
			}
		}
		
		std::cerr << "Could not find expected symbol " << symbolName << " in this fragment" << std::endl;
		return ResolvedSymbol::Invalid;
	}
	
	PEFSymbolResolver::~PEFSymbolResolver()
	{ }
}
