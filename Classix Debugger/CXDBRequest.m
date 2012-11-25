//
//  CXDBRequest.m
//  Classix
//
//  Created by Félix on 2012-11-25.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "CXDBURLProtocol.h"
#import "CXDBRequest.h"

@implementation CXDBRequest

@synthesize component;
@synthesize documentId;
@synthesize params;
@synthesize hash;

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
	self.hash = url.fragment;
	
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
	if (hash != nil) [url appendFormat:@"#%@", hash];
	
	return [NSURL URLWithString:url];
}

-(NSString*)description
{
	return self.URL.description;
}

@end
