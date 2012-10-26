//
//  SymbolResolver.h
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__SymbolResolver__
#define __pefdump__SymbolResolver__

#include <cstdint>
#include <string>

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
		intptr_t Address;
		
		static const ResolvedSymbol Invalid;
		
		ResolvedSymbol(SymbolUniverse universe, intptr_t address);
		
		static ResolvedSymbol PowerPCSymbol(intptr_t address);
		static ResolvedSymbol IntelSymbol(intptr_t address);
	};
	
	class SymbolResolver
	{
	public:
		typedef int (*MainSymbol)(int argc, const char** argv);
		
		virtual MainSymbol GetMainSymbol() = 0;
		virtual ResolvedSymbol ResolveSymbol(const std::string& name) = 0;
		virtual ~SymbolResolver() = 0;
	};
}

#endif /* defined(__pefdump__SymbolResolver__) */
