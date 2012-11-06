//
//  NativeCall.h
//  pefdump
//
//  Created by Félix on 2012-11-05.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef pefdump_NativeCall_h
#define pefdump_NativeCall_h

#include "MachineState.h"

namespace PPCVM
{
	namespace Execution
	{
		typedef void (*NativeCall)(MachineState*);
	}
}

#endif
