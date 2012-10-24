//
//  Export.cpp
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
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
		
		const uint32_t* hashTableStart = reinterpret_cast<const uint32_t*>(base + header->ExportHashOffset);
		uint32_t hashCount = 1 << header->ExportHashTablePower;
		const uint32_t* exportKeyTable = hashTableStart + hashCount;
		const ExportedSymbolEntry* exportSymbolTable = reinterpret_cast<const ExportedSymbolEntry*>(exportKeyTable + exportedSymbols);
		
		for (uint32_t i = 0; i < exportedSymbols; i++)
		{
			const auto& symbolEntry = exportSymbolTable[i];
			ExportedSymbol symbol;
			symbol.Class = static_cast<SymbolClasses::Enum>(symbolEntry.ClassAndName >> 24);
			symbol.SymbolName = nameTable + (symbolEntry.ClassAndName & 0xffffff);
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
