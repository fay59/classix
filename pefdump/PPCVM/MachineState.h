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
	uint32_t lr, ctr;
	uint32_t fpscr;
	uint32_t pc;
	
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
	
	uint8_t cr0[4];
	uint8_t cr1[4];
} MachineState;

EXTERN_C void MachineStateInit(MachineState* state);

#undef EXTERN_C

#endif /* defined(__pefdump__MachineState__) */
