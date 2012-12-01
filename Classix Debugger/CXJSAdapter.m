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

+(BOOL)isSelectorExcludedFromWebScript:(SEL)selector
{
	SEL validSelectors[] = {
		@selector(setPC:),
		@selector(breakpoints),
		@selector(toggleBreakpoint:)
	};
	
	for (int i = 0; i < sizeof validSelectors / sizeof(SEL); i++)
	{
		if (validSelectors[i] == selector)
			return NO;
	}
	return YES;
}

-(id)initWithDocument:(CXDocument *)aDocument
{
	if (!(self = [super init]))
		return nil;
	
	document = aDocument;
	
	return self;
}

-(void)setPC:(uint32_t)pc
{
	document.vm.pc = pc;
}

-(NSArray*)breakpoints
{
	return document.vm.breakpoints.allObjects;
}

-(BOOL)toggleBreakpoint:(uint32_t)address
{
	NSMutableSet* breakpoints = document.vm.breakpoints;
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
	[super dealloc];
}

@end
