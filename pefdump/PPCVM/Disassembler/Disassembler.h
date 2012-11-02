//
//  Disassembler.h
//  pefdump
//
//  Created by Félix on 2012-11-02.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__Disassembler__
#define __pefdump__Disassembler__

#include <string>

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
		bool Disassemble(uint32_t instruction, DisassembledInstruction& into);
	};
}

#endif /* defined(__pefdump__Disassembler__) */