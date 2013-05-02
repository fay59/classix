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
#include <string>

template<typename... TArgument>
void PACK_EXPAND_identity(TArgument&&...) {}

#define PACK_EXPAND(x) PACK_EXPAND_identity((x)...)

// I'm sooooo looking forward to generalized attributes, so I don't have to use __attribute__((packed)).

namespace InterfaceLib
{
	inline std::string PascalStringToCPPString(const char* pascalString)
	{
		uint8_t length = *pascalString;
		return std::string(pascalString + 1, pascalString + 1 + length);
	}
	
	typedef Common::SInt16 Bits16[16];
	
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
	
	enum class EventModifierFlags : uint16_t
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
		
	enum class EventCode : uint16_t
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
	
	enum class EventMask : uint16_t
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
	
	enum class WindowPartCode : int16_t
	{
		inDesk = 0,
		inMenuBar = 1,
		inSysWindow = 2,
		inContent = 3,
		inDrag = 4,
		inGrow = 5,
		inGoAway = 6,
		inZoomIn = 7,
		inZoomOut = 8,
		inCollapseBox = 11,
		inProxyIcon = 12
	};
	
	struct __attribute__((packed)) Point
	{
		inline static Point FromWord(uint32_t word)
		{
			Point result;
			result.h = word & 0xffff;
			result.v = word >> 16;
			return result;
		}
		
		Common::SInt16 v;
		Common::SInt16 h;
	};
	
	struct __attribute__((packed)) Rect
	{
		Common::SInt16 top;
		Common::SInt16 left;
		Common::SInt16 bottom;
		Common::SInt16 right;
	};
	
	struct __attribute__((packed)) Pattern
	{
		uint8_t pat[8];
	};
	
	struct __attribute__((packed)) BitMap
	{
		Common::UInt32 baseAddr;
		Common::SInt16 rowBytes;
		Rect bounds;
	};
	
	struct __attribute__((packed)) Cursor
	{
		Bits16 data;
		Bits16 mask;
		Point hotSpot;
	};
	
	struct __attribute__((packed)) MacRegion
	{
		Common::UInt16 rgnSize;
		Rect rgnBBox;
	};
		
	struct __attribute__((packed)) MacRegionMax
	{
		Common::UInt16 rgnSize;
		InterfaceLib::Rect rgnBBox;
		uint8_t rgnData[0x8000 - 10];
		
		MacRegionMax() = default;
		MacRegionMax(const MacRegion& region);
	};
	
	struct __attribute__((packed)) RGBColor
	{
		Common::UInt16 red;
		Common::UInt16 green;
		Common::UInt16 blue;
	};
	
	struct __attribute__((packed)) ColorSpec
	{
		Common::SInt16 value;
		RGBColor rgb;
	};
	
	struct __attribute__((packed)) ColorTable
	{
		Common::SInt32 seed; // presumably unique ID
		Common::SInt16 flags;
		Common::SInt16 count;
		ColorSpec table[0];
	};
	
	struct __attribute__((packed)) PixMap
	{
		void* baseAddr;
		Common::SInt16 rowBytes;
		InterfaceLib::Rect bounds;
		Common::SInt16 pmVersion;
		Common::SInt16 packType;
		Common::SInt32 packSize;
		Common::UInt32 hRes;
		Common::UInt32 vRes;
		Common::SInt16 pixelType;
		Common::SInt16 pixelSize;
		Common::SInt16 cmpCount;
		Common::SInt16 cmpSize;
		Common::UInt32 pixelFormat; // fourCharCode
		Common::UInt32 pmTable; // ColorTable**
		Common::UInt32 pmExt;
	};
	
	struct __attribute__((packed)) QDProcs
	{
		Common::UInt32 procs[13];
	};
	
	struct __attribute__((packed)) QDGlobals
	{
		uint8_t privates[76];
		Common::SInt32 randSeed;
		BitMap screenBits;
		Cursor arrow;
		Pattern dkGray;
		Pattern ltGray;
		Pattern gray;
		Pattern black;
		Pattern white;
		Common::UInt32 thePort;
	};
	
	struct __attribute__((packed)) GrafPort
	{
		GrafPort() = default;
		GrafPort(const GrafPort&) = delete;
		GrafPort(GrafPort&&) = delete;
		
		Common::SInt16 device;
		InterfaceLib::BitMap portBits;
		InterfaceLib::Rect portRect;
		Common::UInt32 visRgn; // MacRegion**
		Common::UInt32 clipRgn; // MacRegion**
		InterfaceLib::Pattern bkPat;
		InterfaceLib::Pattern fillPat;
		InterfaceLib::Point pnLoc;
		InterfaceLib::Point pnSize;
		Common::SInt16 pnMode;
		InterfaceLib::Pattern pnPat;
		Common::SInt16 pnVis;
		Common::SInt16 txFont;
		InterfaceLib::Style txFace;
		
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
		Common::UInt32 procs; // QDProcs*
	};
	
	struct __attribute__((packed)) CGrafPort
	{
		CGrafPort() = default;
		CGrafPort(const CGrafPort&) = delete;
		CGrafPort(CGrafPort&&) = delete;
		
		Common::SInt16 device;
		
		Common::UInt32 portPixMap; // PixMap**
		Common::SInt16 portVersion;
		Common::UInt32 grafVars;
		Common::SInt16 chExtra;
		Common::SInt16 pnLocHFrac;
		
		InterfaceLib::Rect portRect;
		Common::UInt32 visRgn; // MacRegion**
		Common::UInt32 clipRgn; // MacRegion**
		Common::UInt32 bkPixPat;
		InterfaceLib::RGBColor rgbFgColor;
		InterfaceLib::RGBColor rgbBgColor;
		InterfaceLib::Point pnLoc;
		InterfaceLib::Point pnSize;
		Common::SInt16 pnMode;
		Common::UInt32 pnPixPat;
		Common::UInt32 fillPixPat;
		Common::SInt16 pnVis;
		Common::SInt16 txFont;
		InterfaceLib::Style style;
		uint8_t padding;
		Common::SInt16 txMode;
		Common::SInt16 txSize;
		Common::SInt32 spExtra;
		Common::SInt32 fgColor;
		Common::SInt32 bkColor;
		Common::SInt16 colrBit;
		Common::UInt32 picSave; // Handle
		Common::UInt32 rgnSave; // Handle
		Common::UInt32 polySave; // Handle
		Common::UInt32 procs; // QDProcs*
	};
		
	union UGrafPort
	{
		inline UGrafPort() {}
		UGrafPort(const UGrafPort&) = delete;
		UGrafPort(UGrafPort&&) = delete;
		
		GrafPort gray;
		CGrafPort color;
		
		inline bool IsColor()
		{
			return (color.portVersion >> 14) != 0;
		}
	};
	
	struct __attribute__((packed)) EventRecord
	{
		Common::UInt16 what;
		Common::UInt32 message;
		Common::UInt32 when;
		Point where;
		Common::UInt16 modifiers;
	};
	
	struct __attribute__((packed)) SysEnvRec
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
		
	enum class IPCMessage : unsigned
	{
		Beep,
		
		PeekNextEvent,
		DequeueNextEvent,
		IsMouseDown,
		
		CreateWindow,
		FindFrontWindow,
		FindWindowByCoordinates,
		SetDirtyRect,
		RefreshWindows,
		DragWindow,
		
		ClearMenus,
		InsertMenu,
		InsertMenuItem,
		MenuSelect,
		MenuKey,
		CheckItem,
	};
}

#endif
