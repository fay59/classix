//
// DisassembledOpcode.h
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

#ifndef __Classix__DisassembledOpcode__
#define __Classix__DisassembledOpcode__

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include "Instruction.h"

namespace PPCVM
{
	namespace Disassembly
	{
		enum class OpcodeArgumentFormat
		{
			Null,
			GPR,
			FPR,
			SPR,
			CR,
			Offset,
			Literal
		};
		
		struct OpcodeArgument
		{
			OpcodeArgumentFormat Format;
			int32_t Value;
			
			static OpcodeArgument GPR(uint8_t gpr);
			static OpcodeArgument FPR(uint8_t fpr);
			static OpcodeArgument SPR(uint16_t spr);
			static OpcodeArgument CR(uint8_t cr);
			static OpcodeArgument Offset(int32_t offset);
			static OpcodeArgument Literal(int32_t literal);
			
			OpcodeArgument();
			OpcodeArgument(OpcodeArgumentFormat format, int32_t value);
			
			std::string ToString() const;
			
			inline bool IsGPR(uint8_t gpr) { return Format == OpcodeArgumentFormat::GPR && Value == gpr; }
			inline bool IsFPR(uint8_t fpr) { return Format == OpcodeArgumentFormat::FPR && Value == fpr; }
			inline bool IsSPR(uint16_t spr) { return Format == OpcodeArgumentFormat::SPR && Value == spr; }
			inline bool IsCR(uint8_t cr) { return Format == OpcodeArgumentFormat::CR && Value == cr; }
		};
		
		struct DisassembledOpcode
		{
			Instruction Instruction;
			std::string Opcode;
			std::vector<OpcodeArgument> Arguments;
			
			DisassembledOpcode();
			
			template<typename... TParams>
			DisassembledOpcode(union Instruction instruction, const std::string& opcode, TParams... arguments)
			: Instruction(instruction), Opcode(opcode), Arguments({arguments...})
			{ }
			
			std::string ArgumentsString() const;
		};
		
		std::ostream& operator<<(std::ostream& into, const DisassembledOpcode& opcode);
	}
}

#endif /* defined(__Classix__DisassembledOpcode__) */
