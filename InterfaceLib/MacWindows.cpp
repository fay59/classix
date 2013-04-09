//
// MacWindows.cpp
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

using namespace InterfaceLib;

void InterfaceLib_BeginUpdate(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_BringToFront(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CalcVis(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CalcVisBehind(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CheckUpdate(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ClipAbove(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposeWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DragGrayRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DragTheRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DragWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DrawGrowIcon(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DrawNew(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EndUpdate(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FrontWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetAuxWin(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetCWMgrPort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetGrayRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetNewCWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetNewWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetWindowPic(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetWMgrPort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetWRefCon(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetWTitle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetWVariant(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GrowWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HideWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HiliteWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitWindows(InterfaceLib::Globals* globals, MachineState* state)
{
	// nothing to do here
}

void InterfaceLib_InvalRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InvalRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewCWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	// TODO check ABI for parameters, I'm not entirely sure so many of them are passed by registers
	
	// We use storage pointers as window indentifiers in the UI head, since they're guaranteed to be unique
	// for ay given window.
	uint32_t key = state->r3;
	GrafPort* port;
	
	// We try to get the title first, since we can use it for the allocation name.
	ShortString title;
	const char* pascalTitle = globals->allocator.ToPointer<const char>(state->r5);
	std::string cppTitle = PascalStringToCPPString(pascalTitle);
	strncpy(title, cppTitle.c_str(), sizeof title);
	
	const InterfaceLib::Rect& rect = *globals->allocator.ToPointer<const InterfaceLib::Rect>(state->r4);
	
	if (key == 0)
	{
		std::stringstream ss;
		ss << "Window: \"" << cppTitle << "\"";
		port = &globals->grafPorts.AllocateGrafPort(rect.right - rect.left, rect.top - rect.bottom, cppTitle);
		key = globals->allocator.ToIntPtr(port);
	}
	else
	{
		port = globals->allocator.ToPointer<GrafPort>(key);
	}
	
	bool visible = state->r6 != 0;
	uint32_t createBehind = state->r8;
	
	globals->ipc.PerformAction<void>(IPCMessage::CreateWindow, key, rect, visible, title, createBehind);
	state->r3 = key;
}

void InterfaceLib_NewWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PaintBehind(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PaintOne(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PinRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SaveOld(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SelectWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SendBehind(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDeskCPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetWinColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetWindowPic(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetWRefCon(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetWTitle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ShowHide(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SizeWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TrackBox(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TrackGoAway(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ValidRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ValidRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ZoomWindow(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

