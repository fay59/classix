//
// Palettes.cpp
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

#include <CoreGraphics/CoreGraphics.h>
#include "Prototypes.h"
#include "NotImplementedException.h"
#include "InterfaceLib.h"

void InterfaceLib_ActivatePalette(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_AnimateEntry(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CopyPalette(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CTab2Palette(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposePalette(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Entry2Index(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetEntryColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetEntryUsage(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetGray(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetNewPalette(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPalette(InterfaceLib::Globals* globals, MachineState* state)
{
	uint32_t requestedPalette = state->r3;
	if (requestedPalette == 0xffffffff)
	{
		state->r3 = globals->allocator.ToIntPtr(&globals->grafPorts.GetDefaultPalette());
	}
	else
	{
		InterfaceLib::UGrafPort& port = *globals->allocator.ToPointer<InterfaceLib::UGrafPort>(requestedPalette);
		InterfaceLib::Palette* palette = globals->grafPorts.PaletteOfGrafPort(port);
		state->r3 = globals->allocator.ToIntPtr(palette);
	}
}

void InterfaceLib_GetPaletteUpdates(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HasDepth(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitPalettes(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewPalette(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NSetPalette(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Palette2CTab(InterfaceLib::Globals* globals, MachineState* state)
{
	const InterfaceLib::Palette* palette = globals->allocator.ToPointer<InterfaceLib::Palette>(state->r3);
	const Common::UInt32& cTabPtr = *globals->allocator.ToPointer<Common::UInt32>(state->r4);
	InterfaceLib::ColorTable* cTab = globals->allocator.ToPointer<InterfaceLib::ColorTable>(cTabPtr);
	cTab->count = palette->pmEntries;
	
	for (int16_t i = 0; i < palette->pmEntries; i++)
	{
		const InterfaceLib::ColorInfo& input = palette->pmInfo[i];
		InterfaceLib::ColorSpec& output = cTab->table[i];
		output.value = i;
		output.rgb = input.ciRGB;
	}
}

void InterfaceLib_PmBackColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PmForeColor(InterfaceLib::Globals* globals, MachineState* state)
{
	// TODO support for 8-bit pixel depth
	InterfaceLib::UGrafPort& port = globals->grafPorts.GetCurrentPort();
	if (InterfaceLib::ColorTable* table = globals->grafPorts.ColorTableOfGrafPort(port))
	{
		int16_t colorIndex = static_cast<int16_t>(state->r3);
		if (colorIndex >= table->count)
		{
			std::cerr << "*** invalid color index for " << __func__ << std::endl;
			return;
		}
		
		port.color.rgbFgColor = table->table[colorIndex].rgb;
	
		CGFloat max = std::numeric_limits<uint16_t>::max();
		CGFloat r = port.color.rgbFgColor.red / max;
		CGFloat g = port.color.rgbFgColor.green / max;
		CGFloat b = port.color.rgbFgColor.blue / max;
		
		CGContextRef ctx = globals->grafPorts.ContextOfGrafPort(port);
		CGContextSetRGBFillColor(ctx, r, g, b, 1);
		CGContextSetRGBStrokeColor(ctx, r, g, b, 1);
	}
	else
	{
		std::cerr << "*** Using " << __func__ << " on a non-color port" << std::endl;
		return;
	}
}

void InterfaceLib_PMgrVersion(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RestoreBack(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RestoreDeviceClut(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RestoreFore(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SaveBack(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SaveFore(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDepth(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetEntryColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetEntryUsage(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetPalette(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetPaletteUpdates(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

