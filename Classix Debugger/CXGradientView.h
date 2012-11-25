//
//  CXGradientView.h
//  Classix
//
//  Created by Félix on 2012-11-25.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface CXGradientView : NSView
{
	NSColor* keyLightColor;
	NSColor* keyDarkColor;
	NSColor* nonkeyLightColor;
	NSColor* nonkeyDarkColor;
	NSColor* borderColor;
	bool isWindowKey;
}

@property (retain) NSColor* keyLightColor;
@property (retain) NSColor* keyDarkColor;
@property (retain) NSColor* nonkeyLightColor;
@property (retain) NSColor* nonkeyDarkColor;
@property (retain) NSColor* borderColor;

@end
