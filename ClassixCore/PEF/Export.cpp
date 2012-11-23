//
// Export.cpp
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

#include "Export.h"

namespace
{
	inline int32_t PseudoRotate(int32_t x)
	{
		return (x << 1) - (x >> 16);
	}
	
	struct ExportedSymbolEntry
	{
		PEF::UInt32 ClassAndName;
		PEF::UInt32 SymbolValue;
		PEF::SInt16 SectionIndex;
	} __attribute__((packed));
}

namespace PEF
{
	uint32_t ExportHashTable::HashSymbolName(const std::string& symbolName)
	{
		int32_t hashValue = 0;
		
		for (auto iter = symbolName.begin(); iter != symbolName.end(); iter++)
			hashValue = PseudoRotate(hashValue) ^ *iter;
		
		return ((uint32_t)symbolName.length() << 16) | ((uint16_t)((hashValue ^ (hashValue >> 16)) & 0xffff));
	}
	
	ExportHashTable::ExportHashTable(const LoaderHeader* header)
	: symbolTable(2, HashSymbolName)
	{
		const uint8_t* base = reinterpret_cast<const uint8_t*>(header);
		const char* nameTable = reinterpret_cast<const char*>(base) + header->LoaderStringsOffset;
		uint32_t exportedSymbols = header->ExportedSymbolCount;
		
		const Common::UInt32* hashTableStart = reinterpret_cast<const Common::UInt32*>(base + header->ExportHashOffset);
		uint32_t hashCount = 1 << header->ExportHashTablePower;
		const Common::UInt32* exportKeyTable = hashTableStart + hashCount;
		const ExportedSymbolEntry* exportSymbolTable = reinterpret_cast<const ExportedSymbolEntry*>(exportKeyTable + exportedSymbols);
		
		for (uint32_t i = 0; i < exportedSymbols; i++)
		{
			const ExportedSymbolEntry& symbolEntry = exportSymbolTable[i];
			ExportedSymbol symbol;
			symbol.Class = static_cast<SymbolClasses::Enum>(symbolEntry.ClassAndName >> 24);
			const char* nameBegin = nameTable + (symbolEntry.ClassAndName & 0xffffff);
			const char* nameEnd = nameBegin + (exportKeyTable[i] >> 16);
			symbol.SymbolName = std::string(nameBegin, nameEnd);
			symbol.SectionIndex = symbolEntry.SectionIndex;
			symbol.Offset = symbolEntry.SymbolValue;
			symbolNames.push_back(symbol.SymbolName);
			symbolTable[symbol.SymbolName] = std::move(symbol);
		}
	}
	
	const ExportedSymbol* ExportHashTable::Find(const std::string &name) const
	{
		auto findResult = symbolTable.find(name);
		if (findResult == symbolTable.end())
			return nullptr;
		
		return &findResult->second;
	}
	
	const ExportedSymbol* ExportHashTable::Find(uint32_t index) const
	{
		return Find(symbolNames.at(index));
	}
	
	ExportHashTable::name_iterator ExportHashTable::begin() const
	{
		return symbolNames.begin();
	}
	
	ExportHashTable::name_iterator ExportHashTable::end() const
	{
		return symbolNames.end();
	}
	
	uint32_t ExportHashTable::SymbolCount() const
	{
		return static_cast<uint32_t>(symbolNames.size());
	}
}
