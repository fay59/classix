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
#include "MachineState.h"

namespace PPCVM
{
	typedef void (*NativeCallback)(MachineState* state);
	
	struct NativeCallVector
	{
		uint32_t signature; // 0xffffffff
		NativeCallback callback;
		
		NativeCallVector(NativeCallback callback);
	};
	
	class Executor
	{
	protected:
		MachineState& state;
		
	public:
		Executor(MachineState& state);
		
		virtual void ExecuteSingleInstruction(uint32_t instruction) = 0;
		virtual void ExecuteFunction(const void* functionAddress) = 0;
		void ExecuteNativeFunction(const NativeCallVector* vector);
		
		virtual ~Executor();
	};
}

#endif /* defined(__pefdump__Executor__) */
