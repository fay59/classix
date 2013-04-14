//
// InterfaceLib.h
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

#include <type_traits>
#include <unordered_map>

#include "CommonDefinitions.h"
#include "IAllocator.h"
#include "MachineState.h"
#include "SymbolType.h"
#include "BigEndian.h"
#include "ResourceManager.h"
#include "GrafPortManager.h"
#include "UIChannel.h"

namespace InterfaceLib
{
	struct Globals
	{
		// the padding helps make it less harmful if a program was to overwrite stuff from the globals
		uint8_t padding[0x1000];
		
		Common::IAllocator& allocator;
		ResourceManager resources;
		GrafPortManager grafPorts;
		UIChannel ipc;
		
		uint32_t systemFatalErrorHandler;
		
		Globals(Common::IAllocator& allocator);
		
		void RefreshWindows();
	};
}

extern "C"
{
	InterfaceLib::Globals* LibraryLoad(Common::IAllocator* allocator);
	SymbolType LibraryLookup(InterfaceLib::Globals* globals, const char* symbolName, void** symbol);
	void LibraryUnload(InterfaceLib::Globals* context);
	extern const char* LibrarySymbolNames[];
	
	void InterfaceLib___LibraryInit(InterfaceLib::Globals* globals, PPCVM::MachineState* state);
}

