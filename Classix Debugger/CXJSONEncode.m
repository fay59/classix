//
//  CXJSONDecode.m
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "CXJSONEncode.h"
#include <objc/runtime.h>

static NSString* CXJSONEncodeString(NSString* string)
{
	NSString* escapedSlashes = [string stringByReplacingOccurrencesOfString:@"\\" withString:@"\\\\"];
	NSString* escapedQuotes = [escapedSlashes stringByReplacingOccurrencesOfString:@"\"" withString:@"\\\""];
	return [NSString stringWithFormat:@"\"%@\"", escapedQuotes];
}

static NSString* CXJSONEncodeDictionary(NSDictionary* dict)
{
	NSMutableArray* elements = [NSMutableArray arrayWithCapacity:[dict count]];
	for (NSString* key in dict)
	{
		id<NSObject> object = [dict objectForKey:key];
		NSString* element = [NSString stringWithFormat:@"%@:%@", CXJSONEncodeString(key), CXJSONEncode(object)];
		[elements addObject:element];
	}
	
	return [NSString stringWithFormat:@"{%@}", [elements componentsJoinedByString:@","]];
}

static NSString* CXJSONEncodeArray(NSArray* array)
{
	NSMutableArray* elements = [NSMutableArray arrayWithCapacity:[array count]];
	for (id<NSObject> object in array)
		[elements addObject:CXJSONEncode(object)];
	
	return [NSString stringWithFormat:@"[%@]", [elements componentsJoinedByString:@","]];
}

NSString* CXJSONEncode(id<NSObject> object)
{
	if ([object isKindOfClass:[NSString class]])
		return CXJSONEncodeString((NSString*)object);
	
	if ([object isKindOfClass:[NSNumber class]])
		return [object description];
	
	if ([object isKindOfClass:[NSDate class]])
		return [object description];
	
	if ([object isKindOfClass:[NSNull class]])
		return @"null";
	
	if ([object isKindOfClass:[NSDictionary class]])
		return CXJSONEncodeDictionary((NSDictionary*)object);
	
	if ([object isKindOfClass:[NSArray class]])
		return CXJSONEncodeArray((NSArray*)object);
	
	NSLog(@"*** trying to JSON-encode an object of class %s", class_getName([object class]));
	return nil;
}