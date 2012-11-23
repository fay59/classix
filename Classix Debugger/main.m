//
//  main.m
//  Classix Debugger
//
//  Created by Félix on 2012-11-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CXDBURLProtocol.h"
#import "CXDocumentController.h"

int main(int argc, char *argv[])
{
	[CXDBURLProtocol class];
	[[CXDocumentController alloc] init];
	return NSApplicationMain(argc, (const char **)argv);
}
