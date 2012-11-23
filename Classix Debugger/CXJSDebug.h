//
//  CXJSDebug.h
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CXJSDebug : NSObject

+(BOOL)isSelectorExcludedFromWebScript:(SEL)sel;
-(void)log:(NSString*)message;

@end
