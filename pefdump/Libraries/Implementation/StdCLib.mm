//
//  StdCLib.m
//  pefdump
//
//  Created by Félix on 2012-10-28.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "BigEndian.h"
#include "MachineState.h"
#import "StdCLib.h"

struct StdCLibGlobals
{
	Common::UInt32 __C_phase;
	Common::UInt32 __loc;
	Common::UInt32 __NubAt3;
	Common::UInt32 __p_CType;
	Common::UInt32 __SigEnv;
	MachineState __target_for_exit;
	Common::UInt32 __yd;
	Common::UInt32 _CategoryLoc;
	Common::Real64 _DBL_EPSILON;
	Common::Real64 _DBL_MAX;
	Common::Real64 _DBL_MIN;
	Common::UInt32 _exit_status;
	Common::Real64 _FLT_EPSILON;
	Common::Real64 _FLT_MAX;
	Common::Real64 _FLT_MIN;
	Common::UInt32 _IntEnv;
	Common::UInt32 _iob;
	Common::UInt32 _lastbuf;
	Common::Real64 _LDBL_EPSILON;
	Common::Real64 _LDBL_MIN;
	Common::Real64 _LDBL_MAX;
	Common::UInt32 _PublicTimeInfo;
	Common::UInt32 _StdDevs;
	Common::UInt32 errno_;
	Common::UInt32 MacOSErr;
	Common::UInt32 MoneyData;
	Common::UInt32 NoMoreDebugStr;
	Common::UInt32 NumericData;
	Common::UInt32 StandAlone;
	Common::UInt32 TimeData;
	
	StdCLibGlobals()
	{
		// I know, I know, this is ugly
		memset(this, 0, sizeof *this);
		
		_DBL_EPSILON = DBL_EPSILON;
		_DBL_MIN = DBL_MIN;
		_DBL_MAX = DBL_MAX;
		
		_FLT_EPSILON = FLT_EPSILON;
		_FLT_MIN = FLT_MIN;
		_FLT_MAX = FLT_MAX;
		
		_LDBL_EPSILON = DBL_EPSILON;
		_DBL_MIN = DBL_MIN;
		_LDBL_MAX = DBL_MAX;
	}
};

@implementation StdCLib

-(id)initWithAllocator:(PPCAllocator *)aAllocator
{
	if (!(self = [super init]))
		return nil;
	
	allocator = [aAllocator retain];
	globals = reinterpret_cast<StdCLibGlobals*>([allocator allocate:sizeof(StdCLibGlobals)]);
	
	return self;
}

-(NSString*)libraryName
{
	return @"StdCLib";
}

-(PPCLibraryFunction)resolve:(NSString *)functionName
{
	return nullptr;
}

-(void)dealloc
{
	[allocator deallocate:globals];
	[allocator release];
	[super dealloc];
}

#pragma mark -
#pragma mark Data Symbols
-(void*)StdCLib___C_phase { return &globals->__C_phase; }
-(void*)StdCLib___loc { return &globals->__loc; }
-(void*)StdCLib___NubAt3 { return &globals->__NubAt3; }
-(void*)StdCLib___p_CType { return &globals->__p_CType; }
-(void*)StdCLib___SigEnv { return &globals->__SigEnv; }
-(void*)StdCLib___target_for_exit { return &globals->__target_for_exit; }
-(void*)StdCLib___yd { return &globals->__yd; }
-(void*)StdCLib__CategoryLoc { return &globals->_CategoryLoc; }
-(void*)StdCLib__DBL_EPSILON { return &globals->_DBL_EPSILON; }
-(void*)StdCLib__DBL_MAX { return &globals->_DBL_MAX; }
-(void*)StdCLib__DBL_MIN { return &globals->_DBL_MIN; }
-(void*)StdCLib__exit_status { return &globals->_exit_status; }
-(void*)StdCLib__FLT_EPSILON { return &globals->_FLT_EPSILON; }
-(void*)StdCLib__FLT_MAX { return &globals->_FLT_MAX; }
-(void*)StdCLib__FLT_MIN { return &globals->_FLT_MIN; }
-(void*)StdCLib__IntEnv { return &globals->_IntEnv; }
-(void*)StdCLib__iob { return &globals->_iob; }
-(void*)StdCLib__lastbuf { return &globals->_lastbuf; }
-(void*)StdCLib__LDBL_EPSILON { return &globals->_LDBL_EPSILON; }
-(void*)StdCLib__LDBL_MAX { return &globals->_LDBL_MAX; }
-(void*)StdCLib__LDBL_MIN { return &globals->_LDBL_MIN; }
-(void*)StdCLib__PublicTimeInfo { return &globals->_PublicTimeInfo; }
-(void*)StdCLib__StdDevs { return &globals->_StdDevs; }
-(void*)StdCLib_errno { return &globals->errno_; }
-(void*)StdCLib_MacOSErr { return &globals->MacOSErr; }
-(void*)StdCLib_MoneyData { return &globals->MoneyData; }
-(void*)StdCLib_NoMoreDebugStr { return &globals->NoMoreDebugStr; }
-(void*)StdCLib_NumericData { return &globals->NumericData; }
-(void*)StdCLib_StandAlone { return &globals->StandAlone; }
-(void*)StdCLib_TimeData { return &globals->TimeData; }

#pragma mark -
#pragma mark Code Symbols
// for now, just implement what it takes for the Hello program
-(void)StdCLib___setjmp:(MachineState *)state
{
	// TODO this is *almost certainly* not the right way to do it
	void* address = [allocator translate:state->gpr[3]];
	memcpy(address, state, sizeof *state);
	state->gpr[3] = 0;
}

-(void)StdCLib__BreakPoint:(MachineState *)state
{
	__asm__ ("int $3");
}

-(void)StdCLib_exit:(MachineState *)state
{
	// TODO longjmp to __target_for_exit
	// for now we'll just kill the host (which is really, really bad)
	exit(state->gpr[3]);
}

-(void)StdCLib_puts:(MachineState *)state
{
	void* address = [allocator translate:state->gpr[3]];
	const char* ptr = reinterpret_cast<const char*>(address);
	puts(ptr);
}

@end
