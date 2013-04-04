//
// InterfaceLib.h
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

#include <unistd.h>

#include "IAllocator.h"
#include "MachineState.h"
#include "SymbolType.h"
#include "BigEndian.h"
#include "ResourceManager.h"

namespace InterfaceLib
{
	using namespace Common;
	typedef SInt16 Bits16[16];
	
	struct Point
	{
		SInt16 v;
		SInt16 h;
	};
	
	struct Rect
	{
		SInt16 top;
		SInt16 left;
		SInt16 bottom;
		SInt16 right;
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
		UInt16 rgnSize;
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
		UInt32 procs[13];
	};
	
	struct GrafPort
	{
		SInt16 device;
		BitMap portBits;
		Rect portRect;
		UInt32 visRgn; // MacRegion**
		UInt32 clipRgn; // MacRegion**
		Pattern bkPat;
		Pattern fillPat;
		Point pnLoc;
		Point pnSize;
		SInt16 pnMode;
		Pattern pnPat;
		SInt16 pnVis;
		SInt16 txFont;
		Style txFace;
		
		SInt16 txMode;
		SInt16 txSize;
		SInt32 spExtra;
		SInt32 fgColor;
		SInt32 bkColor;
		SInt16 colrBit;
		SInt16 patStretch;
		UInt32 picSave; // Handle
		UInt32 rgnSave; // Handle
		UInt32 polySave; // Handle
		QDProcs procs;
	};
	
	union Pipe
	{
		int fd[2];
		struct
		{
			int read;
			int write;
		};
	};
	
	struct Globals
	{
		GrafPort port;
		uint8_t padding[0x1000];
		
		Common::IAllocator& allocator;
		ResourceManager resources;
		Pipe read;
		Pipe write;
		pid_t head;
		
		Globals(Common::IAllocator& allocator);
		~Globals();
	};
}

extern "C"
{
	InterfaceLib::Globals* LibraryLoad(Common::IAllocator* allocator);
	SymbolType LibraryLookup(InterfaceLib::Globals* globals, const char* symbolName, void** symbol);
	void LibraryUnload(InterfaceLib::Globals* context);
	extern const char* LibrarySymbolNames[];
	
	void InterfaceLib___LibraryInit(InterfaceLib::Globals* globals, PPCVM::MachineState* state);
}

