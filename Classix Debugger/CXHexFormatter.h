//
//  CXHexFormatter.h
//  Classix
//
//  Created by Félix on 2013-02-16.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CXHexFormatter : NSFormatter

-(NSString *)stringForObjectValue:(id)obj;
-(NSString *)editingStringForObjectValue:(id)obj;

-(BOOL)getObjectValue:(out id *)obj forString:(NSString *)string errorDescription:(out NSString **)error;

@end
