//
//  InterpreterException.cpp
//  Classix
//
//  Created by Félix on 2012-12-01.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "InterpreterException.h"

namespace PPCVM
{
	namespace Execution
	{
		InterpreterException::InterpreterException(uint32_t pc, const Common::PPCRuntimeException& reason)
		: reason(reason.ToHeapAlloc()), pc(pc)
		{ }
		
		uint32_t InterpreterException::GetPC() const
		{
			return pc;
		}
		
		const char* InterpreterException::what() const noexcept
		{
			return reason->what();
		}
		
		InterpreterException::~InterpreterException()
		{
			delete reason;
		}
	}
}
