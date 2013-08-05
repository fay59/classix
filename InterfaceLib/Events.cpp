//
// Events.cpp
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

#include "Prototypes.h"
#include "InterfaceLib.h"
#include "NotImplementedException.h"

using namespace InterfaceLib;

namespace
{
	void RefreshWindows(InterfaceLib::Globals* globals)
	{
		
	}
}

void InterfaceLib_Button(InterfaceLib::Globals* globals, MachineState* state)
{
	globals->ipc().PerformAction<void>(IPCMessage::RefreshWindows);
	state->r3 = globals->ipc().PerformAction<bool>(IPCMessage::IsMouseDown);
}

void InterfaceLib_EventAvail(InterfaceLib::Globals* globals, MachineState* state)
{
	// same as GetNextEvent, but without discarding the event if it's matched
	globals->ipc().PerformAction<void>(IPCMessage::RefreshWindows);
	
	EventMask mask = static_cast<EventMask>(state->r3);
	uint32_t timeout = 0xffffffff;
	
	MacRegion empty;
	empty.rgnSize = 10;
	empty.rgnBBox.top = 0;
	empty.rgnBBox.left = 0;
	empty.rgnBBox.right = 0;
	empty.rgnBBox.bottom = 0;
	
	EventRecord nextEvent = globals->ipc().PerformAction<EventRecord>(IPCMessage::PeekNextEvent, mask, timeout, empty);
	
	*globals->allocator.ToPointer<EventRecord>(state->r4) = nextEvent;
	state->r3 = nextEvent.what != 0;
}

void InterfaceLib_FlushEvents(InterfaceLib::Globals* globals, MachineState* state)
{
	EventMask discardMask = static_cast<EventMask>(state->r3);
	EventMask stopMask = static_cast<EventMask>(state->r4);
	globals->ipc().PerformAction<void>(IPCMessage::DiscardEventsUntil, discardMask, stopMask);
}

void InterfaceLib_GetCaretTime(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetDblTime(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetEvQHdr(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetKeys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetMouse(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetNextEvent(InterfaceLib::Globals* globals, MachineState* state)
{
	globals->ipc().PerformAction<void>(IPCMessage::RefreshWindows);
	
	EventMask mask = static_cast<EventMask>(state->r3);
	uint32_t timeout = 0xffffffff;
	
	MacRegion empty;
	empty.rgnSize = 10;
	empty.rgnBBox.top = 0;
	empty.rgnBBox.left = 0;
	empty.rgnBBox.right = 0;
	empty.rgnBBox.bottom = 0;
	
	EventRecord nextEvent = globals->ipc().PerformAction<EventRecord>(IPCMessage::PeekNextEvent, mask, timeout, empty);
	globals->ipc().PerformAction<void>(IPCMessage::DequeueNextEvent, mask);
	
	*globals->allocator.ToPointer<EventRecord>(state->r4) = nextEvent;
	state->r3 = nextEvent.what != 0;
}

void InterfaceLib_GetOSEvent(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_IsCmdChar(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_KeyScript(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_KeyTranslate(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetKbdLast(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetKbdType(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetKeyRepThresh(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetKeyThresh(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetKbdLast(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetKbdType(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetKeyRepThresh(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetKeyThresh(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OSEventAvail(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PostEvent(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PPostEvent(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetEventMask(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StillDown(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SystemClick(InterfaceLib::Globals* globals, MachineState* state)
{
	// purposefully does nothing on OS X
}

void InterfaceLib_SystemEvent(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SystemTask(InterfaceLib::Globals* globals, MachineState* state)
{
	globals->ipc().PerformAction<void>(IPCMessage::RefreshWindows);
}

void InterfaceLib_WaitMouseUp(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_WaitNextEvent(InterfaceLib::Globals* globals, MachineState* state)
{
	globals->ipc().PerformAction<void>(IPCMessage::RefreshWindows);
	
	MacRegion emptyRegion;
	emptyRegion.rgnSize = 10;
	
	EventMask mask = static_cast<EventMask>(state->r3);
	uint32_t ticksTimeout = state->r5; // tick = 1/60s
	MacRegion* region;
	if (Common::UInt32* regionHandle = globals->allocator.ToPointer<Common::UInt32>(state->r6, true))
	{
		region = globals->allocator.ToPointer<MacRegion>(*regionHandle);
	}
	else
	{
		region = &emptyRegion;
	}
	
	EventRecord nextEvent = globals->ipc().PerformAction<EventRecord>(IPCMessage::PeekNextEvent, mask, ticksTimeout, *region);
	globals->ipc().PerformAction<void>(IPCMessage::DequeueNextEvent, mask);
	*globals->allocator.ToPointer<EventRecord>(state->r4) = nextEvent;
	
	state->r3 = nextEvent.what != 0;
}
