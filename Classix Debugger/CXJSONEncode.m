//
// CXJSONEncode.m
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