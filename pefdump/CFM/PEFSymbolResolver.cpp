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

namespace
{
	using namespace CFM;
	using namespace PEF;
	
	class PEFRelocator
	{
		FragmentManager& cfm;
		PEF::InstantiableSection& section;
		UInt32* data;
		
		uint32_t relocAddress;
		uint32_t importIndex;
		uint32_t sectionC;
		uint32_t sectionD;
		
		inline void Add(uint32_t value)
		{
			uint32_t nativeEndian = data[relocAddress];
			nativeEndian += value;
			data[relocAddress] = nativeEndian;
			relocAddress++;
		}
		
		void RelocBySectDWithSkip(uint32_t value)
		{
			int skipCount = (value >> 6) & 0xff;
			int relocCount = value & 0x3f;
			
			relocAddress += skipCount;
			for (int i = 0; i < relocCount; i++)
				Add(sectionD);
		}
		
		void RelocGroup(uint32_t value)
		{
			int subopcode = (value >> 9) & 0xf;
			int runLength = (value & 0x1ff) + 1;
			switch (subopcode)
			{
				case 0:
					for (int i = 0; i < runLength; i++)
						Add(sectionC);
					break;
					
				case 1:
					for (int i = 0; i < runLength; i++)
						Add(sectionD);
					break;
				
				case 2:
					for (int i = 0; i < runLength; i++)
					{
						Add(sectionC);
						Add(sectionD);
						relocAddress++;
					}
					break;
				
				case 3:
					for (int i = 0; i < runLength; i++)
					{
						Add(sectionC);
						Add(sectionD);
					}
					break;
				
				case 4:
					for (int i = 0; i < runLength; i++)
					{
						Add(sectionD);
						relocAddress++;
					}
					break;
				
				case 5:
					throw std::logic_error("RelocImportRun is not implemented");
					break;
			}
		}
		
	public:
		PEFRelocator(FragmentManager& cfm, PEF::InstantiableSection& section)
		: cfm(cfm), section(section)
		{
			data = reinterpret_cast<UInt32*>(section.Data);
		}
		
		void Execute(Relocation::iterator begin, Relocation::iterator end)
		{
			for (auto iter = begin; iter != end; iter++)
			{
				uint32_t value = *iter;
				int opcode = value >> 12;
				switch (opcode)
				{
					case 0:
						RelocBySectDWithSkip(value);
						break;
						
					case 4:
						RelocGroup(value);
						break;
				}
			}
		}
	};
}

namespace CFM
{
	PEFSymbolResolver::PEFSymbolResolver(Common::IAllocator* allocator, FragmentManager& cfm, const std::string& filePath)
	: mapping(filePath)
	, container(allocator, mapping.begin(), mapping.end())
	, cfm(cfm)
	{
		Fixup();
	}
	
	PEFSymbolResolver::PEFSymbolResolver(Common::IAllocator* allocator, FragmentManager& cfm, FileMapping&& mapping)
	: mapping(std::move(mapping))
	, container(allocator, this->mapping.begin(), this->mapping.end())
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
	
	intptr_t PEFSymbolResolver::ResolveSymbol(const std::string &symbolName)
	{
		auto symbol = container.LoaderSection()->ExportTable.Find(symbolName);
		if (symbol != nullptr)
		{
			// section 0-n: address relative to section
			if (symbol->SectionIndex > -1)
			{
				const uint8_t* address = container.GetSection(symbol->SectionIndex).Data + symbol->Offset;
				return reinterpret_cast<intptr_t>(address);
			}
			
			// section -2: address absolute to container
			if (symbol->SectionIndex == -2)
			{
				const uint8_t* address = container.Base + symbol->Offset;
				return reinterpret_cast<intptr_t>(address);
			}
			
			// section -3: reexported symbol
			if (symbol->SectionIndex == -3)
			{
				auto importedSymbol = container.LoaderSection()->GetSymbol(symbol->Offset);
				return cfm.ResolveSymbol(importedSymbol.LibraryName, importedSymbol.Name);
			}
		}
		
		std::cerr << "Could not find expected symbol " << symbolName << " in this fragment" << std::endl;
		return 0;
	}
	
	PEFSymbolResolver::~PEFSymbolResolver()
	{ }
}