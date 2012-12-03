//
//  PanicException.cpp
//  Classix
//
//  Created by Félix on 2012-12-02.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "PanicException.h"

namespace PPCVM
{
	namespace Execution
	{
		PanicException::PanicException(const std::string& message)
		: message(message)
		{ }
		
		Common::PPCRuntimeException* PanicException::ToHeapAlloc() const
		{
			return new PanicException(*this);
		}
		
		const char* PanicException::what() const noexcept
		{
			return message.c_str();
		}
		
		PanicException::~PanicException()
		{ }
	}
}
