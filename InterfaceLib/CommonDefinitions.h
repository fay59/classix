//
// CommonDefinitions.h
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

#ifndef Classix_CommonDefinitions_h
#define Classix_CommonDefinitions_h

#include "BigEndian.h"

template<typename... TArgument>
void PACK_EXPAND_identity(TArgument&&...) {}

#define PACK_EXPAND(x) PACK_EXPAND_identity((x)...)

namespace InterfaceLib
{
	typedef Common::SInt16 Bits16[16];
	
	struct Point
	{
		Common::SInt16 v;
		Common::SInt16 h;
	};
	
	struct Rect
	{
		Common::SInt16 top;
		Common::SInt16 left;
		Common::SInt16 bottom;
		Common::SInt16 right;
	};
	
	struct Pattern
	{
		uint8_t pat[8];
	};
	
	struct BitMap
	{
		Common::UInt32 baseAddr;
		Common::SInt16 rowBytes;
		Rect bounds;
	};
	
	struct Cursor
	{
		Bits16 data;
		Bits16 mask;
		Point hotSpot;
	};
	
	struct MacRegion
	{
		Common::UInt16 rgnSize;
		Rect rgnBBox;
	};
	
	enum class Style : uint8_t
	{
		Normal,
		Bold,
		Italic,
		Underline,
		Outline,
		Shadow,
		Condense,
		Extend
	};
	
	struct QDProcs
	{
		Common::UInt32 procs[13];
	};
	
	struct GrafPort
	{
		Common::SInt16 device;
		BitMap portBits;
		Rect portRect;
		Common::UInt32 visRgn; // MacRegion**
		Common::UInt32 clipRgn; // MacRegion**
		Pattern bkPat;
		Pattern fillPat;
		Point pnLoc;
		Point pnSize;
		Common::SInt16 pnMode;
		Pattern pnPat;
		Common::SInt16 pnVis;
		Common::SInt16 txFont;
		Style txFace;
		
		Common::SInt16 txMode;
		Common::SInt16 txSize;
		Common::SInt32 spExtra;
		Common::SInt32 fgColor;
		Common::SInt32 bkColor;
		Common::SInt16 colrBit;
		Common::SInt16 patStretch;
		Common::UInt32 picSave; // Handle
		Common::UInt32 rgnSave; // Handle
		Common::UInt32 polySave; // Handle
		QDProcs procs;
	};
	
	struct EventRecord
	{
		Common::UInt16 what;
		Common::UInt32 message;
		Common::UInt32 when;
		Point where;
		Common::UInt16 modifiers;
	};
	
	struct SysEnvRec
	{
		Common::SInt16 environsVersion;
		Common::SInt16 machineType;
		Common::SInt16 systemVersion;
		Common::SInt16 processor;
		uint8_t hasFPU;
		uint8_t hasColorQD;
		Common::SInt16 keyBoardType;
		Common::SInt16 atDrvrVersNum;
		Common::SInt16 sysVRefNum;
	};
	
	enum class EventModifierFlags : unsigned short
	{
		activeFlag = 1, // set when the foreground window is being activated
		mouseButtonState = 128, // set when the mouse button is up
		cmdKey = 256,
		shiftKey = 512,
		alphaLock = 1024, // set when caps lock is activated
		optionKey = 2048,
		controlKey = 4096,
		rightShiftKey = 8192,
		rightOptionKey = 16384,
		rightControlKey = 32768
	};
	
	enum class EventCode : unsigned short
	{
		nullEvent,
		mouseDown,
		mouseUp,
		keyDown,
		keyUp,
		autoKey,
		updateEvent,
		diskEvent,
		activateEvent,
		osEvent = 15,
		highLevelEvent = 23
	};
	
	enum class EventMask : unsigned short
	{
		noEvent = 0,
		mDownMask = 2,		// mouse down
		mUpMask = 4,		// mouse up
		keyDownMask = 8,	// key down
		keyUpMask = 16,		// key up
		autoKeyMask = 32,	// key repeat
		updateMask = 64,	// update (?)
		diskMask = 128,		// disk inserted
		activMask = 256,	// app activated
		highLevelEventMask = 0x400,
		osMask = 0x1000,
		everyEvent = 0xffff
	};
		
	enum class IPCMessage : unsigned
	{
		Beep,
		
		PeekNextEvent,
		DequeueNextEvent,
	};
}

#endif
