//
// InterpreterException.h
// Classix
//
// Copyright (C) 2012 Félix Cloutier
//
// This file is part of Classix.
//
// Classix is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Classix is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Classix. If not, see http://www.gnu.org/licenses/.
//

#ifndef __Classix__InterpreterException__
#define __Classix__InterpreterException__

#include <memory>
#include <exception>
#include <cstdint>
#include "PPCRuntimeException.h"

namespace PPCVM
{
	namespace Execution
	{
		class InterpreterException : public std::exception
		{
			std::shared_ptr<Common::PPCRuntimeException> reason;
			uint32_t pc;
			
		public:
			InterpreterException(uint32_t pc, const Common::PPCRuntimeException& reason);
			
			uint32_t GetPC() const;
			std::shared_ptr<Common::PPCRuntimeException> GetReason();
			virtual const char* what() const noexcept override;
			
			virtual ~InterpreterException() override;
		};
	}
}

#endif /* defined(__Classix__InterpreterException__) */
