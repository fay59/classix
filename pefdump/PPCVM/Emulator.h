//
//  Emulator.h
//  pefdump
//
//  Created by Félix on 2012-10-28.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__Emulator__
#define __pefdump__Emulator__

#include "Executor.h"

namespace PPCVM
{
	class Emulator : public Executor
	{
	public:
		Emulator(MachineState& state, const Common::UInt32* base);
		
		virtual void ExecuteSingleInstruction() = 0;
		virtual void ExecuteFunction() = 0;
		
		virtual ~Emulator();
	};
}

#endif /* defined(__pefdump__Emulator__) */
