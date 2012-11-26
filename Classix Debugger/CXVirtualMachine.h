//
//  CXVirtualMachine.h
//  Classix
//
//  Created by Félix on 2012-11-25.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

struct ClassixCoreVM;

extern NSNumber* CXVirtualMachineGPRKey;
extern NSNumber* CXVirtualMachineFPRKey;
extern NSNumber* CXVirtualMachineCRKey;
extern NSNumber* CXVirtualMachineSPRKey;

@interface CXVirtualMachine : NSObject <NSOutlineViewDataSource>
{
	struct ClassixCoreVM* vm;
	NSDictionary* registers;
}

@property (readonly) NSArray* gpr;
@property (readonly) NSArray* fpr;
@property (readonly) NSArray* cr;
@property (readonly) NSArray* spr;
@property (readonly) NSDictionary* allRegisters; // split by category

-(id)init;

@end
