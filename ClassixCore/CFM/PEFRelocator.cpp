//
// PEFRelocator.cpp
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

#include "PEFRelocator.h"
#include <cassert>

namespace
{
	inline uint32_t SectionAddress(PEF::Container& container, uint32_t section)
	{
		return container.GetAllocator()->ToIntPtr(container.GetSection(section).Data);
	}
}

namespace CFM
{
	PEFRelocator::PEFRelocator(FragmentManager& cfm, Container& container, InstantiableSection& section)
	: cfm(cfm), fixupSection(section), container(container), loaderSection(*container.LoaderSection())
	{
		data = reinterpret_cast<Common::UInt32*>(section.Data);
		relocAddress = 0;
		importIndex = 0;
		sectionC = SectionAddress(container, 0);
		sectionD = SectionAddress(container, 1);
	}
	
	void PEFRelocator::AddSymbol(uint32_t index)
	{
		const ImportedSymbol& symbolHeader = loaderSection.GetSymbol(index);
		auto symbol = cfm.ResolveSymbol(symbolHeader.LibraryName, symbolHeader.Name);
		if (symbol.Universe == SymbolUniverse::LostInTimeAndSpace)
			throw std::logic_error("cannot perform fixup: symbol not found");
		
		Common::UInt32& relocValue = data[relocAddress];
		uint32_t nativeEndian = relocValue;
		if (nativeEndian != 0 && symbol.Universe != SymbolUniverse::PowerPC && symbolHeader.Class == SymbolClasses::CodeSymbol)
			throw std::logic_error("cannot fixup a non-PPC function whose offset is not 0");
		
		nativeEndian += symbol.Address;
		relocValue.Set(nativeEndian);
		relocAddress++;
	}
	
	void PEFRelocator::RelocByIndex(int subOpcode, int index)
	{
		if (subOpcode == 0)
		{
			AddSymbol(index);
			importIndex = index + 1;
		}
		else
		{
			uint32_t sectionAddress = SectionAddress(container, index);
			switch (subOpcode)
			{
				case 1:
					sectionC = sectionAddress;
					break;
					
				case 2:
					sectionD = sectionAddress;
					break;
					
				case 3:
					Add(sectionAddress);
					break;
			}
		}
	}
	
	void PEFRelocator::RelocBySectDWithSkip(uint32_t value)
	{
		int skipCount = (value >> 6) & 0xff;
		int relocCount = value & 0x3f;
		
		relocAddress += skipCount;
		for (int i = 0; i < relocCount; i++)
			Add(sectionD);
	}
	
	void PEFRelocator::RelocGroup(uint32_t value)
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
				for (int i = 0; i < runLength; i++)
				{
					AddSymbol(importIndex);
					importIndex++;
				}
				break;
		}
	}
	
	void PEFRelocator::RelocSmallByIndex(uint32_t value)
	{
		int subOpcode = (value >> 9) & 0xf;
		int index = value & 0x1ff;
		RelocByIndex(subOpcode, index);
	}
	
	void PEFRelocator::RelocIncrementPosition(uint32_t value)
	{
		uint32_t increment = (value & 0xfff) + 1;
		assert((increment & 3) == 0 && "incrementing to unaligned memory address");
		relocAddress += increment / 4;
	}
	
	void PEFRelocator::RelocSmallRepeat(uint32_t value, Relocation::iterator current)
	{
		const int blockCount = ((value >> 8) & 0xff) + 1;
		const int repeatCount = (value & 0xf) + 1;
		Loop(current - blockCount, current, repeatCount);
	}
	
	void PEFRelocator::RelocSetPosition(uint32_t value)
	{
		relocAddress = value & 0xffffff;
	}
	
	void PEFRelocator::RelocLargeByImport(uint32_t value)
	{
		importIndex = value & 0xffffff;
		AddSymbol(importIndex);
		importIndex++;
	}
	
	void PEFRelocator::RelocLargeRepeat(uint32_t value, Relocation::iterator current)
	{
		const int repeatCount = 0x7fffff;
		const int blockCount = ((value >> 22) & 0xf) + 1;
		Loop(current - blockCount, current, repeatCount);
	}
	
	void PEFRelocator::RelocLargeSetOrBySection(uint32_t value)
	{
		int subOpcode = (value >> 22) & 0xf;
		int index = value & 0x7fffff;
		RelocByIndex(subOpcode, index);
	}
	
	void PEFRelocator::Execute(Relocation::iterator begin, Relocation::iterator end)
	{
		for (auto iter = begin; iter != end; iter++)
		{
			uint32_t value = *iter;
			if (value >> 14 == 0b00)
				RelocBySectDWithSkip(value);
			else if (value >> 13 == 0b010)
				RelocGroup(value);
			else if (value >> 13 == 0b011)
				RelocSmallByIndex(value);
			else if (value >> 12 == 0b1000)
				RelocIncrementPosition(value);
			else if (value >> 12 == 0b1001)
				RelocSmallRepeat(value, iter);
			else
			{
				iter++;
				value <<= 16;
				value |= *iter;
				if (value >> 26 == 0b101000)
					RelocSetPosition(value);
				else if (value >> 26 == 0b101001)
					RelocLargeByImport(value);
				else if (value >> 26 == 0b101100)
					RelocLargeRepeat(value, iter - 1);
				else if (value >> 26 == 0b101101)
					RelocLargeSetOrBySection(value);
				else
					throw std::logic_error("unknown instruction prefix");
			}
		}
	}
}