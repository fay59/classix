//
//  Executor.h
//  pefdump
//
//  Created by Félix on 2012-10-28.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__Executor__
#define __pefdump__Executor__

#include <cstdint>
#include "BigEndian.h"
#include "MachineState.h"

namespace PPCVM
{
	typedef void (*NativeCall)(MachineState*);
	
	class Executor
	{
	protected:
		MachineState& state;
		const Common::UInt32* base;
		
	public:
		Executor(MachineState& state, const Common::UInt32* base);
		
		virtual void ExecuteSingleInstruction() = 0;
		virtual void ExecuteFunction() = 0;
		
		virtual ~Executor();
	};
}

#endif /* defined(__pefdump__Executor__) */
