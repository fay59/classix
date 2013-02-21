//
// SymbolResolver.h
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

#ifndef __pefdump__SymbolResolver__
#define __pefdump__SymbolResolver__

#include <cstdint>
#include <string>
#include <vector>

namespace CFM
{
	// The "symbol universe" tells if the symbol belongs to the PowerPC universe or the Intel universe.
	// We can say that all non-code symbols belong to the PPC universe as their usage is strictly dictated
	// by PowerPC code or follows PowerPC code requirements (like endianness). On the opposite, functions that are
	// implemented in the Intel universe need to be bridged to the PowerPC universe, and when such a bridge is
	// necessary, we cannot perform relocations to offsets of symbols because the Intel offset will be
	// completely unrelated.
	// the "LostInTimeAndSpace" universe tells that a symbol has no associated universe. This is usually bad and such
	// a symbol cannot be used.
	enum class SymbolUniverse
	{
		LostInTimeAndSpace,
		PowerPC,
		Intel
	};
	
	struct ResolvedSymbol
	{
		SymbolUniverse Universe;
		std::string Name;
		uint32_t Address;
		
		static const ResolvedSymbol Invalid;
		
		ResolvedSymbol(SymbolUniverse universe, const std::string& name, uint32_t address);
		
		static ResolvedSymbol PowerPCSymbol(const std::string& name, uint32_t address);
		static ResolvedSymbol IntelSymbol(const std::string& name, uint32_t address);
	};
	
	class SymbolResolver
	{
	public:
		static const std::string MainSymbolName;
		
		virtual std::vector<ResolvedSymbol> GetEntryPoints() const = 0;
		
		virtual const std::string* FilePath() const = 0;
		virtual std::vector<std::string> SymbolList() const = 0;
		
		virtual ResolvedSymbol ResolveSymbol(const std::string& name) = 0;
		
		virtual ~SymbolResolver() = 0;
	};
}

#endif /* defined(__pefdump__SymbolResolver__) */
