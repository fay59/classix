//
// CXIOSurfaceView.m
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

#import "CXIOSurfaceView.h"

static CGFloat titleBarHeight;

@implementation CXIOSurfaceView
{
	IOSurfaceRef surface;
	CGContextRef surfaceContext;
	
	// FPS counter
	time_t start;
	int frameCounter;
}

+(void)initialize
{
	CGRect contentRect = CGRectMake(0, 0, 10, 10);
	CGRect frameRect = [NSWindow frameRectForContentRect:contentRect styleMask:NSTitledWindowMask];
	titleBarHeight = frameRect.size.height - contentRect.size.height;
}

-(id)initWithFrame:(NSRect)frameRect
{
	return nil;
}

-(id)initWithFrame:(NSRect)frame surface:(IOSurfaceRef)aSurface surfaceBounds:(CGRect)bounds
{
	if (!(self = [super initWithFrame:frame]))
	{
		return nil;
	}
	
	surface = aSurface;
	IOSurfaceIncrementUseCount(surface);
	
	void* buffer = IOSurfaceGetBaseAddress(surface);
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	surfaceContext = CGBitmapContextCreate(buffer, bounds.size.width, bounds.size.height, 8, bounds.size.width * 4, rgb, kCGImageAlphaNoneSkipFirst);
	CGColorSpaceRelease(rgb);
	
	return self;
}

-(BOOL)wantsDefaultClipping { return NO; }

-(void)drawRect:(NSRect)dirtyRect
{
	time_t now = time(NULL);
	if (start == 0)
		start = now;
	
	if (start != now)
	{
		NSLog(@"%i fps", frameCounter);
		frameCounter = 0;
		start = now;
	}
	frameCounter++;
	
	CGImageRef image = CGBitmapContextCreateImage(surfaceContext);
	// wtf??
	// In the "CGImage coordinate system", pixel (0,0) is the top left pixel
	// while in CGContextRefs (0,0) is the bottom left pixel
	CGRect imageDirtyRegion = dirtyRect;
	imageDirtyRegion.origin.y = CGImageGetHeight(image) - imageDirtyRegion.origin.y - imageDirtyRegion.size.height;
	CGImageRef smaller = CGImageCreateWithImageInRect(image, imageDirtyRegion);
	CGImageRelease(image);
	
	CGContextRef currentContext = NSGraphicsContext.currentContext.graphicsPort;
	CGContextDrawImage(currentContext, dirtyRect, smaller);
	CGImageRelease(smaller);
}

-(void)dealloc
{
	CGContextRelease(surfaceContext);
	IOSurfaceDecrementUseCount(surface);
}

@end
