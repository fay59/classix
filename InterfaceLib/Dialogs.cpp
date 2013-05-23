//
// Dialogs.cpp
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

#include <sstream>
#include "Prototypes.h"
#include "NotImplementedException.h"
#include "InterfaceLib.h"
#include "ResourceTypes.h"

using namespace InterfaceLib;
using namespace InterfaceLib::Resources;

void InterfaceLib_Alert(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_AppendDITL(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CautionAlert(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CloseDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CountDITL(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DialogCopy(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DialogCut(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DialogDelete(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DialogPaste(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DialogSelect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposeDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DrawDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ErrorSound(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FindDialogItem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetAlertStage(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetDialogItem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetDialogItemText(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetNewDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
	
	// tentative implementation: I don't feel like going through dialogs right now but I don't feel like removing
	// this, either
	/*
	uint16_t resourceId = static_cast<uint16_t>(state->r3);
	if (DLOG* dialog = globals->resources.GetResource<DLOG>(resourceId))
	{
		uint32_t portAddress = state->r4;
		
		std::string title = dialog->GetTitle();
		Rect rect = dialog->rect;
		
		UGrafPort* port;
		if (portAddress == 0)
		{
			std::stringstream ss;
			ss << "Dialog: \"" << title << "\"";
			port = &globals->grafPorts.AllocateColorGrafPort(rect, nullptr, title);
			portAddress = globals->allocator.ToIntPtr(port);
		}
		else
		{
			port = globals->allocator.ToPointer<UGrafPort>(portAddress);
		}
		
		bool visible = dialog->visibility == 1;
		globals->ipc().PerformAction<void>(IPCMessage::CreateDialog, portAddress, rect, visible, title);
	}
	 */
}

void InterfaceLib_GetStdFilterProc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HideDialogItem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitDialogs(InterfaceLib::Globals* globals, MachineState* state)
{
	globals->systemFatalErrorHandler = state->r3;
}

void InterfaceLib_IsDialogEvent(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ModalDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewColorDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NoteAlert(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ParamText(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ResetAlertStage(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SelectDialogItemText(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDialogCancelItem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDialogDefaultItem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDialogFont(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDialogItem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDialogItemText(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDialogTracksCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ShortenDITL(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ShowDialogItem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdFilterProc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StopAlert(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_UpdateDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

