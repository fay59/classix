//
// CXILAppDelegate.m
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

#import "CXILAppDelegate.h"
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

@implementation CXILAppDelegate
{
	int writeHandle;
	int readHandle;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	NSArray* arguments = NSProcessInfo.processInfo.arguments;
	if (arguments.count != 3)
		die(@"Bad arguments passed to main().");
	
	readHandle = [arguments[1] intValue];
	writeHandle = [arguments[2] intValue];
	
	if (!isFDValid(readHandle) || !isFDValid(writeHandle))
		die(@"Either the read or write pipe is invalid.");
}

@end
