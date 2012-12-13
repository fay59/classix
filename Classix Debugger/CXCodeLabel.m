//
// CXCodeLabel.m
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

#import "CXCodeLabel.h"

@implementation CXCodeLabel

@synthesize label;
@synthesize uniqueName;
@synthesize address;
@synthesize length;
@synthesize instructions;
@synthesize isFunction;

+(id)codeLabelWithAddress:(uint32_t)address length:(uint32_t)length uniqueName:(NSString *)uniqueName
{
	return [[[self alloc] initWithAddress:address length:length uniqueName:uniqueName] autorelease];
}

-(id)initWithAddress:(uint32_t)aAddress length:(uint32_t)aLength uniqueName:(NSString *)aUniqueName
{
	if (!(self = [super init]))
		return nil;
	
	address = aAddress;
	length = aLength;
	uniqueName = [aUniqueName copy];
	self.label = [NSString stringWithFormat:@".%08x", aAddress];
	
	return self;
}

-(void)dealloc
{
	[uniqueName release];
	self.label = nil;
	self.instructions = nil;
	[super dealloc];
}

@end
