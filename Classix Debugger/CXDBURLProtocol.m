//
// CXDBURLProtocol.m
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
#import "CXJSONEncode.h"
#import "CXDocumentController.h"
#import "CXDBRequest.h"

@interface CXDBURLProtocol (Private)

-(void)respondToRequest:(CXDBRequest*)request withData:(NSData*)data ofType:(NSString*)mimeType error:(NSError*)error;
-(void)loadResource:(CXDBRequest*)request;
-(void)loadDisassembly:(CXDBRequest*)request;

-(NSString*)argumentsToXHTML:(NSArray*)arguments;
-(NSString*)argumentToXHTML:(NSDictionary*)argument;

@end

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
	NSURL* url = request.URL;
	return [url.scheme caseInsensitiveCompare:self.CXDBProtocolScheme] == NSOrderedSame;
}

+(NSURLRequest*)canonicalRequestForRequest:(NSURLRequest *)request
{
	return request;
}

-(void)startLoading
{
	CXDBRequest* request = [CXDBRequest requestWithURL:self.request.URL];
	NSLog(@"Doing request: %@", request);
	
	// if it's a data URL, load the file synchronously
	if ([request.component isEqualToString:@"resource"])
	{
		[self loadResource:request];
	}
	else if ([request.component isEqualToString:@"disassembly"])
	{
		[self loadDisassembly:request];
	}
	return;
	
#if 0
	// request format: documentId/command/param1/param2/...
	NSUInteger documentId = [[pathComponents objectAtIndex:0] integerValue];
	NSString* command = [pathComponents objectAtIndex:1];
	NSRange argumentsRange = NSMakeRange(2, pathComponents.count - 2);
	
	CXDocument* document = [[CXDocumentController documentController] documentWithId:documentId];
	dispatch_async(dispatch_get_main_queue(),
	^{
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
			
			// it's legal to cache "sections" responses
			NSURLCacheStoragePolicy policy = [command isEqualToString:@"sections"] ? NSURLCacheStorageAllowedInMemoryOnly : NSURLCacheStorageNotAllowed;
			
			[client URLProtocol:self didReceiveResponse:response cacheStoragePolicy:policy];
			[client URLProtocol:self didLoadData:data];
			[client URLProtocolDidFinishLoading:self];
			[response release];
		}
	});
#endif
}

-(void)stopLoading
{ }

#pragma mark -
#pragma mark Private Implementation

-(void)respondToRequest:(CXDBRequest*)request withData:(NSData*)data ofType:(NSString*)mimeType error:(NSError*)error
{
	id<NSURLProtocolClient> client = self.client;
	
	if (error == nil)
	{
		NSURLResponse* response = [[NSURLResponse alloc] initWithURL:request.URL MIMEType:mimeType expectedContentLength:data.length textEncodingName:@"UTF-8"];
		[client URLProtocol:self didReceiveResponse:response cacheStoragePolicy:NSURLCacheStorageAllowedInMemoryOnly];
		[client URLProtocol:self didLoadData:data];
		[client URLProtocolDidFinishLoading:self];
	}
	else
		[client URLProtocol:self didFailWithError:error];
}

-(void)loadResource:(CXDBRequest*)request
{
	NSURL* url = request.URL;
	NSArray* pathComponents = url.pathComponents;
	
	NSString* resourceGivenName = [pathComponents objectAtIndex:1];
	NSString* resourceName = [resourceGivenName stringByDeletingPathExtension];
	NSString* resourceType = [resourceGivenName pathExtension];
	NSString* resourcePath = [NSBundle.mainBundle pathForResource:resourceName ofType:resourceType];
	NSString* fullPath = [resourcePath stringByExpandingTildeInPath];
	NSURL* resourceURL = [NSURL fileURLWithPath:fullPath];
	NSURLRequest* fileRequest = [NSURLRequest requestWithURL:resourceURL];
	
	[NSURLConnection sendAsynchronousRequest:fileRequest queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse* response, NSData* data, NSError* error)
	{
		[self respondToRequest:request withData:data ofType:response.MIMEType error:error];
	}];
}

-(void)loadDisassembly:(CXDBRequest*)request
{
	NSError* error = nil;
	NSString* templatePath = [[NSBundle mainBundle] pathForResource:@"cxdb" ofType:@"xhtml"];
	NSMutableString* template = [NSMutableString stringWithContentsOfFile:templatePath encoding:NSUTF8StringEncoding error:&error];
	if (error != nil)
	{
		[self respondToRequest:request withData:nil ofType:nil error:error];
		return;
	}
	
	NSString* label = request.params.lastObject;
	NSMutableString* xhtmlDisassembly = [NSMutableString stringWithString:@"<table id=\"disasm\">"];
	CXDocument* document = [[CXDocumentController documentController] documentWithId:request.documentId];
	NSArray* disassembly = [document disassemblyForAddress:label];
	
	for (NSDictionary* dict in disassembly)
	{
		NSString* label = [dict objectForKey:@"label"];
		NSArray* instructions = [dict objectForKey:@"instructions"];
		if (instructions.count == 0)
			continue;
		
		[xhtmlDisassembly appendFormat:@"<tr><th/><th colspan=\"5\" id=\"%@\">.%@</th></tr>", label, label];
		for (NSDictionary* instruction in instructions)
		{
			uint32_t location = [[instruction objectForKey:@"location"] integerValue];
			uint32_t code = [[instruction objectForKey:@"code"] integerValue];
			NSString* opcode = [instruction objectForKey:@"opcode"];
			NSArray* arguments = [instruction objectForKey:@"arguments"];
			id target = [instruction objectForKey:@"target"];
			
			[xhtmlDisassembly appendFormat:@"<tr id=\"i%08x\">", location];
			[xhtmlDisassembly appendFormat:@"<td> %08x</td>", location];
			[xhtmlDisassembly appendString:@"<td/>"];
			[xhtmlDisassembly appendFormat:@"<td> %08x</td>", code];
			[xhtmlDisassembly appendFormat:@"<td> %@</td>", opcode];
			[xhtmlDisassembly appendFormat:@"<td>%@</td>", [self argumentsToXHTML:arguments]];
			if (target != NSNull.null)
			{
				NSString* stringTarget = target;
				NSString* prefix = [stringTarget substringToIndex:2];
				NSString* targetContent;
				if ([prefix isEqualToString:@"lb"] || [prefix isEqualToString:@"fn"])
				{
					targetContent = [NSString stringWithFormat:@"<a href=\"%@#%@\">%@</a>", stringTarget, stringTarget, stringTarget];
				}
				else
				{
					targetContent = stringTarget;
				}
				[xhtmlDisassembly appendFormat:@"<td>%@</td>", targetContent];
			}
			[xhtmlDisassembly appendFormat:@"</tr>"];
		}
	}
	
	[xhtmlDisassembly appendString:@"</table>"];
	
	NSRange docIdRange = [template rangeOfString:@"##data-document-id##"];
	[template replaceCharactersInRange:docIdRange withString:[NSString stringWithFormat:@"%@", label]];
	
	NSRange tableRange = [template rangeOfString:@"<table id=\"disasm\"/>"];
	[template replaceCharactersInRange:tableRange withString:xhtmlDisassembly];

	NSData* result = [template dataUsingEncoding:NSUTF8StringEncoding];
	[self respondToRequest:request withData:result ofType:@"application/xhtml+xml" error:nil];
}

-(NSString*)argumentToXHTML:(NSDictionary *)argument
{
	int32_t value = [[argument objectForKey:@"value"] intValue];
	switch ([[argument objectForKey:@"type"] intValue])
	{
		default:
		case 0: return @"(null)";
			
		case 1: return [NSString stringWithFormat:@"<span class=\"gpr\">r%u</span>", value];
		case 2: return [NSString stringWithFormat:@"<span class=\"fpr\">fr%u</span>", value];
		case 3: return [NSString stringWithFormat:@"<span class=\"spr\">sr%u</span>", value]; // FIXME use good spr names
		case 4: return [NSString stringWithFormat:@"<span class=\"cr\">cr%u</span>", value];
		case 5:
		{
			int32_t reg = [[argument objectForKey:@"gpr"] intValue];
			return [NSString stringWithFormat:@"<span class=\"ptr\">%i(r%u)</span>", value, reg];
		}
		case 6:
		{
			const char* sign = "";
			if (value < 0)
			{
				sign = "-";
				value = abs(value);
			}
			if (value > 0xffff)
				return [NSString stringWithFormat:@"%s0x%08x", sign, value];
			else if (value > 0xff)
				return [NSString stringWithFormat:@"%s0x%04x", sign, value];
			else
				return [NSString stringWithFormat:@"%s0x%02x", sign, value];
		}
	}
}

-(NSString*)argumentsToXHTML:(NSArray *)arguments
{
	NSMutableArray* result = [NSMutableArray arrayWithCapacity:arguments.count];
	for (NSDictionary* arg in arguments)
		[result addObject:[self argumentToXHTML:arg]];
	return [result componentsJoinedByString:@", "];
}

@end
