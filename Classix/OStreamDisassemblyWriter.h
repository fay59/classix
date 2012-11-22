//
//  OStreamDisassemblyWriter.h
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__OStreamDisassemblyWriter__
#define __Classix__OStreamDisassemblyWriter__

#include <iostream>
#include <string>
#include "FancyDisassembler.h"

class OStreamDisassemblyWriter : public PPCVM::Disassembly::DisassemblyWriter
{
	std::ostream& into;
	
public:
	OStreamDisassemblyWriter(std::ostream& into);
	
	virtual void EnterSection(const PEF::InstantiableSection& section);
	virtual void EnterLabel(const PPCVM::Disassembly::InstructionRange& label, intptr_t labelAddress);
	virtual void VisitOpcode(const PPCVM::Disassembly::DisassembledOpcode& opcode, intptr_t opcodeAddress, const std::string* metadata);
	
	virtual ~OStreamDisassemblyWriter();
};

#endif /* defined(__Classix__OStreamDisassemblyWriter__) */
