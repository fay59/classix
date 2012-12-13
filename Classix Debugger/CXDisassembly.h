//
//  CXDisassembly.h
//  Classix
//
//  Created by Félix on 2012-12-04.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CXVirtualMachine.h"
#import "CXCodeLabel.h"

@interface CXDisassembly : NSObject
{
	CXVirtualMachine* vm;
	NSMutableDictionary* displayNames;
	NSDictionary* addressesToUniqueNames;
	NSDictionary* disassembly;
	NSArray* orderedAddresses;
}

@property (copy) NSMutableDictionary* displayNames;

-(id)initWithVirtualMachine:(CXVirtualMachine*)vm;

-(NSArray*)functionDisassemblyForUniqueName:(NSString*)uniqueName;
-(NSArray*)functionDisassemblyForAddress:(uint32_t)address;

-(CXCodeLabel*)labelDisassemblyForUniqueName:(NSString*)uniqueName;
-(CXCodeLabel*)labelDisassemblyForAddress:(uint32_t)address;

-(NSString*)closestUniqueNameToAddress:(uint32_t)address;

-(NSString*)displayNameForUniqueName:(NSString*)uniqueName;
-(void)setDisplayName:(NSString*)displayName forUniqueName:(NSString*)uniqueName;

-(void)dealloc;

@end
