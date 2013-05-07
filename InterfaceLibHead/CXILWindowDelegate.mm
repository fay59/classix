//
// CXILWindowDelegate.cpp
// Classix
//
// Copyright (C) 2012 Félix Cloutier
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

#import "CXILWindowDelegate.h"
#import "CXILApplication.h"
#import "CXIOSurfaceView.h"
#include "CommonDefinitions.h"

@interface CXILDragWindowEventHandler : NSObject <CXILEventHandler>

-(id)initWithWindow:(NSWindow*)window mouseLocation:(NSPoint)location dragBounds:(NSRect)dragBounds;

@end

@interface CXILWindowDelegate (Private)

-(NSWindow*)windowForID:(uint32_t)windowID;

@end

@implementation CXILWindowDelegate
{
	NSMutableDictionary* windows;
	NSWindow* menuGate;
}

-(id)initWithMenuGate:(NSWindow *)window
{
	if (!(self = [super init]))
		return nil;
	
	windows = [NSMutableDictionary dictionary];
	menuGate = window;
	
	return self;
}

-(void)createWindow:(uint32_t)key withRect:(NSRect)rect surface:(IOSurfaceRef)surface title:(NSString *)title visible:(BOOL)visible behind:(uint32_t)behindKey
{
	NSUInteger windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
	NSWindow* window = [[NSWindow alloc] initWithContentRect:rect styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO];
	window.title = title;
	window.oneShot = YES;
	window.movable = NO;
	
	[windows setObject:window forKey:@(key)];
	
	if (visible)
	{
		if (behindKey == 0xffffffff)
		{
			[window makeKeyAndOrderFront:self];
		}
		else
		{
			if (NSWindow* above = [self windowForID:behindKey])
			{
				[window orderWindow:NSWindowBelow relativeTo:above.orderedIndex];
				[window makeKeyWindow];
			}
			else
			{
				NSLog(@"*** trying to place window above dead reference; ordering front");
				[window makeKeyAndOrderFront:self];
			}
		}
	}
	
	NSView* contentView = window.contentView;
	CXIOSurfaceView* rootView = [[CXIOSurfaceView alloc] initWithFrame:contentView.frame surface:surface surfaceBounds:rect];
	window.contentView = rootView;
}

-(void)setDirtyRect:(CGRect)rect inWindow:(uint32_t)key
{
	NSWindow* window = windows[@(key)];
	[[window contentView] setNeedsDisplayInRect:rect];
}

-(void)destroyWindow:(uint32_t)windowID
{
	[windows removeObjectForKey:@(windowID)];
}

-(id<CXILEventHandler>)startDragWindow:(uint32_t)windowKey mouseLocation:(NSPoint)location dragBounds:(NSRect)bounds
{
	NSWindow* window = windows[@(windowKey)];
	return [[CXILDragWindowEventHandler alloc] initWithWindow:window mouseLocation:location dragBounds:bounds];
}

-(uint32_t)keyOfFrontWindow
{
	uint32_t windowKey = 0;
	NSInteger bestLevel = NSIntegerMin;
	for (NSNumber* key : windows)
	{
		NSWindow* window = windows[key];
		NSInteger level = window.level;
		if (level > bestLevel)
		{
			CFNumberGetValue((CFNumberRef)key, kCFNumberSInt32Type, &windowKey);
			bestLevel = level;
		}
	}
	return windowKey;
}

-(uint32_t)keyOfWindow:(NSWindow *)window
{
	return [[windows allKeysForObject:window][0] unsignedIntValue];
}

-(uint32_t)findWindowUnderPoint:(NSPoint)point area:(int16_t *)partCode
{
	if (point.y >= menuGate.frame.origin.y)
	{
		*partCode = static_cast<int16_t>(InterfaceLib::WindowPartCode::inMenuBar);
		return 0;
	}
	
	NSInteger windowNumber = [NSWindow windowNumberAtPoint:point belowWindowWithWindowNumber:0];
	
	for (NSWindow* window in windows.allValues)
	{
		if (window.windowNumber == windowNumber)
		{
			NSRect viewFrame = [window.contentView frame];
			viewFrame.origin = window.frame.origin;
			*partCode = NSPointInRect(point, viewFrame)
				? static_cast<int16_t>(InterfaceLib::WindowPartCode::inContent)
				: static_cast<int16_t>(InterfaceLib::WindowPartCode::inDrag);
			return [self keyOfWindow:window];
		}
	}
	
	*partCode = -1;
	return 0;
}

@end

@implementation CXILDragWindowEventHandler
{
	NSWindow* draggedWindow;
	NSRect dragBounds;
	NSPoint cursorDelta;
	NSMutableArray* removalActions;
}

-(id)initWithWindow:(NSWindow *)window mouseLocation:(NSPoint)location dragBounds:(NSRect)bounds
{
	if (!(self = [super init]))
		return nil;
	
	draggedWindow = window;
	dragBounds = bounds;
	
	NSPoint windowOrigin = window.frame.origin;
	cursorDelta = NSMakePoint(location.x - windowOrigin.x, location.y - windowOrigin.y);
	
	removalActions = [NSMutableArray array];
	
	return self;
}

-(CXILEventHandlerActionResult)handleEvent:(NSEvent *)event
{
	if (event.type == NSLeftMouseDragged)
	{
		NSPoint location = NSEvent.mouseLocation;
		NSSize windowSize = draggedWindow.frame.size;
		NSRect newWindowRect = NSMakeRect(location.x - cursorDelta.x, location.y - cursorDelta.y, windowSize.width, windowSize.height);
		
		if (NSMinX(newWindowRect) < NSMinX(dragBounds))
			newWindowRect.origin.x = NSMinX(dragBounds);
		if (NSMinY(newWindowRect) < NSMinY(dragBounds))
			newWindowRect.origin.y = NSMinY(dragBounds);
		
		if (NSMaxX(newWindowRect) > NSMaxX(dragBounds))
			newWindowRect.origin.x = NSMaxX(dragBounds) - newWindowRect.size.width;
		if (NSMaxY(newWindowRect) > NSMaxY(dragBounds))
			newWindowRect.origin.y = NSMaxY(dragBounds) - newWindowRect.size.height;
		
		[draggedWindow setFrame:newWindowRect display:NO];
	}
	else if (event.type == NSLeftMouseUp)
	{
		for (CXILEventHandlerRemovedAction action : removalActions)
			action(self);
		
		return kCXILEventHandlerRemoveHandler;
	}
	
	return kCXILEventHandlerNormalResolution;
}

-(CXILEventHandlerActionResult)handleNotification:(NSNotification *)notification
{
	return kCXILEventHandlerNormalResolution;
}

-(void)registerRemovalAction:(CXILEventHandlerRemovedAction)action
{
	[removalActions addObject:action];
}

@end
