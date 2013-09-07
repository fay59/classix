//
// DebugLib.cpp
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

#include "DebugLib.h"

using namespace std;
using namespace CFM;
using namespace Common;

struct DebugLibContext
{
	
};

const string DebugLib::LibraryName = ":DebugLib";

DebugLib::DebugLib(Common::Allocator& allocator)
: allocator(allocator), transitions(STAllocator<PEF::TransitionVector>(allocator))
{
	context = allocator.Allocate<DebugLibContext>("DebugLib Globals");
}

SymbolResolver* DebugLib::ResolveLibrary(const string &name)
{
	if (name == LibraryName)
		return this;
	
	return nullptr;
}

const string* DebugLib::FilePath() const
{
	return &LibraryName;
}

vector<string> DebugLib::SymbolList() const
{
	return vector<string>();
}

ResolvedSymbol DebugLib::ResolveSymbol(const string &name)
{
	return ResolvedSymbol::Invalid;
}

vector<ResolvedSymbol> DebugLib::GetEntryPoints()
{
	return vector<ResolvedSymbol>();
}

DebugLib::~DebugLib()
{
	allocator.Deallocate(context);
}
