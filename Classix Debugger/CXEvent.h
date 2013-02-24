//
//  CXEventListener.h
//  Classix
//
//  Created by Félix on 2013-02-24.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

struct CXEventCXX;

typedef void (^CXEventListener)(id owner, NSDictionary* eventData);

@interface CXEvent : NSObject
{
	struct CXEventCXX* cxx;
	__weak id owner;
}

-(id)initWithOwner:(id)owner;

-(void)attach:(CXEventListener)listener;
-(void)detach:(CXEventListener)listener;
-(void)attachListener:(id)listener selector:(SEL)selector;
-(void)detachListener:(id)listener;

-(void)triggerWithData:(NSDictionary*)data;

@end
