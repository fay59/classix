//
// InterpreterException.cpp
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
