//
// OStreamDisassemblyWriter.h
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
	
	virtual void EnterSection(const PEF::InstantiableSection& section, uint32_t sectionIndex) override;
	virtual void EnterLabel(const PPCVM::Disassembly::InstructionRange& label, uint32_t labelAddress) override;
	virtual void VisitOpcode(const PPCVM::Disassembly::DisassembledOpcode& opcode, uint32_t opcodeAddress, const PPCVM::Disassembly::SectionDisassembler::MetadataMap::mapped_type* metadata) override;
	
	virtual ~OStreamDisassemblyWriter() override;
};

#endif /* defined(__Classix__OStreamDisassemblyWriter__) */
