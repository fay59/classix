//
//  Executor.cpp
//  pefdump
//
//  Created by Félix on 2012-10-28.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "Executor.h"
#include <cassert>

namespace PPCVM
{
	Executor::Executor(MachineState& state, const Common::UInt32* base)
	: state(state), base(base)
	{ }
	
	Executor::~Executor()
	{ }
}
