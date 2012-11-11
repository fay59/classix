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
		Container& container;
		InstantiableSection& fixupSection;
		const LoaderSection& loaderSection;
		Common::UInt32* data;
		
		uint32_t relocAddress;
		uint32_t importIndex;
		uint32_t sectionC;
		uint32_t sectionD;
		
		inline void Add(uint32_t value)
		{
			uint32_t nativeEndian = data[relocAddress];
			nativeEndian += value;
			data[relocAddress].Set(nativeEndian);
			relocAddress++;
		}
		
		inline void Loop(Relocation::iterator begin, Relocation::iterator end, int times)
		{
			for (int i = 0; i < times; i++)
				Execute(begin, end);
		}
		
		void RelocByIndex(int subOpcode, int index);
		void AddSymbol(uint32_t index);
		
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
		PEFRelocator(FragmentManager& cfm, Container& container, InstantiableSection& section);
		
		void Execute(Relocation::iterator begin, Relocation::iterator end);
	};
}

#endif /* defined(__pefdump__PEFRelocator__) */
