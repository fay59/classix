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
	
	enum class EventMask : uint16_t
	{
		mDownMask = 2,
		mUpMask = 4,
		keyDownMask = 8,
		keyUpMask = 16,
		autoKeyMask = 32,
		updateMask = 64,
		diskMask = 128,
		activMask = 256,
		networkMask = 1024,
		driverMask = 2048,
		app1Mask = 4096,
		app2Mask = 8192,
		app3Mask = 16384,
		app4Mask = 32768,
		everyEvent = 0xffff
	};
}

#endif
