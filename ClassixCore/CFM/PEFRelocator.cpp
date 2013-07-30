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
		return container.GetAllocator().ToIntPtr(container.GetSection(section).Data);
	}
}

namespace CFM
{
	PEFRelocator::PEFRelocator(FragmentManager& cfm, Container& container, InstantiableSection& section)
	: cfm(cfm), fixupSection(section), container(container), loaderSection(*container.LoaderSection())
	{
		data = section.Data;
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
		
		Common::UInt32 relocValue;
		memcpy(&relocValue, &data[relocAddress], sizeof relocValue);
		uint32_t nativeEndian = relocValue;
		if (nativeEndian != 0 && symbol.Universe != SymbolUniverse::PowerPC && symbolHeader.Class == SymbolClasses::CodeSymbol)
			throw std::logic_error("cannot fixup a non-PPC function whose offset is not 0");
		
		nativeEndian += symbol.Address;
		relocValue = nativeEndian;
		memcpy(&data[relocAddress], &relocValue, sizeof relocValue);
		relocAddress += 4;
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
		
		relocAddress += skipCount * 4;
		for (int i = 0; i < relocCount; i++)
			Add(sectionD);
	}
	
	void PEFRelocator::RelocBySectC(uint32_t value)
	{
		int runLength = (value & 0x1ff) + 1;
		for (int i = 0; i < runLength; i++)
			Add(sectionC);
	}
	
	void PEFRelocator::RelocBySectD(uint32_t value)
	{
		int runLength = (value & 0x1ff) + 1;
		for (int i = 0; i < runLength; i++)
			Add(sectionD);
	}
	
	void PEFRelocator::RelocTVector12(uint32_t value)
	{
		int runLength = (value & 0x1ff) + 1;
		for (int i = 0; i < runLength; i++)
		{
			Add(sectionC);
			Add(sectionD);
			relocAddress += 4;
		}
	}
	
	void PEFRelocator::RelocTVector8(uint32_t value)
	{
		int runLength = (value & 0x1ff) + 1;
		for (int i = 0; i < runLength; i++)
		{
			Add(sectionC);
			Add(sectionD);
		}
	}
	
	void PEFRelocator::RelocVTable8(uint32_t value)
	{
		int runLength = (value & 0x1ff) + 1;
		for (int i = 0; i < runLength; i++)
		{
			Add(sectionD);
			relocAddress += 4;
		}
	}
	
	void PEFRelocator::RelocImportRun(uint32_t value)
	{
		int runLength = (value & 0x1ff) + 1;
		for (int i = 0; i < runLength; i++)
		{
			AddSymbol(importIndex);
			importIndex++;
		}
	}
	
	void PEFRelocator::RelocSmallByIndex(uint32_t value)
	{
		int subOpcode = (value >> 9) & 0xf;
		int index = value & 0x01ff;
		RelocByIndex(subOpcode, index);
	}
	
	void PEFRelocator::RelocIncrementPosition(uint32_t value)
	{
		uint32_t increment = (value & 0x0fff) + 1;
		relocAddress += increment;
	}
	
	void PEFRelocator::RelocSmallRepeat(uint32_t value, Relocation::iterator current)
	{
		const int blockCount = ((value >> 8) & 0xff) + 1;
		const int repeatCount = (value & 0xf) + 1;
		Loop(current - blockCount, current, repeatCount);
	}
	
	void PEFRelocator::RelocSetPosition(uint32_t value)
	{
		relocAddress = value & 0x00ffffff;
	}
	
	void PEFRelocator::RelocLargeByImport(uint32_t value)
	{
		importIndex = value & 0x00ffffff;
		AddSymbol(importIndex);
		importIndex++;
	}
	
	void PEFRelocator::RelocLargeRepeat(uint32_t value, Relocation::iterator current)
	{
		const int repeatCount = 0x007fffff;
		const int blockCount = ((value >> 22) & 0xf) + 1;
		Loop(current - blockCount, current, repeatCount);
	}
	
	void PEFRelocator::RelocLargeSetOrBySection(uint32_t value)
	{
		int subOpcode = (value >> 22) & 0xf;
		int index = value & 0x007fffff;
		RelocByIndex(subOpcode, index);
	}
	
	void PEFRelocator::Execute(Relocation::iterator begin, Relocation::iterator end)
	{
		for (auto iter = begin; iter != end; iter++)
		{
			uint16_t value = *iter;
			uint8_t opcode = value >> 9;
			switch (opcode)
			{
				case 0x00 ... 0x1f: RelocBySectDWithSkip(value); break;
				
				case 0x20: RelocBySectC(value); break;
				case 0x21: RelocBySectD(value); break;
				case 0x22: RelocTVector12(value); break;
				case 0x23: RelocTVector8(value); break;
				case 0x24: RelocVTable8(value); break;
				case 0x25: RelocImportRun(value); break;
					
				case 0x30 ... 0x33: RelocSmallByIndex(value); break;
					
				case 0x40 ... 0x47: RelocIncrementPosition(value); break;
				case 0x48 ... 0x4f: RelocSmallRepeat(value, iter); break;
					
				default:
					iter++;
					uint32_t large = (value << 16) | *iter;
					switch (opcode)
					{
						case 0x50 ... 0x51: RelocSetPosition(large); break;
						case 0x52 ... 0x53: RelocLargeByImport(large); break;
						case 0x58 ... 0x59: RelocLargeRepeat(large, iter); break;
						case 0x5a ... 0x5b: RelocLargeSetOrBySection(large); break;
							
						default: throw std::logic_error("unknown opcode prefix");
					}
					break;
			}
		}
	}
}