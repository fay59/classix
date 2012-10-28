//
//  PPCMachineState.h
//  pefdump
//
//  Created by Félix on 2012-10-26.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PPCMachineState : NSObject
{
	void* state;
	BOOL ownsState;
}

@property (assign) uint32_t lr;
@property (assign) uint32_t ctr;
@property (assign) uint32_t cr;
@property (assign) uint32_t xer;
@property (assign) uint32_t fpscr;
@property (assign) uint32_t pc;

-(id)init;
-(id)initWithMachineState:(void*)machineState;

-(uint32_t)GPR:(uint32_t)gprNumber;
-(void)setGPR:(uint32_t)gprNumber value:(uint32_t)value;

-(double)FPR:(uint32_t)fprNumber;
-(void)setFPR:(uint32_t)fprNumber value:(double)value;

@end
