//
// DebugLib.h
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

#ifndef __Classix__DebugLib__
#define __Classix__DebugLib__

#include <list>
#include <unordered_map>

#include "LibraryResolver.h"
#include "SymbolResolver.h"
#include "STAllocator.h"
#include "Structures.h"

// DebugLib supports debugger-initiated operations through the function call interface

struct DebugLibContext;

class DebugLib : public CFM::LibraryResolver, public CFM::SymbolResolver
{
	Common::Allocator& allocator;
	DebugLibContext* context;
	
	std::list<PEF::TransitionVector, Common::STAllocator<PEF::TransitionVector>> transitions;
	std::unordered_map<std::string, CFM::ResolvedSymbol> symbols;
	
public:
	static const std::string LibraryName;
	
	DebugLib(Common::Allocator& allocator);
	
	// LibraryResolver
	virtual SymbolResolver* ResolveLibrary(const std::string& name) override;
	
	// SymbolResolver
	virtual const std::string* FilePath() const override;
	virtual std::vector<std::string> SymbolList() const override;
	virtual CFM::ResolvedSymbol ResolveSymbol(const std::string& name) override;
	virtual std::vector<CFM::ResolvedSymbol> GetEntryPoints() override;
	
	virtual ~DebugLib();
};

#endif /* defined(__Classix__DebugLib__) */
