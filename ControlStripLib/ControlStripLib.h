//
// ControlStripLib.h
// Classix
//
// Copyright (C) 2013 Félix Cloutier
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

#ifndef __Classix__File__
#define __Classix__File__

#include "IAllocator.h"
#include "SymbolType.h"

namespace ControlStripLib
{
	struct Globals;
}

namespace OSEnvironment
{
	class Managers;
}

extern "C"
{
	ControlStripLib::Globals* LibraryLoad(Common::IAllocator* allocator, OSEnvironment::Managers* managers);
	SymbolType LibraryLookup(ControlStripLib::Globals* globals, const char* symbolName, void** symbol);
	void LibraryUnload(ControlStripLib::Globals* context);
	
	extern const char* LibrarySymbolNames[];
}

#endif /* defined(__Classix__File__) */
