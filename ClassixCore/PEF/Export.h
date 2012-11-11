//
//  Export.h
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__Export__
#define __pefdump__Export__

#include "Structures.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace PEF
{
	struct ExportedSymbol
	{
		std::string SymbolName;
		SymbolClasses::Enum Class;
		int16_t SectionIndex;
		uint32_t Offset;
	};
	
	class ExportHashTable
	{
	public:
		static uint32_t HashSymbolName(const std::string& name);
		static uint32_t HashSymbol(const ExportedSymbol& symbol);
		
	private:
		typedef uint32_t (*HashFunction)(const std::string&);
		std::vector<std::string> symbolNames;
		std::unordered_map<std::string, ExportedSymbol, HashFunction> symbolTable;
		
	public:
		typedef std::vector<std::string>::const_iterator name_iterator;
		ExportHashTable(const LoaderHeader* loaderHeader);
		
		const ExportedSymbol* Find(const std::string& name) const;
		const ExportedSymbol* Find(uint32_t index) const;
		
		name_iterator begin() const;
		name_iterator end() const;
		uint32_t SymbolCount() const;
	};
}

#endif /* defined(__pefdump__Export__) */
