//
//  PPCRuntimeException.h
//  Classix
//
//  Created by Félix on 2012-12-01.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__PPCRuntimeException__
#define __Classix__PPCRuntimeException__

#include <exception>

namespace Common
{
	class PPCRuntimeException : public std::exception
	{
	public:
		virtual PPCRuntimeException* ToHeapAlloc() const = 0;
		virtual inline ~PPCRuntimeException() {};
	};
}

#endif /* defined(__Classix__PPCRuntimeException__) */
