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

OStreamDisassemblyWriter::OStreamDisassemblyWriter(Common::IAllocator& allocator, std::ostream& into)
: into(into), allocator(allocator)
{ }

void OStreamDisassemblyWriter::EnterSection(const PEF::InstantiableSection& section, uint32_t sectionIndex)
{
	into << "Section \"" << section.Name << "\" (" << sectionIndex << "):" << endline;
}

void OStreamDisassemblyWriter::EnterLabel(const PPCVM::Disassembly::InstructionRange &label, uint32_t labelAddress)
{
	if (label.Begin != label.End)
	{
		if (label.IsFunction)
		{
			into << endline << "\t\t.fn";
		}
		else
		{
			into << "\t\t.lb";
		}
		
		into << std::setw(8) << std::right << std::setfill('0') << std::hex;
		into << labelAddress << ":" << endline;
	}
}

void OStreamDisassemblyWriter::VisitOpcode(const PPCVM::Disassembly::DisassembledOpcode& opcode, uint32_t opcodeAddress, const PPCVM::Disassembly::SectionDisassembler::MetadataMap::mapped_type* metadata)
{
	into << std::setw(8) << std::setfill('0') << std::right << std::hex << opcodeAddress << ' ';
	into << '\t' << std::setw(12) << std::setfill(' ') << std::left << opcode.Opcode;
	into << std::setw(28) << opcode.ArgumentsString();
	if (metadata != nullptr)
	{
		into << '<';
		uint32_t relatedAddress = *metadata;
		if (opcode.Opcode[0] == 'b')
		{
			into << (opcode.Opcode.back() == 'l' ? ".fn" : ".lb");
			into << std::hex << std::setw(8) << std::right << std::setfill('0');
			into << relatedAddress;
		}
		else if (relatedAddress == 0)
		{
			into << "missing symbol";
		}
		else
		{
			try
			{
				uint32_t offset = allocator.GetAllocationOffset(relatedAddress);
				uint32_t base = relatedAddress - offset;
				into << allocator.GetDetails(base)->GetAllocationDetails(offset);
			}
			catch (Common::AccessViolationException& ex)
			{
				into << "access violation";
			}
		}
		into << '>';
	}
	
	into << endline;
}

OStreamDisassemblyWriter::~OStreamDisassemblyWriter()
{ }
