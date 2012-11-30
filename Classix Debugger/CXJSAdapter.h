//
//  CXJSAdapter.h
//  Classix
//
//  Created by Félix on 2012-11-30.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

@class CXDocument;

@interface CXJSAdapter : NSObject
{
	CXDocument* document;
	NSMutableSet* breakpoints;
}

-(id)initWithDocument:(CXDocument*)document;

-(void)setPC:(uint32_t)pc;

-(NSArray*)breakpoints;
-(BOOL)toggleBreakpoint:(uint32_t)address;

-(void)dealloc;

@end
