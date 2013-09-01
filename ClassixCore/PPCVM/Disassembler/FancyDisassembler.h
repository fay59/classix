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
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Allocator.h"
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
			
			Common::Allocator& allocator;
			const PEF::InstantiableSection& section;
			std::shared_ptr<PPCVM::Disassembly::Disassembler> disasm;
			uint32_t sectionNumber;
			
		public:
			typedef std::unordered_map<uint32_t, uint32_t> MetadataMap;
			
			SectionDisassembler(Common::Allocator& allocator, uint32_t sectionNumber, const PEF::InstantiableSection& section);
			SectionDisassembler(const SectionDisassembler& that) = delete;
			SectionDisassembler(SectionDisassembler&& that);
			
			void WriteTo(DisassemblyWriter& writer, MetadataMap metadata) const;
		};
		
		class FancyDisassembler
		{
			typedef PPCVM::Disassembly::InstructionRange InstructionRange;
			
			Common::Allocator& allocator;
			SectionDisassembler::MetadataMap metadata;
			std::map<uint32_t, SectionDisassembler> sections;
			std::unordered_map<InstructionRange*, std::shared_ptr<PPCVM::Disassembly::Disassembler>> rangeToDisasm;
			std::unordered_set<InstructionRange*> unprocessedRanges;
			std::unordered_map<InstructionRange*, const uint8_t*> r2Values;
			
			void DoDisassemble(const PEF::Container& container);
			void TryInitR2WithMainSymbol(const PEF::Container& container);
			void ProcessRange(PPCVM::Disassembly::InstructionRange& range, const uint8_t* r2);
			void TryFollowBranch(PPCVM::Disassembly::InstructionRange* range, const Common::UInt32* currentAddress, const Common::UInt32 *targetAddress, const uint8_t* r2);
			
		public:
			FancyDisassembler(Common::Allocator& allocator);
			
			void Disassemble(const PEF::Container& container, DisassemblyWriter& writer);
		};
		
		class DisassemblyWriter
		{
		public:
			virtual void EnterSection(const PEF::InstantiableSection& section, uint32_t sectionIndex) = 0;
			virtual void EnterLabel(const InstructionRange& label, uint32_t labelAddress) = 0;
			virtual void VisitOpcode(const DisassembledOpcode& opcode, uint32_t opcodeAddress, const SectionDisassembler::MetadataMap::mapped_type* metadata) = 0;
			
			virtual ~DisassemblyWriter();
		};
	}
}

#endif /* defined(__Classix__FancyDisassembler__) */
