//
//  CXJSAdapter.h
//  Classix
//
//  Created by Félix on 2012-11-30.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

@class CXDocument;

@interface CXJSAdapter : NSObject
{
	CXDocument* document;
}

+(BOOL)isSelectorExcludedFromWebScript:(SEL)selector;

-(id)initWithDocument:(CXDocument*)document;

-(void)setPC:(uint32_t)pc;

-(NSArray*)breakpoints;
-(BOOL)toggleBreakpoint:(uint32_t)address;

-(void)dealloc;

@end
