//
// Quickdraw.cpp
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

#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <iostream>
#include <limits>

#include "Prototypes.h"
#include "InterfaceLib.h"
#include "NotImplementedException.h"
#include "CFOwningRef.h"

using namespace InterfaceLib;

namespace
{
	CGRect RectToCGRect(const InterfaceLib::Rect& rect, CGContextRef context)
	{
		CGFloat height = rect.bottom - rect.top;
		return CGRectMake(rect.left, CGBitmapContextGetHeight(context) - rect.top - height, rect.right - rect.left, height);
	}
	
	CGPoint PointToCGPoint(const InterfaceLib::Point& point, CGContextRef context)
	{
		return CGPointMake(point.h, CGBitmapContextGetHeight(context) - point.v);
	}
}

void InterfaceLib_AddComp(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_AddPt(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_AddSearch(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_AllocCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_AngleFromSlope(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_BackColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_BackPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_BackPixPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_BitMapToRegion(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CalcCMask(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CalcMask(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ClipRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CloseCPort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CloseCursorComponent(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ClosePicture(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ClosePoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ClosePort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CloseRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Color2Index(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ColorBit(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CopyBits(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CopyDeepMask(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CopyMask(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CopyPixMap(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CopyPixPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CursorComponentChanged(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_CursorComponentSetData(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DelComp(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DelSearch(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DeltaPoint(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DeviceLoop(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DiffRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposeCCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposeCTable(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposeGDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposePixMap(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposePixPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DisposeRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_DrawPicture(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EmptyRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EmptyRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EqualPt(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EraseArc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EraseOval(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ErasePoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EraseRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EraseRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_EraseRoundRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillArc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillCArc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillCOval(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillCPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillCRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillCRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillCRoundRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillOval(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FillRoundRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ForeColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FrameArc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FrameOval(InterfaceLib::Globals* globals, MachineState* state)
{
	UGrafPort& port = globals->grafPorts.GetCurrentPort();
	CGContextRef ctx = globals->grafPorts.ContextOfGrafPort(port);
	const InterfaceLib::Rect* rect = globals->allocator.ToPointer<InterfaceLib::Rect>(state->r3);
	CGRect cgRect = RectToCGRect(*rect, ctx);
	
	CGContextStrokeEllipseInRect(ctx, cgRect);
	
	uint32_t key = globals->allocator.ToIntPtr(&port);
	if (globals->grafPorts.UpdateRegion(port, cgRect) == false)
	{
		globals->ipc().PerformAction<void>(IPCMessage::SetDirtyRect, key, cgRect);
	}
}

void InterfaceLib_FramePoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_FrameRoundRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetBackColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetCCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetClip(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetCPixel(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetCTable(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetCTSeed(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetDeviceList(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetForeColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetGDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetIndPattern(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetMainDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetMaskTable(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetMaxDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetNextDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPattern(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPen(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPenState(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPicture(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPixPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetPortCustomXFerProc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GetSubTable(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GlobalToLocal(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_GrafDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HideCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	globals->ipc().PerformAction<void>(IPCMessage::SetCursorVisibility, false);
}

void InterfaceLib_HidePen(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_HiliteColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Index2Color(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitCPort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	// do nothing
}

void InterfaceLib_InitGDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InitGraf(InterfaceLib::Globals* globals, MachineState* state)
{
	CFOwningRef<CGDisplayModeRef> displayMode = CGDisplayCopyDisplayMode(CGMainDisplayID());
	int16_t width = static_cast<int16_t>(CGDisplayModeGetWidth(displayMode));
	int16_t height = static_cast<int16_t>(CGDisplayModeGetHeight(displayMode));
	
	InterfaceLib::Rect screenRect;
	screenRect.left = 0;
	screenRect.right = width;
	screenRect.top = 0;
	screenRect.bottom = height;
	InterfaceLib::UGrafPort& port = globals->grafPorts.AllocateColorGrafPort(screenRect, nullptr, "QD Screen Port");
	uint32_t grafPtr = globals->allocator.ToIntPtr(&port);
	
	// initialize qd while we're at it
	QDGlobals* qd = globals->allocator.ToPointer<QDGlobals>(state->r3 - sizeof(QDGlobals) + sizeof(UInt32));
	qd->screenBits.bounds = screenRect;
	qd->thePort = grafPtr;
	// TODO patterns and cursor
}

void InterfaceLib_InitPort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InsetRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InvertArc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InvertColor(InterfaceLib::Globals* globals, MachineState* state)
{
	InterfaceLib::RGBColor& color = *globals->allocator.ToPointer<InterfaceLib::RGBColor>(state->r3);
	color.red = std::numeric_limits<uint16_t>::max() - color.red;
	color.green = std::numeric_limits<uint16_t>::max() - color.green;
	color.blue = std::numeric_limits<uint16_t>::max() - color.blue;
}

void InterfaceLib_InvertOval(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InvertPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_InvertRoundRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_KillPicture(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_KillPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Line(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetCursorNew(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetDeviceList(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetFractEnable(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetHiliteMode(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetHiliteRGB(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetLastFOND(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetLastSPExtra(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetMainDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetQDColors(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetScrHRes(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetScrVRes(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetTheGDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetWidthListHand(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetWidthPtr(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMGetWidthTabHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetCursorNew(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetDeviceList(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetFractEnable(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetHiliteMode(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetHiliteRGB(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetLastFOND(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetLastSPExtra(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetMainDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetQDColors(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetScrHRes(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetScrVRes(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetTheGDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetWidthListHand(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetWidthPtr(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LMSetWidthTabHandle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_LocalToGlobal(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MakeITable(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MakeRGBPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MapPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MapPt(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MapRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MapRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Move(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MovePortTo(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_MoveTo(InterfaceLib::Globals* globals, MachineState* state)
{
	UGrafPort& port = globals->grafPorts.GetCurrentPort();
	port.color.pnLoc.h = static_cast<int16_t>(state->r3);
	port.color.pnLoc.v = static_cast<int16_t>(state->r4);
	
	CGPoint point = PointToCGPoint(port.color.pnLoc, globals->grafPorts.ContextOfGrafPort(port));
	CGContextMoveToPoint(globals->grafPorts.ContextOfGrafPort(port), point.x, point.y);
}

void InterfaceLib_NewGDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewPixMap(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewPixPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_NewRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ObscureCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OffsetPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OpColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OpenCPicture(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OpenCPort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OpenCursorComponent(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OpenPicture(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OpenPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OpenPort(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_OpenRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PackBits(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PaintArc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PaintOval(InterfaceLib::Globals* globals, MachineState* state)
{
	UGrafPort& port = globals->grafPorts.GetCurrentPort();
	CGContextRef ctx = globals->grafPorts.ContextOfGrafPort(port);
	const InterfaceLib::Rect* rect = globals->allocator.ToPointer<InterfaceLib::Rect>(state->r3);
	CGRect cgRect = RectToCGRect(*rect, ctx);
	
	CGContextFillEllipseInRect(ctx, cgRect);
	
	uint32_t key = globals->allocator.ToIntPtr(&port);
	if (globals->grafPorts.UpdateRegion(port, cgRect) == false)
	{
		globals->ipc().PerformAction<void>(IPCMessage::SetDirtyRect, key, cgRect);
	}
}

void InterfaceLib_PaintPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PaintRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PaintRoundRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PenMode(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PenNormal(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PenPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PenPixPat(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PenSize(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PicComment(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PortSize(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ProtectEntry(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Pt2Rect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PtInRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_PtToAngle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_QDError(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_Random(InterfaceLib::Globals* globals, MachineState* state)
{
	// FIXME cheap implementation that doesn't use the value of randSeed
	int16_t number = static_cast<int16_t>(random() & 0xffff);
	state->r3 = number;
}

void InterfaceLib_RealColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RectInRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RectRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ReserveEntry(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RestoreEntries(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RGBBackColor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_RGBForeColor(InterfaceLib::Globals* globals, MachineState* state)
{
	InterfaceLib::UGrafPort& port = globals->grafPorts.GetCurrentPort();
	if (!port.IsColor())
	{
		std::cerr << "*** Using RGBForeColor on a non-color port" << std::endl;
		return;
	}
	
	port.color.rgbFgColor = *globals->allocator.ToPointer<InterfaceLib::RGBColor>(state->r3);
	
	CGFloat max = std::numeric_limits<uint16_t>::max();
	CGFloat r = port.color.rgbFgColor.red / max;
	CGFloat g = port.color.rgbFgColor.green / max;
	CGFloat b = port.color.rgbFgColor.blue / max;
	
	CGContextRef ctx = globals->grafPorts.ContextOfGrafPort(port);
	CGContextSetRGBFillColor(ctx, r, g, b, 1);
	CGContextSetRGBStrokeColor(ctx, r, g, b, 1);
}

void InterfaceLib_SaveEntries(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ScalePt(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ScreenRes(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ScrollRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SectRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SectRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SeedCFill(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SeedFill(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetCCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetClientID(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetClip(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetCPixel(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetCursorComponent(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetDeviceAttribute(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetEmptyRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetEntries(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetGDevice(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetOrigin(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetPenState(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetPortBits(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetPortCustomXFerProc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetPortPix(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetPt(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetStdCProcs(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SetStdProcs(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ShieldCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_ShowCursor(InterfaceLib::Globals* globals, MachineState* state)
{
	globals->ipc().PerformAction<void>(IPCMessage::SetCursorVisibility, true);
}

void InterfaceLib_ShowPen(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SlopeFromAngle(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdArc(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdBits(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdComment(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdGetPic(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdLine(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdOval(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdPoly(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdPutPic(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdRgn(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StdRRect(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_StuffHex(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_SubPt(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_TestDeviceAttribute(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

void InterfaceLib_UnpackBits(InterfaceLib::Globals* globals, MachineState* state)
{
	throw PPCVM::NotImplementedException(__func__);
}

