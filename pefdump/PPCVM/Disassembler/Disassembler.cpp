//
//  Disassembler.cpp
//  pefdump
//
//  Created by Félix on 2012-11-02.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "Disassembler.h"

extern "C" {
#include "ppc_disasm.h"
}

PPCVM::Disassembler::DisassembledInstruction::DisassembledInstruction() {}

PPCVM::Disassembler::DisassembledInstruction::DisassembledInstruction(const std::string& opcode, const std::string& operands)
: Opcode(opcode), Arguments(operands)
{ }

PPCVM::Disassembler::DisassembledInstruction PPCVM::Disassembler::Disassemble(Instruction instruction)
{
	return Disassemble(instruction.hex);
}

PPCVM::Disassembler::DisassembledInstruction PPCVM::Disassembler::Disassemble(uint32_t instruction)
{
	DisassembledInstruction disassembly;
	if (Disassemble(instruction, disassembly))
		return disassembly;
	
	throw std::logic_error("could not disassemble instruction");
}

bool PPCVM::Disassembler::Disassemble(Instruction instruction, DisassembledInstruction& into)
{
	return Disassemble(instruction.hex, into);
}

bool PPCVM::Disassembler::Disassemble(uint32_t instruction, DisassembledInstruction& into)
{
	// arrays sized accordingly to ppc_disasm.h
	char opcodeBuffer[10];
	char operandsBuffer[24];
	
	DisasmPara_PPC params;
	params.instr = &instruction;
	params.iaddr = &instruction;
	params.opcode = opcodeBuffer;
	params.operands = operandsBuffer;
	
	void* result = PPC_Disassemble(&params);
	if (result == nullptr)
		return false;
	
	into.Opcode = opcodeBuffer;
	into.Arguments = operandsBuffer;
	return true;
}
