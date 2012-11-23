//
// OStreamDisassemblyWriter.cpp
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

#include <iomanip>
#include "OStreamDisassemblyWriter.h"

using namespace Common;
using namespace PPCVM::Disassembly;

static char endline = '\n';

OStreamDisassemblyWriter::OStreamDisassemblyWriter(std::ostream& into)
: into(into)
{ }

void OStreamDisassemblyWriter::EnterSection(const PEF::InstantiableSection& section, uint32_t sectionIndex)
{
	into << "Section " << section.Name << " (" << sectionIndex << "):" << endline;
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
