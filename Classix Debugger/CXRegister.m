//
// CXRegister.m
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

#import "CXRegister.h"

@implementation CXRegister

@synthesize name;
@synthesize type;

+(id)GPRNumber:(int)number location:(uint32_t *)location
{
	NSString* name = [NSString stringWithFormat:@"r%i", number];
	return [[[self alloc] initWithName:name address:location type:CXRegisterGPR] autorelease];
}

+(id)FPRNumber:(int)number location:(double*)location
{
	NSString* name = [NSString stringWithFormat:@"fr%i", number];
	return [[[self alloc] initWithName:name address:location type:CXRegisterFPR] autorelease];
}

+(id)CRNumber:(int)number location:(uint8_t*)location
{
	NSString* name = [NSString stringWithFormat:@"cr%i", number];
	return [[[self alloc] initWithName:name address:location type:CXRegisterCR] autorelease];
}

+(id)SPRName:(NSString*)name location:(uint32_t*)location
{
	return [[[self alloc] initWithName:name address:location type:CXRegisterSPR] autorelease];
}

-(id)initWithName:(NSString *)aName address:(void *)aAddress type:(CXRegisterType)aType
{
	NSParameterAssert(aName != nil);
	NSParameterAssert(aAddress != NULL);
	
	if (!(self = [super init]))
		return nil;
	
	name = [aName copy];
	address = aAddress;
	type = aType;
	
	return self;
}

-(NSNumber*)value
{
	switch (type)
	{
		case CXRegisterCR: return @(*(uint8_t*)address);
		case CXRegisterGPR: return @(*(uint32_t*)address);
		case CXRegisterFPR: return @(*(double*)address);
		case CXRegisterSPR: return @(*(uint32_t*)address);
		default: return nil;
	}
}

-(void)setValue:(NSNumber *)value
{
	[self willChangeValueForKey:@"value"];
	switch (type)
	{
		case CXRegisterCR: *(uint8_t*)address = value.unsignedCharValue; break;
		case CXRegisterFPR: *(double*)address = value.doubleValue; break;
			
		case CXRegisterGPR:
		case CXRegisterSPR:
			*(uint32_t*)address = value.unsignedIntValue;
			break;
	}
	[self didChangeValueForKey:@"value"];
}

-(NSString*)description
{
	return [NSString stringWithFormat:@"%@: %@", name, self.value];
}

@end
