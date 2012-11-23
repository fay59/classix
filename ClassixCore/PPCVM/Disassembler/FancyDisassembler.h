//
// FancyDisassembler.h
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

#ifndef __Classix__FancyDisassembler__
#define __Classix__FancyDisassembler__

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "IAllocator.h"
#include "Container.h"
#include "Disassembler.h"

namespace PPCVM
{
	namespace Disassembly
	{
		class DisassemblyWriter;
		
		class SectionDisassembler
		{
			friend class FancyDisassembler;
			
			Common::IAllocator* allocator;
			const PEF::InstantiableSection& section;
			PPCVM::Disassembly::Disassembler* disasm;
			uint32_t sectionNumber;
			
		public:
			typedef std::unordered_map<intptr_t, std::string> MetadataMap;
			
			SectionDisassembler(Common::IAllocator* allocator, uint32_t sectionNumber, const PEF::InstantiableSection& section);
			SectionDisassembler(const SectionDisassembler& that) = delete;
			SectionDisassembler(SectionDisassembler&& that);
			
			void WriteTo(DisassemblyWriter& writer, MetadataMap metadata) const;
			
			~SectionDisassembler();
		};
		
		class FancyDisassembler
		{
			typedef PPCVM::Disassembly::InstructionRange InstructionRange;
			
			Common::IAllocator* allocator;
			SectionDisassembler::MetadataMap metadata;
			std::map<uint32_t, SectionDisassembler> sections;
			std::unordered_map<InstructionRange*, PPCVM::Disassembly::Disassembler*> rangeToDisasm;
			std::unordered_set<InstructionRange*> unprocessedRanges;
			std::unordered_map<InstructionRange*, const uint8_t*> r2Values;
			
			void DoDisassemble(PEF::Container& container);
			void TryInitR2WithMainSymbol(PEF::Container& container);
			void ProcessRange(PPCVM::Disassembly::InstructionRange& range, const uint8_t* r2);
			void TryFollowBranch(PPCVM::Disassembly::InstructionRange* range, const Common::UInt32* currentAddress, const Common::UInt32 *targetAddress, const uint8_t* r2);
			
		public:
			FancyDisassembler(Common::IAllocator* allocator);
			
			void Disassemble(PEF::Container& container, DisassemblyWriter& writer);
		};
		
		class DisassemblyWriter
		{
		public:
			virtual void EnterSection(const PEF::InstantiableSection& section, uint32_t sectionIndex) = 0;
			virtual void EnterLabel(const InstructionRange& label, intptr_t labelAddress) = 0;
			virtual void VisitOpcode(const DisassembledOpcode& opcode, intptr_t opcodeAddress, const std::string* metadata) = 0;
			
			virtual ~DisassemblyWriter();
		};
	}
}

#endif /* defined(__Classix__FancyDisassembler__) */
