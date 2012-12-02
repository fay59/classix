//
// CXGradientView.m
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

#import "CXGradientView.h"

@interface CXGradientView (Private)

-(void)doInit;
-(void)setIsWindowKey:(BOOL)isKey;
-(void)notificationReceived:(NSNotification*)notification;

@end

@implementation CXGradientView

static void NSColorGetRGBAComponents(NSColor* color, CGFloat* components)
{
	NSColor* deviceColor = [color colorUsingColorSpaceName:NSDeviceRGBColorSpace];
	[deviceColor getRed:&components[0] green:&components[1] blue:&components[2] alpha:&components[3]];
}

@synthesize keyDarkColor;
@synthesize keyLightColor;
@synthesize nonkeyDarkColor;
@synthesize nonkeyLightColor;
@synthesize borderColor;

-(id)initWithFrame:(NSRect)frameRect
{
	if (!(self = [super initWithFrame:frameRect]))
		return nil;
	
	[self doInit];
	
	return self;
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
	if (!(self = [super initWithCoder:aDecoder]))
		return nil;
	
	[self doInit];
	
	return self;
}

-(void)drawRect:(NSRect)dirtyRect
{
	CGContextRef ctx = NSGraphicsContext.currentContext.graphicsPort;
	const NSRect bounds = self.bounds;
	const CGFloat bottom = NSMinY(bounds);
	const CGFloat top = NSMaxY(bounds);
	const CGFloat left = NSMinX(bounds);
	const CGFloat right = NSMaxX(bounds);
	
	CGFloat stops[2] = {0, 1};
	CGFloat components[8] = {0};
	if (isWindowKey)
	{
		NSColorGetRGBAComponents(keyLightColor, components);
		NSColorGetRGBAComponents(keyDarkColor, components + 4);
	}
	else
	{
		NSColorGetRGBAComponents(nonkeyLightColor, components);
		NSColorGetRGBAComponents(nonkeyDarkColor, components + 4);
	}
	
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	CGGradientRef gradient = CGGradientCreateWithColorComponents(rgb, components, stops, 2);
	CGContextDrawLinearGradient(ctx, gradient, CGPointMake(left, top), CGPointMake(left, bottom), 0);
	CGGradientRelease(gradient);
	CGColorSpaceRelease(rgb);
	
	CGFloat borderColorComponents[4];
	NSColorGetRGBAComponents(borderColor, borderColorComponents);
	
	CGPoint lineSegments[] = {
		CGPointMake(left, top),
		CGPointMake(right, top),
		CGPointMake(left, bottom),
		CGPointMake(right, bottom),
	};
	
	CGContextSetStrokeColor(ctx, borderColorComponents);
	CGContextStrokeLineSegments(ctx, lineSegments, 4);
}

-(void)viewDidMoveToWindow
{
	isWindowKey = self.window.isKeyWindow;
}

-(void)dealloc
{
	[NSNotificationCenter.defaultCenter removeObserver:self name:NSWindowDidBecomeKeyNotification object:nil];
	
	self.keyDarkColor = nil;
	self.keyLightColor = nil;
	self.nonkeyDarkColor = nil;
	self.nonkeyLightColor = nil;
	[super dealloc];
}

#pragma mark -
#pragma mark Private

-(void)doInit
{
	CGFloat keyDark = 0xbc / 255.;
	CGFloat keyLight = 0xda / 255.;
	CGFloat nonkeyDark = 0xd9 / 255.;
	CGFloat nonkeyLight = 0xf6 / 255.;
	CGFloat border = 0x80 / 255.;
	
	self.keyDarkColor = [NSColor colorWithCalibratedRed:keyDark green:keyDark blue:keyDark alpha:1];
	self.keyLightColor = [NSColor colorWithCalibratedRed:keyLight green:keyLight blue:keyLight alpha:1];
	self.nonkeyDarkColor = [NSColor colorWithCalibratedRed:nonkeyDark green:nonkeyDark blue:nonkeyDark alpha:1];
	self.nonkeyLightColor = [NSColor colorWithCalibratedRed:nonkeyLight green:nonkeyLight blue:nonkeyLight alpha:1];
	self.borderColor = [NSColor colorWithCalibratedRed:border green:border blue:border alpha:1];
	
	[NSNotificationCenter.defaultCenter addObserver:self selector:@selector(notificationReceived:) name:NSWindowDidBecomeKeyNotification object:nil];
}

-(void)setIsWindowKey:(BOOL)isKey
{
	if (isKey != isWindowKey)
	{
		isWindowKey = isKey;
		[self setNeedsDisplay:YES];
	}
}

-(void)notificationReceived:(NSNotification *)notification
{
	NSWindow* sender = notification.object;
	isWindowKey = sender == self.window;
}

@end
