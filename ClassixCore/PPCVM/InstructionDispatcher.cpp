//
//  InstructionDispatcher.cpp
//  Classix
//
//  Created by Félix on 2012-11-16.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <iostream>
#include "Disassembler.h"

namespace PPCVM
{
	void DisassembleIfAsked(Instruction inst)
	{
		if (getenv("DEBUG_DISASSEMBLE"))
		{
			std::cerr << '\t' << Disassembly::InstructionDecoder::Decode(inst) << std::endl;
		}
	}
}
