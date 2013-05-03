//
// OSUtils.cpp
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

void InterfaceLib_Delay(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Dequeue(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DTInstall(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Enqueue(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FlushCodeCacheRange(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetSysPPtr(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitUtil(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_IsMetric(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MakeDataExecutable(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReadLocation(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetA5(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetCurrentA5(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SysEnvirons(InterfaceLib::Globals* globals, MachineState* state)
{
	if (state->r3 < 1)
	{
		state->r3 = 0xea83;
		return;
	}
	
	if (state->r3 > 2)
	{
		state->r3 = 0xea82;
		return;
	}
	
	// mostly based on the output I get from running this in SheepShaver, I should probably
	// run that on hardware at some point
	SysEnvRec& record = *globals->allocator.ToPointer<SysEnvRec>(state->r4);
	record.environsVersion = static_cast<int16_t>(state->r3);
	record.machineType = 0x41;
	record.systemVersion = 0x0922; // faking mac os 9.2.2
	record.processor = 3;
	record.hasFPU = false; // we don't have FPU support yet
	record.hasColorQD = true;
	record.keyBoardType = 9;
	record.atDrvrVersNum = 0;
	record.sysVRefNum = 0x80c3;
	state->r3 = 0;
}

void InterfaceLib_TickCount(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_WriteLocation(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_WriteParam(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

