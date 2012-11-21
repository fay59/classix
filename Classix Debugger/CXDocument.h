//
//  CXDocument.h
//  Classix Debugger
//
//  Created by Félix on 2012-11-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

struct ClassixCoreVM;

extern NSString* CXErrorDomain;
extern NSString* CXErrorFileURL;

enum CXErrorCode
{
	CXErrorCodeNotLocalURL = 1,
	CXErrorCodeFileNotLoadable = 2
};

@interface CXDocument : NSDocument
{
	struct ClassixCoreVM* vm;
	NSData* execData;
}

@end
