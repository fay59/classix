//
//  MachineState.cpp
//  pefdump
//
//  Created by Félix on 2012-10-26.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "MachineState.h"
#include <cstring>

namespace PPCVM
{
	MachineState::MachineState()
	{
		memset(gpr, 0, sizeof gpr);
		memset(fpr, 0, sizeof fpr);
		lr = ctr = 0;
		cr = xer = 0;
		fpscr = 0;
		pc = 0;
	}
}
