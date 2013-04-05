//
// CXILApplication.m
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

#import "CXILApplication.h"
#include <dispatch/dispatch.h>

static void die(NSString* reason)
{
	NSLog(@"InterfaceLibHead is not meant to be run directly. Please let InterfaceLib launch it.");
	NSLog(@"%@", reason);
	abort();
}

static inline BOOL isFDValid(int fd)
{
	return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

@implementation CXILApplication
{
	int writeHandle;
	int readHandle;
	dispatch_source_t ipcSource;
}

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	NSArray* arguments = NSProcessInfo.processInfo.arguments;
	if (arguments.count != 3)
		die(@"Bad arguments passed to main().");
	
	readHandle = [arguments[1] intValue];
	writeHandle = [arguments[2] intValue];
	
	if (readHandle == 0 || writeHandle == 0 || !isFDValid(readHandle) || !isFDValid(writeHandle))
		die(@"Either the read or write pipe is invalid.");
	
	ipcSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ, readHandle, 0, dispatch_get_main_queue());
	dispatch_source_set_event_handler(ipcSource, ^{ [self processIPCMessage]; });
	dispatch_resume(ipcSource);
	
	return self;
}

-(void)processIPCMessage
{
	uint32_t messageType;
	size_t count = read(readHandle, &messageType, sizeof messageType);
	if (count < sizeof messageType)
	{
		// broken pipe
		[NSApp terminate:self];
	}
}

-(void)sendEvent:(NSEvent *)theEvent
{
	[super sendEvent:theEvent];
}

-(void)dealloc
{
	dispatch_suspend(ipcSource);
}

@end
