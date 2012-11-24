//
// SymbolResolver.cpp
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

#include "SymbolResolver.h"

namespace CFM
{
	const ResolvedSymbol ResolvedSymbol::Invalid(SymbolUniverse::LostInTimeAndSpace, "<invalid>", 0);
	
	ResolvedSymbol::ResolvedSymbol(SymbolUniverse universe, const std::string& name, intptr_t address)
	: Name(name)
	{
		Universe = universe;
		Address = address;
	}
	
	ResolvedSymbol ResolvedSymbol::PowerPCSymbol(const std::string& name, intptr_t address)
	{
		return ResolvedSymbol(SymbolUniverse::PowerPC, name, address);
	}
	
	ResolvedSymbol ResolvedSymbol::IntelSymbol(const std::string& name, intptr_t address)
	{
		return ResolvedSymbol(SymbolUniverse::Intel, name, address);
	}
	
	CFM::SymbolResolver::~SymbolResolver()
	{ }
}