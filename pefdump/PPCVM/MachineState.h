//
//  MachineState.h
//  pefdump
//
//  Created by Félix on 2012-10-26.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__MachineState__
#define __pefdump__MachineState__

#ifdef __cplusplus
# define EXTERN_C extern "C"
#else
# define EXTERN_C
#endif

#include <stdint.h>

typedef struct MachineState
{
	uint32_t gpr[32];
	double fpr[32];
	uint32_t pc;
	uint8_t cr[8];
	
	// system registers
	union
	{
		uint32_t xer;
		struct
		{
			unsigned xer_so : 1;
			unsigned xer_ov : 1;
			unsigned xer_ca : 1;
			unsigned xer_unused : 22;
			unsigned xer_bytecount : 7;
		};
	};
	uint32_t lr;
	uint32_t ctr;
	
	// floating-point status
	union
	{
		struct
		{
			unsigned RN		:	2;
			unsigned NI		:	1;
			unsigned XE		:	1;
			unsigned ZE		:	1;
			unsigned UE		:	1;
			unsigned OE		:	1;
			unsigned VE		:	1;
			unsigned VXCVI	:	1;
			unsigned VXSQRT	:	1;
			unsigned VXSOFT	:	1;
			unsigned		:	1;
			unsigned FPRF	:	5;
			unsigned FI		:	1;
			unsigned FR		:	1;
			unsigned VXVC	:	1;
			unsigned VXIMZ	:	1;
			unsigned VXZDZ	:	1;
			unsigned VXIDI	:	1;
			unsigned VXISI	:	1;
			unsigned VXSNAN	:	1;
			unsigned XX		:	1;
			unsigned ZX		:	1;
			unsigned UX		:	1;
			unsigned OX		:	1;
			unsigned VX		:	1;
			unsigned FEX	:	1;
			unsigned FX		:	1;
		};
		unsigned hex;
	} fpscr;
} MachineState;

EXTERN_C void MachineStateInit(MachineState* state);

#undef EXTERN_C

#endif /* defined(__pefdump__MachineState__) */
