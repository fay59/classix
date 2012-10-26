//
//  PEFRelocator.h
//  pefdump
//
//  Created by Félix on 2012-10-24.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__PEFRelocator__
#define __pefdump__PEFRelocator__

#include "FragmentManager.h"
#include "InstantiableSection.h"
#include "LoaderSection.h"

namespace CFM
{
	using namespace PEF;
	
	class PEFRelocator
	{
		FragmentManager& cfm;
		InstantiableSection& fixupSection;
		LoaderSection& loaderSection;
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
		
		inline void AddSymbol(uint32_t index)
		{
			auto& symbolHeader = loaderSection.GetSymbol(index);
			auto symbol = cfm.ResolveSymbol(symbolHeader.LibraryName, symbolHeader.Name);
			if (symbol.Universe == SymbolUniverse::LostInTimeAndSpace)
				throw std::logic_error("cannot perform fixup: symbol not found");
			
			UInt32& relocValue = data[relocAddress];
			uint32_t nativeEndian = relocValue;
			if (nativeEndian != 0 && symbol.Universe != SymbolUniverse::PowerPC && symbolHeader.Class == SymbolClasses::CodeSymbol)
				throw std::logic_error("cannot fixup a non-PPC function whose offset is not 0");
			
			nativeEndian += symbol.Address;
			relocValue = nativeEndian;
			relocAddress++;
		}
		
		inline void Loop(Relocation::iterator begin, Relocation::iterator end, int times)
		{
			for (int i = 0; i < times; i++)
				Execute(begin, end);
		}
		
		inline void RelocByIndex(int subOpcode, int index)
		{
			UInt32* asIntegers = reinterpret_cast<UInt32*>(fixupSection.Data);
			switch (subOpcode)
			{
				case 0:
					AddSymbol(index);
					importIndex = index + 1;
					break;
					
				case 1:
					sectionC = asIntegers[index];
					break;
					
				case 2:
					sectionD = asIntegers[index];
					break;
					
				case 3:
					Add(asIntegers[index]);
					break;
			}
		}
		
		void RelocBySectDWithSkip(uint32_t value);
		void RelocGroup(uint32_t value);
		void RelocSmallByIndex(uint32_t value);
		void RelocIncrementPosition(uint32_t value);
		void RelocSmallRepeat(uint32_t value, Relocation::iterator current);
		void RelocSetPosition(uint32_t value);
		void RelocLargeByImport(uint32_t value);
		void RelocLargeRepeat(uint32_t value, Relocation::iterator current);
		void RelocLargeSetOrBySection(uint32_t value);
		
	public:
		PEFRelocator(FragmentManager& cfm, LoaderSection& loaderSection, InstantiableSection& section);
		
		void Execute(Relocation::iterator begin, Relocation::iterator end);
	};
}

#endif /* defined(__pefdump__PEFRelocator__) */
