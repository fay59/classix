//
//  CXDBRequest.h
//  Classix
//
//  Created by Félix on 2012-11-25.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CXDBRequest : NSObject
{
	NSString* component;
	NSUInteger documentId;
	NSArray* params;
	NSString* hash;
}

@property (copy) NSString* component;
@property (assign) NSUInteger documentId;
@property (copy) NSArray* params;
@property (copy) NSString* hash;
@property (readonly) NSURL* URL;

+(id)requestWithURL:(NSURL*)url;

-(id)init;
-(id)initWithURL:(NSURL*)url;

-(NSString*)description;

@end
