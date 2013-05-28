//
// Gestalt.cpp
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
#include "NotImplementedException.h"
#include "InterfaceLib.h"

// see http://developer.apple.com/library/mac/#documentation/Carbon/reference/Gestalt_Manager/Reference/reference.html

template<char A1, char A2, char A3, char A4>
struct CharCode
{
	static constexpr uint32_t Value = (A1 << 24) | (A2 << 16) | (A3 << 8) | A4;
};

void InterfaceLib_DeleteGestaltValue(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Gestalt(InterfaceLib::Globals* globals, MachineState* state)
{
	Common::SInt32& result = *globals->allocator.ToPointer<Common::SInt32>(state->r4);
	switch (state->r3)
	{
		case CharCode<'t', 'h', 'd', 's'>::Value: // thread manager
			result = 0; // absolutely no threading support right now
			break;
			
		default:
			result = 0;
			state->r3 = -5551; // gestaltUndefSelectorErr
			return;
	}
	
	state->r3 = 0;
}

void InterfaceLib_NewGestalt(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewGestaltValue(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReplaceGestalt(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReplaceGestaltValue(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetGestaltValue(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

