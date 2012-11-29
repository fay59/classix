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
}

@property (copy) NSString* component;
@property (assign) NSUInteger documentId;
@property (copy) NSArray* params;
@property (readonly) NSURL* URL;

+(id)requestWithURL:(NSURL*)url;

-(id)init;
-(id)initWithURL:(NSURL*)url;

-(NSString*)description;

@end
