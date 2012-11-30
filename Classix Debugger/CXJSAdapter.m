//
//  CXJSAdapter.m
//  Classix
//
//  Created by Félix on 2012-11-30.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "CXJSAdapter.h"
#import "CXDocument.h"

@implementation CXJSAdapter

-(id)initWithDocument:(CXDocument *)aDocument
{
	if (!(self = [super init]))
		return nil;
	
	document = aDocument;
	breakpoints = [[NSMutableSet alloc] init];
	
	return self;
}

-(void)setPC:(uint32_t)pc
{
	document.pc = pc;
}

-(NSArray*)breakpoints
{
	return breakpoints.allObjects;
}

-(BOOL)toggleBreakpoint:(uint32_t)address
{
	NSNumber* number = @(address);
	if ([breakpoints containsObject:number])
	{
		[breakpoints removeObject:number];
		return NO;
	}
	else
	{
		[breakpoints addObject:number];
		return YES;
	}
}

-(void)dealloc
{
	[breakpoints release];
	[super dealloc];
}

@end
