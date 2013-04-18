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

@interface CXILWindowDelegate (Private)

-(NSWindow*)windowForID:(uint32_t)windowID;

@end

@implementation CXILWindowDelegate
{
	NSMutableDictionary* windows;
}

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	windows = [NSMutableDictionary dictionary];
	
	return self;
}

-(void)createWindow:(uint32_t)key withRect:(NSRect)rect surface:(IOSurfaceRef)surface title:(NSString *)title visible:(BOOL)visible behind:(uint32_t)behindKey
{
	NSUInteger windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
	NSWindow* window = [[NSWindow alloc] initWithContentRect:rect styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO];
	window.title = title;
	window.oneShot = YES;
	
	[windows setObject:window forKey:@(key)];
	
	if (visible)
	{
		if (behindKey == -1)
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
	NSWindow* window = [windows objectForKey:@(key)];
	[[window contentView] setNeedsDisplayInRect:rect];
}

-(void)destroyWindow:(uint32_t)windowID
{
	[windows removeObjectForKey:@(windowID)];
}

-(uint32_t)keyOfWindow:(NSWindow *)window
{
	return [[windows allKeysForObject:window][0] unsignedIntValue];
}

@end
