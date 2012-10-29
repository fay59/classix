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
