//
// CXDBRequest.m
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

#import "CXDBURLProtocol.h"
#import "CXDBRequest.h"

@implementation CXDBRequest

@synthesize component;
@synthesize documentId;
@synthesize params;

-(id)init
{
	return [super init];
}

-(id)initWithURL:(NSURL *)url
{
	if (!(self = [self init]))
		return nil;
	
	if (![url.scheme isEqualToString:[CXDBURLProtocol CXDBProtocolScheme]])
		return nil;
	
	NSArray* urlParams = url.pathComponents;
	
	self.component = url.host;
	
	int skipCount = 1;
	if (urlParams.count > 1)
	{
		NSString* firstParam = [urlParams objectAtIndex:1];
		NSInteger integer = [firstParam integerValue];
		if (integer > 0)
		{
			self.documentId = integer;
			skipCount++;
		}
	}
	
	self.params = [urlParams subarrayWithRange:NSMakeRange(skipCount, urlParams.count - skipCount)];
	
	return self;
}

+(id)requestWithURL:(NSURL *)url
{
	return [[[self alloc] initWithURL:url] autorelease];
}

-(NSURL*)URL
{
	NSMutableString* url = [NSMutableString stringWithFormat:@"%@://", [CXDBURLProtocol CXDBProtocolScheme]];
	[url appendString:component];
	if (documentId != 0) [url appendFormat:@"/%@", @(documentId)];
	[url appendFormat:@"/%@", [params componentsJoinedByString:@"/"]];
	
	return [NSURL URLWithString:url];
}

-(NSString*)description
{
	return self.URL.description;
}

@end
