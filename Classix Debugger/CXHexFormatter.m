//
//  CXHexFormatter.m
//  Classix
//
//  Created by Félix on 2013-02-16.
//  Copyright (c) 2013 Félix. All rights reserved.
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
