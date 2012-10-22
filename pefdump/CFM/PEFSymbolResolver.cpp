//
//  PEFSymbolResolver.cpp
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "PEFSymbolResolver.h"
#include <iostream>

namespace CFM
{
	PEFSymbolResolver::PEFSymbolResolver(FragmentManager& cfm, const std::string& filePath)
	: mapping(filePath)
	, container(mapping.begin(), mapping.end())
	, cfm(cfm)
	{
		Fixup();
	}
	
	PEFSymbolResolver::PEFSymbolResolver(FragmentManager& cfm, FileMapping&& mapping)
	: mapping(std::move(mapping))
	, container(this->mapping.begin(), this->mapping.end())
	, cfm(cfm)
	{
		Fixup();
	}
	
	void PEFSymbolResolver::Fixup()
	{
		
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
	
	void* PEFSymbolResolver::ResolveSymbol(const std::string &symbolName)
	{
		auto symbol = container.LoaderSection()->ExportTable.Find(symbolName);
		if (symbol != nullptr)
		{
			// section 0-n: address relative to section
			if (symbol->SectionIndex > -1)
			{
				const uint8_t* address = container.GetSection(symbol->SectionIndex).Data + symbol->Offset;
				std::cerr << "Identified symbol " << symbolName << " at address " << static_cast<const void*>(address)
					<< ", but the PowerPC VM is not implemented." << std::endl;
				return nullptr;
			}
			
			// section -2: address absolute to container
			if (symbol->SectionIndex == -2)
			{
				const uint8_t* address = container.Base + symbol->Offset;
				std::cerr << "Identified symbol " << symbolName << " at address " << static_cast<const void*>(address)
					<< ", but the PowerPC VM is not implemented." << std::endl;
				return nullptr;
			}
			
			// section -3: reexported symbol
			if (symbol->SectionIndex == -3)
			{
				auto importedSymbol = container.LoaderSection()->GetSymbol(symbol->Offset);
				return cfm.ResolveSymbol(importedSymbol.LibraryName, importedSymbol.Name);
			}
		}
		
		std::cerr << "Could not find expected symbol " << symbolName << " in this fragment" << std::endl;
		return nullptr;
	}
	
	PEFSymbolResolver::~PEFSymbolResolver()
	{ }
}