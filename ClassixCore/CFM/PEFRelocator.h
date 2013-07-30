//
// PEFRelocator.h
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
		uint8_t* data;
		
		uint32_t relocAddress;
		uint32_t importIndex;
		uint32_t sectionC;
		uint32_t sectionD;
		
		inline void Add(uint32_t value)
		{
			Common::UInt32 initialValue;
			memcpy(&initialValue, &data[relocAddress], sizeof initialValue);
			uint32_t nativeEndian = initialValue;
			nativeEndian += value;
			initialValue = nativeEndian;
			memcpy(&data[relocAddress], &initialValue, sizeof initialValue);
			relocAddress += 4;
		}
		
		inline void Loop(Relocation::iterator begin, Relocation::iterator end, int times)
		{
			for (int i = 0; i < times; i++)
				Execute(begin, end);
		}
		
		void RelocByIndex(int subOpcode, int index);
		void AddSymbol(uint32_t index);
		
		void RelocBySectDWithSkip(uint32_t value);
		void RelocBySectC(uint32_t value);
		void RelocBySectD(uint32_t value);
		void RelocTVector12(uint32_t value);
		void RelocTVector8(uint32_t value);
		void RelocVTable8(uint32_t value);
		void RelocImportRun(uint32_t value);
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
