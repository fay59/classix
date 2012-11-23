//
//  CXJSDebug.m
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "CXJSDebug.h"

@implementation CXJSDebug

+(BOOL)isSelectorExcludedFromWebScript:(SEL)sel
{
	return sel != @selector(log:);
}

-(void)log:(NSString *)message
{
	NSLog(@"[JS] %@", message);
}

@end
