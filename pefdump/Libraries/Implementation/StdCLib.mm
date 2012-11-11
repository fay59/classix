//
//  StdCLib.m
//  pefdump
//
//  Created by Félix on 2012-10-28.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <cstring>
#include "BigEndian.h"
#include "MachineState.h"
#include "STAllocator.h"
#import "StdCLib.h"

const int StdCLib_NFILE = 40;

union StdCLibPPCFILE
{
	struct
	{
		int32_t _cnt;
		uint32_t _ptr;
		uint32_t _base;
		uint32_t _end;
		uint16_t _size;
		uint16_t _flag;
		uint16_t _file;
	};
	
	struct
	{
		uint32_t : 32;
		FILE* fptr;
	};
};

struct StdCLibGlobals
{
#pragma mark Globals
	Common::UInt32 __C_phase;
	Common::UInt32 __loc;
	Common::UInt32 __NubAt3;
	Common::UInt32 __p_CType;
	Common::UInt32 __SigEnv;
	uint32_t __target_for_exit[64];
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
	StdCLibPPCFILE _iob[StdCLib_NFILE];
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
	
#pragma mark Housekeeping
	
	StdCLibGlobals()
	{
		memset(__target_for_exit, 0, sizeof __target_for_exit);
		
		_iob[0].fptr = stdin;
		_iob[1].fptr = stdout;
		_iob[2].fptr = stderr;
		for (int i = 3; i < StdCLib_NFILE; i++)
			_iob[i].fptr = nullptr;
		
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

static FILE* MakeFilePtr(StdCLibGlobals* globals, intptr_t ptr)
{
	for (int i = 0; i < StdCLib_NFILE; i++)
	{
		intptr_t thisIOB = reinterpret_cast<intptr_t>(&globals->_iob[i]);
		if (ptr == thisIOB)
			return globals->_iob[i].fptr;
	}
	return nullptr;
}

-(id)initWithAllocator:(PPCAllocator *)aAllocator
{
	if (!(self = [super init]))
		return nil;
	
	allocator = [aAllocator retain];
	void* globalsRegion = [allocator allocate:sizeof(StdCLibGlobals) reason:@"StdCLib Globals"];
	globals = reinterpret_cast<StdCLibGlobals*>(globalsRegion);
	new (globals) StdCLibGlobals();
	
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
-(void)StdCLib___setjmp:(MachineState *)state
{
	// since the client is not really supposed to read from jmpBuf values, we
	// shouldn't have to worry about endianness
	uint32_t* jmpBuf = reinterpret_cast<uint32_t*>([allocator translate:state->r3]);
	jmpBuf[0] = state->lr;
	jmpBuf[1] = MachineStateGetCR(state);
	jmpBuf[2] = state->r1;
	jmpBuf[3] = state->r2;
	jmpBuf[4] = 0;
	memcpy(jmpBuf + 5, state->gpr + 13, 19 * sizeof(uint32_t));
	memcpy(jmpBuf + 24, state->fpr + 14, 18 * sizeof(double));
	jmpBuf[61] = 0;
	jmpBuf[62] = 0;
	
	// ??? there are two fields left over
	jmpBuf[63] = 0;
	jmpBuf[64] = 0;
	
	state->r3 = 0;
}

-(void)StdCLib__BreakPoint:(MachineState *)state
{
	__asm__ ("int $3");
}

-(void)StdCLib__filbuf:(MachineState *)state
{
	fprintf(stderr, "Using '_filbuf'\n");
	state->r3 = 0;
}

-(void)StdCLib__flsbuf:(MachineState *)state
{
	fprintf(stderr, "Using '_flsbuf'\n");
	state->r3 = 0;
}

-(void)StdCLib_exit:(MachineState *)state
{
	// TODO longjmp to __target_for_exit
	// for now we'll just kill the host (which is really, really bad)
	exit(state->r3);
}

-(void)StdCLib_faccess:(MachineState *)state
{
	fprintf(stderr, "Using 'faccess' with mode %08x\n", state->r4);
	state->r3 = 0;
}

-(void)StdCLib_fclose:(MachineState *)state
{
	for (int i = 0; i < StdCLib_NFILE; i++)
	{
		auto& ioBuffer = globals->_iob[i];
		intptr_t ioBufferAddress = reinterpret_cast<intptr_t>(&ioBuffer);
		if (ioBufferAddress == state->r3)
		{
			state->r3 = fclose(ioBuffer.fptr);
			ioBuffer.fptr = nullptr;
			return;
		}
	}
	
	globals->errno_ = EBADF;
	state->r3 = EOF;
}

-(void)StdCLib_fgets:(MachineState *)state
{
	char* buffer = reinterpret_cast<char*>(state->r3);
	int32_t size = state->r4;
	FILE* fptr = MakeFilePtr(globals, state->r5);
	
	char* result = fgets(buffer, size, fptr);
	state->r3 = reinterpret_cast<intptr_t>(result);
}

-(void)StdCLib_fopen:(MachineState *)state
{
	const char* filename = reinterpret_cast<const char*>(state->r3);
	const char* mode = reinterpret_cast<const char*>(state->r4);
	
	for (int i = 0; i < StdCLib_NFILE; i++)
	{
		auto& ioBuffer = globals->_iob[i];
		if (ioBuffer.fptr == nullptr)
		{
			ioBuffer.fptr = fopen(filename, mode);
			state->r3 = reinterpret_cast<intptr_t>(&ioBuffer);
			return;
		}
	}
	state->r3 = 0;
}

-(void)StdCLib_fprintf:(MachineState *)state
{
	FILE* fptr = MakeFilePtr(globals, state->r3);
	const char* formatString = reinterpret_cast<const char*>(state->r4);
	// gah, let's just print the format string for now.
	state->r3 = fputs(formatString, fptr);
}

-(void)StdCLib_fseek:(MachineState *)state
{
	FILE* fptr = MakeFilePtr(globals, state->r3);
	int32_t offset = state->r4;
	int whence = state->r5;
	state->r3 = fseek(fptr, offset, whence);
}

-(void)StdCLib_getenv:(MachineState *)state
{
	const char* name = reinterpret_cast<const char*>(state->r3);
	const char* env = getenv(name);
	state->r3 = reinterpret_cast<intptr_t>(env);
}

-(void)StdCLib_memcmp:(MachineState *)state
{
	const void* s1 = reinterpret_cast<const void*>(state->r3);
	const void* s2 = reinterpret_cast<const void*>(state->r4);
	size_t size = state->r5;
	state->r3 = memcmp(s1, s2, size);
}

-(void)StdCLib_memcpy:(MachineState *)state
{
	void* s1 = reinterpret_cast<void*>(state->r3);
	const void* s2 = reinterpret_cast<const void*>(state->r4);
	size_t size = state->r5;
	state->r3 = reinterpret_cast<intptr_t>(memcpy(s1, s2, size));
}

-(void)StdCLib_printf:(MachineState *)state
{
	const char* formatString = reinterpret_cast<const char*>(state->r3);
	// gah, let's just print the format string for now.
	state->r3 = puts(formatString);
}

-(void)StdCLib_puts:(MachineState *)state
{
	void* address = [allocator translate:state->r3];
	const char* ptr = reinterpret_cast<const char*>(address);
	state->r3 = puts(ptr);
}

-(void)StdCLib_strchr:(MachineState *)state
{
	const char* s = reinterpret_cast<const char*>(state->r3);
	int c = state->r4;
	char* result = strrchr(s, c);
	state->r3 = reinterpret_cast<intptr_t>(result);
}

-(void)StdCLib_strcmp:(MachineState *)state
{
	const char* s1 = reinterpret_cast<const char*>(state->r3);
	const char* s2 = reinterpret_cast<const char*>(state->r4);
	state->r3 = strcmp(s1, s2);
}

-(void)StdCLib_strcpy:(MachineState *)state
{
	char* s1 = reinterpret_cast<char*>(state->r3);
	const char* s2 = reinterpret_cast<const char*>(state->r4);
	state->r3 = reinterpret_cast<intptr_t>(strcpy(s1, s2));
}

-(void)StdCLib_strlen:(MachineState *)state
{
	char* s = reinterpret_cast<char*>(state->r3);
	state->r3 = strlen(s);
}

-(void)StdCLib_tolower:(MachineState *)state
{
	state->r3 = tolower(state->r3);
}

@end
