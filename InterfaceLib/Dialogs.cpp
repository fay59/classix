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
#include <algorithm>
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
	for (size_t i = 0; i < globals->dialogParams.size(); i++)
	{
		const char* pascalString = globals->allocator.ToPointer<char>(state->gpr[3 + i]);
		uint8_t length = *pascalString;
		pascalString++;
		
		globals->dialogParams[i] = std::string(pascalString, pascalString + length);
	}
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
	// TODO this needs to be soooooo much better
	uint32_t key = 0;
	uint16_t templateKey = static_cast<uint16_t>(state->r3);
	
	Resources::ALRT* alert = globals->resources().GetResource<Resources::ALRT>(templateKey);
	globals->ipc().PerformAction<void>(IPCMessage::CreateDialog, key, alert->bounds, true, std::string("Stop!"));
	
	Resources::DITL* ditl = globals->resources().GetResource<Resources::DITL>(alert->ditl);
	auto enumerator = ditl->EnumerateControls();
	std::string paramString = "^0";
	while (enumerator.HasItem())
	{
		Control control = enumerator.GetControl();
		// replace string param placeholders
		for (size_t i = 0; i < globals->dialogParams.size(); i++)
		{
			paramString[1] = static_cast<decltype(paramString)::value_type>('0' + i);
			std::string::size_type paramPosition = control.label.find(paramString);
			while (paramPosition != std::string::npos)
			{
				control.label.replace(paramPosition, paramString.length(), globals->dialogParams[i]);
				paramPosition = control.label.find(paramString, paramPosition + globals->dialogParams[i].length());
			}
		}
		
		globals->ipc().PerformAction<void>(IPCMessage::CreateControl, key, control.type, control.enabled, control.bounds, control.label);
		enumerator.MoveNext();
	}
	
	// only return when the dialog is closed
	abort();
}

void InterfaceLib_UpdateDialog(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

