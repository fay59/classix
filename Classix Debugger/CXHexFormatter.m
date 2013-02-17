//
//  CXHexFormatter.m
//  Classix
//
//  Created by Félix on 2013-02-16.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#import "CXHexFormatter.h"

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

@end
