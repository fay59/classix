//
// CXHexFormatter.m
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

#import "CXHexFormatter.h"

NSString* CXFormatErrorDomain = @"Format Error";
enum CXFormatErrorCodes
{
	CXNotHexDigits
};

@implementation CXHexFormatter

-(NSString*)stringForObjectValue:(id)obj
{
	if ([obj isKindOfClass:NSNumber.class])
	{
		NSNumber* number = obj;
		return [NSString stringWithFormat:@"0x%08x", number.unsignedIntValue];
	}
	return [obj description];
}

-(NSString*)editingStringForObjectValue:(id)obj
{
	if ([obj isKindOfClass:NSNumber.class])
	{
		NSNumber* number = obj;
		return [NSString stringWithFormat:@"%08x", number.unsignedIntValue];
	}
	
	return @"0";
}

-(BOOL)getObjectValue:(out id *)obj forString:(NSString *)string errorDescription:(out NSString **)error
{
	const char* str = string.UTF8String;
	char* end;
	unsigned long result = strtoul(str, &end, 16);
	
	if (*end != 0)
	{
		*obj = nil;
		*error = [NSError errorWithDomain:CXFormatErrorDomain code:CXNotHexDigits userInfo:nil];
		return NO;
	}
	
	*obj = @(result);
	return YES;
}

@end
