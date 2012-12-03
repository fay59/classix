//
//  PanicException.h
//  Classix
//
//  Created by Félix on 2012-12-02.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__PanicException__
#define __Classix__PanicException__

#include "PPCRuntimeException.h"
#include <string>

namespace PPCVM
{
	namespace Execution
	{
		class PanicException : public Common::PPCRuntimeException
		{
			std::string message;
			
		public:
			PanicException(const std::string& message);
			
			virtual PPCRuntimeException* ToHeapAlloc() const override;
			virtual const char* what() const noexcept override;
			virtual ~PanicException() override;
		};
	}
}

#endif /* defined(__Classix__PanicException__) */
