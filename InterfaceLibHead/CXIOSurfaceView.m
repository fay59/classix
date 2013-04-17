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

@implementation CXIOSurfaceView
{
	IOSurfaceRef surface;
	CGContextRef surfaceContext;
}

-(id)initWithFrame:(NSRect)frameRect
{
	return nil;
}

-(id)initWithFrame:(NSRect)frame surface:(IOSurfaceRef)aSurface
{
	if (!(self = [super initWithFrame:frame]))
	{
		return nil;
	}
	
	surface = aSurface;
	IOSurfaceIncrementUseCount(surface);
	
	void* buffer = IOSurfaceGetBaseAddress(surface);
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	surfaceContext = CGBitmapContextCreate(buffer, frame.size.width, frame.size.height, 8, frame.size.width * 4, rgb, kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(rgb);
	
	return self;
}

-(void)drawRect:(NSRect)dirtyRect
{
	CGImageRef image = CGBitmapContextCreateImage(surfaceContext);
	CGContextRef currentContext = NSGraphicsContext.currentContext.graphicsPort;
	CGContextDrawImage(currentContext, self.bounds, image);
	CGImageRelease(image);
}

-(void)dealloc
{
	CGContextRelease(surfaceContext);
	IOSurfaceDecrementUseCount(surface);
}

@end
