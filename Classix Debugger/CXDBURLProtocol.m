//
//  CXDBURLProtocol.m
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "CXDBURLProtocol.h"
#import "CXJSONEncode.h"
#import "CXDocumentController.h"

@implementation CXDBURLProtocol

+(NSString*)CXDBProtocolScheme
{
	return @"cxdb";
}

+(void)initialize
{
	[NSURLProtocol registerClass:self];
}

+(BOOL)canInitWithRequest:(NSURLRequest *)request
{
	return [request.URL.scheme caseInsensitiveCompare:[self CXDBProtocolScheme]] == NSOrderedSame;
}

+(NSURLRequest*)canonicalRequestForRequest:(NSURLRequest *)request
{
	return request;
}

-(void)startLoading
{
	NSURLRequest* request = self.request;
	NSURL* url = request.URL;
	NSArray* pathComponents = [url.resourceSpecifier componentsSeparatedByString:@"/"];
	
	id<NSURLProtocolClient> client = self.client;
	
	NSLog(@"Doing request: %@", url);
	
	// if it's a data URL, load the file synchronously
	if ([[pathComponents objectAtIndex:0] isEqualToString:@"resource"])
	{
		NSString* resourceGivenName = [pathComponents objectAtIndex:1];
		NSString* resourceName = [resourceGivenName stringByDeletingPathExtension];
		NSString* resourceType = [resourceGivenName pathExtension];
		NSString* resourcePath = [NSBundle.mainBundle pathForResource:resourceName ofType:resourceType];
		NSString* fullPath = [resourcePath stringByExpandingTildeInPath];
		NSURL* resourceURL = [NSURL fileURLWithPath:fullPath];
		NSURLRequest* fileRequest = [NSURLRequest requestWithURL:resourceURL];
		
		[NSURLConnection sendAsynchronousRequest:fileRequest queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse* response, NSData* data, NSError* error) {
			if (error == nil)
			{
				[client URLProtocol:self didReceiveResponse:response cacheStoragePolicy:NSURLCacheStorageAllowedInMemoryOnly];
				[client URLProtocol:self didLoadData:data];
				[client URLProtocolDidFinishLoading:self];
			}
			else
				[client URLProtocol:self didFailWithError:error];
		}];
		
		return;
	}
	
	// request format: documentId/command/param1/param2/...
	NSUInteger documentId = [[pathComponents objectAtIndex:0] integerValue];
	NSString* command = [pathComponents objectAtIndex:1];
	NSRange argumentsRange = NSMakeRange(2, pathComponents.count - 2);
	
	CXDocument* document = [[CXDocumentController documentController] documentWithId:documentId];
	dispatch_async(dispatch_get_main_queue(), ^{
		
		NSString* mimeType;
		NSString* textEncoding = nil;
		NSData* data;
		
		id result = [document executeCommand:command arguments:[pathComponents subarrayWithRange:argumentsRange]];
		
		if (result == nil)
		{
			NSError* error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorResourceUnavailable userInfo:nil];
			[client URLProtocol:self didFailWithError:error];
		}
		else
		{
			if ([result isKindOfClass:[NSData class]])
			{
				mimeType = @"application/octet-stream";
				data = result;
			}
			else
			{
				mimeType = @"application/json";
				textEncoding = @"UTF-8";
				NSString* json = CXJSONEncode(result);
				data = [json dataUsingEncoding:NSUTF8StringEncoding];
			}
			
			NSURLResponse* response = [[NSURLResponse alloc] initWithURL:request.URL MIMEType:mimeType expectedContentLength:data.length textEncodingName:textEncoding];
			
			[client URLProtocol:self didReceiveResponse:response cacheStoragePolicy:NSURLCacheStorageNotAllowed];
			[client URLProtocol:self didLoadData:data];
			[client URLProtocolDidFinishLoading:self];
			[response release];
		}
	});
}

-(void)stopLoading
{ }

@end
