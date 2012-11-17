//
//  DisassembledOpcode.h
//  Classix
//
//  Created by Félix on 2012-11-17.
//  Copyright (c) 2012 Félix. All rights reserved.
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
