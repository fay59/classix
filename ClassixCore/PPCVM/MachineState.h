//
//  MachineState.h
//  pefdump
//
//  Created by Félix on 2012-10-26.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__MachineState__
#define __pefdump__MachineState__

#include <cstdint>

namespace PPCVM
{
	struct MachineState
	{
		union
		{
			uint32_t gpr[32];
			struct
			{
				uint32_t r0, r1, r2, r3, r4, r5, r6, r7;
				uint32_t r8, r9, r10, r11, r12, r13, r14, r15;
				uint32_t r16, r17, r18, r19, r20, r21, r22, r23;
				uint32_t r24, r25, r26, r27, r28, r29, r30, r31;
			};
		};
		
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
		
		MachineState();
		
		uint32_t GetCR() const;
		void SetCR(uint32_t cr);
	};
}

#endif /* defined(__pefdump__MachineState__) */
