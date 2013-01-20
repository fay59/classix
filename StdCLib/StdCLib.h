//
// StdCLib.h
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

#ifndef Classix_StdCLib_h
#define Classix_StdCLib_h

#include "IAllocator.h"
#include "MachineState.h"
#include "SymbolType.h"

extern "C"
{
	namespace StdCLib
	{
		struct Globals;
	}

	StdCLib::Globals* LibraryInit(Common::IAllocator* allocator);
	SymbolType LibraryLookup(StdCLib::Globals* globals, const char* symbolName, void** symbol);
	void LibraryFinit(StdCLib::Globals* context);
	
	extern const char* LibrarySymbolNames[];
}

#endif
