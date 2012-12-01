//
//  InterpreterException.h
//  Classix
//
//  Created by Félix on 2012-12-01.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__InterpreterException__
#define __Classix__InterpreterException__

#include <exception>
#include <cstdint>
#include "PPCRuntimeException.h"

namespace PPCVM
{
	namespace Execution
	{
		class InterpreterException
		{
			Common::PPCRuntimeException* reason;
			uint32_t pc;
			
		public:
			InterpreterException(uint32_t pc, const Common::PPCRuntimeException& reason);
			
			uint32_t GetPC() const;
			virtual const char* what() const noexcept;
			
			~InterpreterException();
		};
	}
}

#endif /* defined(__Classix__InterpreterException__) */
