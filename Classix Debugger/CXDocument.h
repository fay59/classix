//
//  CXDocument.h
//  Classix Debugger
//
//  Created by Félix on 2012-11-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

struct ClassixCoreVM;

@interface CXDocument : NSDocument
{
	struct ClassixCoreVM* vm;
	NSData* execData;
}

@end
