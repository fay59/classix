//
//  CXNavBar.m
//  Classix
//
//  Created by Félix on 2012-11-23.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "CXNavBar.h"

@interface CXNavBar (Private)

-(void)popUpMenu:(NSMenu*)menu inRect:(NSRect)rect;
-(void)menuItemSelected:(NSMenuItem*)sender;
-(void)setMenuActions:(NSMenu*)menu;

@end

static NSImage* arrow;

@implementation CXNavBar

@synthesize selectionChanged;

+(void)initialize
{
	NSString* arrowPath = [[NSBundle mainBundle] pathForResource:@"arrow" ofType:@"png"];
	arrow = [[NSImage alloc] initWithContentsOfFile:arrowPath];
}

-(NSMenu*)menu
{
	return menu;
}

-(void)setMenu:(NSMenu *)_menu
{
	NSMenu* newMenu = [_menu copy];
	[menu release];
	menu = newMenu;
	
	selectedItem = nil;
	[self setMenuActions:menu];
	[self setNeedsDisplay:YES];
}

-(void)setMenuActions:(NSMenu *)submenu
{
	for (NSMenuItem* item in submenu.itemArray)
	{
		item.onStateImage = nil;
		if (item.hasSubmenu)
		{
			item.target = nil;
			item.action = NULL;
			[self setMenuActions:item.submenu];
		}
		else
		{
			item.target = self;
			item.action = @selector(menuItemSelected:);
			
			if (selectedItem == nil)
			{
				selectedItem = item;
				if (selectionChanged != nil)
					selectionChanged(self, item);
			}
		}
	}
}

-(void)menuItemSelected:(NSMenuItem *)sender
{
	selectedItem = sender;
	[self setNeedsDisplay:YES];
	if (selectionChanged != nil)
		selectionChanged(self, sender);
}

-(BOOL)acceptsFirstResponder
{
	return YES;
}

-(void)mouseDown:(NSEvent *)theEvent
{
	NSPoint location = [self convertPoint:theEvent.locationInWindow fromView:nil];
	for (NSValue* key in hitBoxes)
	{
		NSRect hitBox;
		[key getValue:&hitBox];
		if (NSPointInRect(location, hitBox))
		{
			[self popUpMenu:[hitBoxes objectForKey:key] inRect:hitBox];
			break;
		}
	}
}

-(void)drawRect:(NSRect)dirtyRect
{
	NSGraphicsContext* nsCtx = [NSGraphicsContext currentContext];
	CGContextRef ctx = [nsCtx graphicsPort];
	
	// background
	static const CGFloat gradientLocations[] = {0, 1};
	static const CGFloat gradientComponents[] = {
		0xda / 255., 0xda / 255., 0xda / 255., 1,
		0xbc / 255., 0xbc / 255., 0xbc / 255., 1,
	};
	
	CGRect bounds = NSRectToCGRect(self.bounds);
	CGFloat bottom = CGRectGetMinY(bounds);
	CGFloat top = CGRectGetMaxY(bounds);
	CGFloat left = CGRectGetMinX(bounds);
	CGFloat right = CGRectGetMaxX(bounds);
	
	CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
	CGGradientRef gradient = CGGradientCreateWithColorComponents(rgbSpace, gradientComponents, gradientLocations, 2);
	CGContextDrawLinearGradient(ctx, gradient, CGPointMake(left, top), CGPointMake(left, bottom), 0);
	CGColorSpaceRelease(rgbSpace);
	CGGradientRelease(gradient);
	
	// border
	static const CGFloat borderColor[] = { 0x55 / 255., 0x55 / 255., 0x55 / 255., 1 };
	CGPoint lineSegments[] = {
		CGPointMake(left, top),
		CGPointMake(right, top),
		CGPointMake(left, bottom),
		CGPointMake(right, bottom),
	};
	
	CGContextSetStrokeColor(ctx, borderColor);
	CGContextStrokeLineSegments(ctx, lineSegments, 4);
	
	if (menu != nil && selectedItem != nil)
	{
		NSDictionary* textDrawingAttributes = @{NSFontAttributeName : [NSFont systemFontOfSize:11]};
		
		// menu items
		NSMutableArray* selectionHierarchy = [NSMutableArray array];
		NSMenuItem* item = selectedItem;
		do
		{
			[selectionHierarchy insertObject:item atIndex:0];
			item = item.parentItem;
		} while (item != nil);
		
		NSMutableDictionary* mutableHitBoxes = [NSMutableDictionary dictionaryWithCapacity:selectionHierarchy.count];
		NSUInteger i = 0;
		CGFloat outputLocation = bounds.origin.x + 10;
		const NSSize arrowSize = arrow.size;
		const CGFloat height = bounds.size.height - 8;
		const NSRect arrowBaseFrame = NSMakeRect(0, bounds.origin.y + 4, height * arrowSize.width / arrowSize.height, height);
		const NSRect arrowBounds = NSMakeRect(0, 0, arrowSize.width, arrowSize.height);
		for (NSMenuItem* item in selectionHierarchy)
		{
			if (i != 0)
			{
				outputLocation += 3;
				NSRect arrowFrame = arrowBaseFrame;
				arrowFrame.origin.x = outputLocation;
				[arrow drawInRect:arrowFrame fromRect:arrowBounds operation:NSCompositeSourceAtop fraction:1];
				outputLocation += arrowFrame.size.width + 7;
			}
			
			NSRect hitBox = NSMakeRect(outputLocation, 0, 0, bounds.size.height);
			NSImage* icon = item.image;
			if (icon != nil)
			{
				NSSize iconSize = icon.size;
				NSRect iconBounds = NSMakeRect(0, 0, iconSize.width, iconSize.height);
				NSRect iconFrame = NSMakeRect(outputLocation, bounds.origin.y + 4, height * iconSize.height / iconSize.width, height);
				[icon drawInRect:iconFrame fromRect:iconBounds operation:NSCompositeSourceAtop fraction:1];
				outputLocation += iconFrame.size.width + 5;
			}
			
			NSString* label = item.title;
			NSSize stringSize = [item.title sizeWithAttributes:textDrawingAttributes];
			NSPoint location = NSMakePoint(outputLocation, bounds.origin.y + (bounds.size.height - stringSize.height) / 2);
			[label drawAtPoint:location withAttributes:textDrawingAttributes];
			outputLocation += stringSize.width + 3;
			
			hitBox.size.width = outputLocation - hitBox.origin.x;
			[mutableHitBoxes setObject:item.menu forKey:[NSValue valueWithRect:hitBox]];
			
			i++;
		}
		[hitBoxes release];
		hitBoxes = [mutableHitBoxes copy];
	}
}

-(void)dealloc
{
	self.menu = nil;
	self.selectionChanged = nil;
	[super dealloc];
}

#pragma mark -
#pragma mark Private

-(void)popUpMenu:(NSMenu*)menuToPop inRect:(NSRect)rect
{
	// location changes to align the menu on the bar
	// (this is right as of Mountain Lion on a non-Retina screen)
	rect.origin.x -= 11;
	rect.origin.y -= 1;
	
	NSPopUpButtonCell *popUpButtonCell = [[[NSPopUpButtonCell alloc] initTextCell:@"" pullsDown:NO] autorelease];
	[popUpButtonCell setMenu:menuToPop];
	[popUpButtonCell performClickWithFrame:rect inView:self];
}

@end
