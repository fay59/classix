//
//  PPCMachineState.m
//  pefdump
//
//  Created by Félix on 2012-10-26.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "PPCMachineState.h"
#include "MachineState.h"

#define STATE			reinterpret_cast<PPCVM::MachineState*>(state)
#define NAMED_REG(n, N)	-(uint32_t)n { return STATE->n; } -(void)set##N:(uint32_t)value { STATE->n = value; }

@implementation PPCMachineState

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	state = new PPCVM::MachineState;
	ownsState = YES;
	
	return self;
}

-(id)initWithMachineState:(void*)machineState
{
	if (!(self = [super init]))
		return nil;
	
	state = reinterpret_cast<PPCVM::MachineState*>(machineState);
	ownsState = NO;
	
	return self;
}

-(uint32_t)GPR:(uint32_t)gprNumber
{
	NSParameterAssert(gprNumber >= 0);
	NSParameterAssert(gprNumber < 32);
	return STATE->gpr[gprNumber];
}

-(void)setGPR:(uint32_t)gprNumber value:(uint32_t)value
{
	NSParameterAssert(gprNumber >= 0);
	NSParameterAssert(gprNumber < 32);
	STATE->gpr[gprNumber] = value;
}

-(double)FPR:(uint32_t)fprNumber
{
	NSParameterAssert(fprNumber >= 0);
	NSParameterAssert(fprNumber < 32);
	return STATE->fpr[fprNumber];
}

-(void)setFPR:(uint32_t)fprNumber value:(double)value
{
	NSParameterAssert(fprNumber >= 0);
	NSParameterAssert(fprNumber < 32);
	STATE->fpr[fprNumber] = value;
}

NAMED_REG(lr, Lr);
NAMED_REG(ctr, Ctr);
NAMED_REG(cr, Cr);
NAMED_REG(xer, Xer);
NAMED_REG(fpscr, Fpscr);
NAMED_REG(pc, Pc);

@end
