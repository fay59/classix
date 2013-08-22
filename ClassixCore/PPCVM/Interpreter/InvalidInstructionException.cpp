//
// InvalidInstructionException.cpp
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

#include "InvalidInstructionException.h"
#include "Disassembler.h"
#include "InstructionDecoder.h"
#include <sstream>
#include <iomanip>

namespace PPCVM
{
	namespace Execution
	{
		InvalidInstructionException::InvalidInstructionException(Instruction inst)
		: inst(inst)
		{
			std::stringstream ss;
			auto opcode = Disassembly::InstructionDecoder::Decode(inst);
			ss << "Instruction not implemented: "
				<< std::hex << std::setw(8) << std::setfill('0') << inst.hex
				<< ' ' << opcode;
			message = ss.str();
		}
		
		Instruction InvalidInstructionException::GetInstruction() const
		{
			return inst;
		}
		
		Common::PPCRuntimeException* InvalidInstructionException::ToHeapAlloc() const
		{
			return new InvalidInstructionException(*this);
		}
		
		const char* InvalidInstructionException::what() const noexcept
		{
			return message.c_str();
		}
	}
}
