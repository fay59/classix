//
//  MachineState.cpp
//  pefdump
//
//  Created by Félix on 2012-10-26.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "MachineState.h"
#include <cstring>

extern "C" void MachineStateInit(MachineState* state)
{
	memset(state, 0, sizeof *state);
}

extern "C" void MachineStateSetCR(MachineState* state, uint32_t value)
{
	for (int i = 0; i < 8; i++)
	{
		state->cr[i] = (value >> (28 - i * 4)) & 0xf;
	}
}

extern "C" uint32_t MachineStateGetCR(MachineState* state)
{
	uint32_t cr = state->cr[0] << 28;
	for (int i = 0; i < 8; i++)
	{
		cr |= state->cr[i] << (28 - i * 4);
	}
	return cr;
}
