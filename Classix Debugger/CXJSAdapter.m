//
// CXJSAdapter.m
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
