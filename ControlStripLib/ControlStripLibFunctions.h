//
// ControlStripLibFunctions.h
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

#ifndef __Classix__ControlStripLibFunctions__
#define __Classix__ControlStripLibFunctions__

namespace PPCVM
{
	struct MachineState;
}

namespace ControlStripLib
{
	struct Globals;
}

extern "C"
{
	void ControlStripLib_SBGetBarGraphWidth(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBTrackPopupMenu(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBIsShowHideHotKeyEnabled(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBSavePreferences(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBIsControlStripVisible(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBEnableShowHideHotKey(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBShowHelpString(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBModalDialogInContext(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBGetDetachedIndString(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBSetControlStripFontID(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBGetShowHideHotKey(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBShowHideControlStrip(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBTrackSlider(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBSetShowHideHotKey(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBGetControlStripFontSize(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBOpenModuleResourceFile(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBGetDetachIconSuite(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBHitTrackSlider(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBDrawBarGraph(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBGetControlStripFontID(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBSafeToAccessStartupDisk(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBLoadPreferences(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
	void ControlStripLib_SBSetControlStripFontSize(ControlStripLib::Globals* globals, PPCVM::MachineState* state);
}

#endif /* defined(__Classix__ControlStripLibFunctions__) */
