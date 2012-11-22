//
//  OStreamDisassemblyWriter.cpp
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <iomanip>
#include "OStreamDisassemblyWriter.h"

using namespace Common;
using namespace PPCVM::Disassembly;

static char endline = '\n';

OStreamDisassemblyWriter::OStreamDisassemblyWriter(std::ostream& into)
: into(into)
{ }

void OStreamDisassemblyWriter::EnterSection(const PEF::InstantiableSection& section)
{
	into << "Section " << section.Name << ":" << endline;
}

void OStreamDisassemblyWriter::EnterLabel(const PPCVM::Disassembly::InstructionRange &label, intptr_t labelAddress)
{
	if (label.Begin != label.End)
	{
		into << "\t\t" << (label.IsFunction ? ".fn" : ".lb");
		into << std::setw(8) << std::right << std::setfill('0') << std::hex;
		into << labelAddress << ":" << endline;
	}
}

void OStreamDisassemblyWriter::VisitOpcode(const PPCVM::Disassembly::DisassembledOpcode &opcode, intptr_t opcodeAddress, const std::string *metadata)
{
	into << std::setw(8) << std::setfill('0') << std::right << std::hex << opcodeAddress << ' ';
	into << '\t' << std::setw(12) << std::setfill(' ') << std::left << opcode.Opcode;
	into << std::setw(24) << opcode.ArgumentsString();
	if (metadata != nullptr)
		into << '<' << *metadata << '>';
	
	into << endline;
}

OStreamDisassemblyWriter::~OStreamDisassemblyWriter()
{ }
