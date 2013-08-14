//
// Export.h
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
		typedef uint32_t HashFunction(const std::string&);
		std::vector<std::string> symbolNames;
		std::unordered_map<std::string, ExportedSymbol, HashFunction&> symbolTable;
		
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
