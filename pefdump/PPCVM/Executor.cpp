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
	NativeCallVector::NativeCallVector(NativeCallback callback)
	{
		signature = 0xffffffff;
		this->callback = callback;
	}
	
	Executor::Executor(MachineState& state)
	: state(state)
	{ }
	
	void Executor::ExecuteNativeFunction(const PPCVM::NativeCallVector *vector)
	{
		assert(vector->signature == 0xffffffff);
		vector->callback(&state);
	}
	
	Executor::~Executor()
	{ }
}
