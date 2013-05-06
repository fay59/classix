//
// MacMemory.cpp
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

void InterfaceLib_ApplicationZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_BlockMove(InterfaceLib::Globals* globals, MachineState* state)
{
	const uint8_t* source = globals->allocator.ToPointer<uint8_t>(state->r3);
	uint8_t* destination = globals->allocator.ToPointer<uint8_t>(state->r4);
	size_t size = state->r5;
	memmove(destination, source, size);
}

void InterfaceLib_BlockMoveData(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CompactMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CompactMemSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DebuggerEnter(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DebuggerExit(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DebuggerGetMax(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DebuggerLockMemory(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DebuggerPoll(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DebuggerUnlockMemory(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DeferUserFn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposeHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	uint8_t* pointer = globals->allocator.ToPointer<uint8_t>(state->r3);
	pointer -= 4;
	globals->allocator.Deallocate(pointer);
}

void InterfaceLib_DisposePtr(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EmptyHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EnterSupervisorMode(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FlushMemory(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FreeMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FreeMemSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetApplLimit(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetHandleSize(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPageState(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPhysical(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPtrSize(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetVolumeVirtualMemoryInfo(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GZSaveHnd(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HandAndHand(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HandleZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HandToHand(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HClrRBit(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HGetState(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HLock(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HLockHi(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HNoPurge(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HoldMemory(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HPurge(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HSetRBit(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HSetState(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HUnlock(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitApplZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InlineGetHandleSize(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LockMemory(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LockMemoryContiguous(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LockMemoryForOutput(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MakeMemoryNonResident(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MakeMemoryResident(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MaxApplZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MaxBlock(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MaxBlockSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MaxMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MaxMemSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MemError(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MoreMasters(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MoveHHi(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewEmptyHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewEmptyHandleSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	size_t size = state->r3 + sizeof(uint32_t);
	std::stringstream handleName;
	handleName << "InterfaceLib Handle [" << state->r3 << " + 4]";
	uint8_t* bytes = globals->allocator.Allocate(handleName.str(), size);
	Common::UInt32& pointer = *reinterpret_cast<Common::UInt32*>(bytes);
	pointer = globals->allocator.ToIntPtr(bytes) + 4;
	state->r3 = globals->allocator.ToIntPtr(&pointer);
}

void InterfaceLib_NewHandleClear(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewHandleSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewHandleSysClear(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewPtr(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewPtrClear(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewPtrSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewPtrSysClear(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PageFaultFatal(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PtrAndHand(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PtrToHand(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PtrToXHand(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PtrZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PurgeMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PurgeMemSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PurgeSpace(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PurgeSpaceContiguous(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PurgeSpaceSysContiguous(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PurgeSpaceSysTotal(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PurgeSpaceTotal(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReallocateHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReallocateHandleSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RecoverHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RecoverHandleSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReleaseMemoryData(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReserveMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReserveMemSys(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetApplBase(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetApplLimit(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetGrowZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetHandleSize(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetPtrSize(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StackSpace(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SystemZone(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TempDisposeHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TempFreeMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TempHLock(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TempHUnlock(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TempMaxMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TempNewHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TempTopMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TopMem(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_UnholdMemory(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_UnlockMemory(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

