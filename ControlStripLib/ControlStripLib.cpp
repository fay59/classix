//
// ControlStripLib.cpp
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

#include <dlfcn.h>

#include "ControlStripLib.h"
#include "Managers.h"
#include "MachineState.h"
#include "NotImplementedException.h"

namespace ControlStripLib
{
	struct Globals
	{
		Common::IAllocator& allocator;
		
		Globals(Common::IAllocator& allocator)
		: allocator(allocator)
		{ }
	};
}

extern "C"
{
	ControlStripLib::Globals* LibraryLoad(Common::IAllocator* allocator, OSEnvironment::Managers* managers)
	{
		managers->Gestalt().SetValue("sdev", 0);
		return allocator->Allocate<ControlStripLib::Globals>("ControlStripLib Globals", *allocator);
	}
	
	SymbolType LibraryLookup(ControlStripLib::Globals* globals, const char* name, void** result)
	{
		char functionName[42] = "ControlStripLib_";
		char* end = stpncpy(functionName + 16, name, 25);
		if (*end == 0)
		{
			if (void* symbol = dlsym(RTLD_SELF, functionName))
			{
				*result = symbol;
				return CodeSymbol;
			}
		}
		
		*result = nullptr;
		return SymbolNotFound;
	}
	
	void LibraryUnload(ControlStripLib::Globals* globals)
	{
		globals->allocator.Deallocate(globals);
	}
	
#pragma mark -
	
	void ControlStripLib_SBGetBarGraphWidth(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBTrackPopupMenu(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBIsShowHideHotKeyEnabled(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBSavePreferences(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBIsControlStripVisible(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBEnableShowHideHotKey(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBShowHelpString(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBModalDialogInContext(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBGetDetachedIndString(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBSetControlStripFontID(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBGetShowHideHotKey(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBShowHideControlStrip(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBTrackSlider(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBSetShowHideHotKey(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBGetControlStripFontSize(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBOpenModuleResourceFile(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBGetDetachIconSuite(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBHitTrackSlider(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBDrawBarGraph(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBGetControlStripFontID(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBSafeToAccessStartupDisk(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBLoadPreferences(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void ControlStripLib_SBSetControlStripFontSize(ControlStripLib::Globals* globals, PPCVM::MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
}
