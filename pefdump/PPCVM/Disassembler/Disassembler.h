//
//  Disassembler.h
//  pefdump
//
//  Created by Félix on 2012-11-02.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__Disassembler__
#define __pefdump__Disassembler__

#include "Instruction.h"
#include <string>
#include <iostream>

namespace PPCVM
{
	namespace Disassembler
	{
		struct DisassembledInstruction
		{
			std::string Opcode;
			std::string Arguments;
			
			DisassembledInstruction();
			DisassembledInstruction(const std::string& opcode, const std::string& argument);
		};
		
		DisassembledInstruction Disassemble(uint32_t instruction);
		DisassembledInstruction Disassemble(Instruction instruction);
		bool Disassemble(uint32_t instruction, DisassembledInstruction& into);
		bool Disassemble(Instruction instruction, DisassembledInstruction& into);
	};
}

std::ostream& operator<<(std::ostream& into, const PPCVM::Disassembler::DisassembledInstruction& inst);

#endif /* defined(__pefdump__Disassembler__) */
