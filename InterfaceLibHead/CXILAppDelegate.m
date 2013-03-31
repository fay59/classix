//
//  CXILAppDelegate.m
//  InterfaceLibHead
//
//  Created by Félix on 2013-03-31.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#import "CXILAppDelegate.h"

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
