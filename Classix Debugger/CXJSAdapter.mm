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
#import "CXRegister.h"
#import "CXJSONEncode.h"

@implementation CXJSAdapter

+(BOOL)isSelectorExcludedFromWebScript:(SEL)selector
{
	SEL validSelectors[] = {
		@selector(setPC:),
		@selector(breakpoints),
		@selector(toggleBreakpoint:),
		@selector(representationsOfGPR:),
		@selector(representationsOfFPR:),
		@selector(representationsOfSPR:),
		@selector(representationsOfCR:),
		@selector(representationsOfMemoryAddress:),
		@selector(jsonize:)
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

-(NSDictionary*)representationsOfGPR:(uint32_t)gprNumber
{
	CXRegister* gpr = [document.vm.gpr objectAtIndex:gprNumber];
	NSMutableDictionary* representations = [NSMutableDictionary dictionary];
	
	uint32_t unsignedValue = gpr.value.unsignedIntValue;
	[representations setObject:[NSString stringWithFormat:@"0x%08x", unsignedValue] forKey:@"unsigned"];
	[representations setObject:@((int32_t)unsignedValue) forKey:@"signed"];
	[representations addEntriesFromDictionary:[self representationsOfMemoryAddress:unsignedValue]];
	
	return representations;
}

-(NSDictionary*)representationsOfMemoryAddress:(uint32_t)memoryAddress
{
	NSMutableDictionary* representations = [NSMutableDictionary dictionary];
	if (NSString* explanation = [document.vm explainAddress:memoryAddress])
	{
		[representations setObject:explanation forKey:@"pointer"];
		if (NSNumber* memoryValue = [document.vm getWordAtAddress:memoryAddress])
		{
			uint32_t unsignedValue = memoryValue.unsignedIntValue;
			[representations setObject:[NSString stringWithFormat:@"0x%08x", unsignedValue] forKey:@"*unsigned"];
			[representations setObject:@((int32_t)unsignedValue) forKey:@"*signed"];
		}
		
		if (NSNumber* memoryValue = [document.vm getFloatAtAddress:memoryAddress])
			[representations setObject:memoryValue forKey:@"*float"];
		
		if (NSNumber* memoryValue = [document.vm getDoubleAtAddress:memoryAddress])
			[representations setObject:memoryValue forKey:@"*double"];
	}
	else
	{
		[representations setObject:@"<access violation>" forKey:@"pointer"];
	}
	
	return representations;
}

-(NSString*)jsonize:(id<NSObject>)object
{
	return CXJSONEncode(object);
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
