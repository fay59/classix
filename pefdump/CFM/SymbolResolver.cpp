//
//  SymbolResolver.cpp
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "SymbolResolver.h"

namespace CFM
{
	const ResolvedSymbol ResolvedSymbol::Invalid(SymbolUniverse::LostInTimeAndSpace, 0);
	
	ResolvedSymbol::ResolvedSymbol(SymbolUniverse universe, intptr_t address)
	{
		Universe = universe;
		Address = address;
	}
	
	ResolvedSymbol ResolvedSymbol::PowerPCSymbol(intptr_t address)
	{
		return ResolvedSymbol(SymbolUniverse::PowerPC, address);
	}
	
	ResolvedSymbol ResolvedSymbol::IntelSymbol(intptr_t address)
	{
		return ResolvedSymbol(SymbolUniverse::Intel, address);
	}
	
	CFM::SymbolResolver::~SymbolResolver()
	{ }
}