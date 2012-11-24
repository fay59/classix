//
// FancyDisassembler.cpp
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
#include <unordered_set>
#include <cassert>
#include <dlfcn.h>

#include "FancyDisassembler.h"
#include "NativeCall.h"

using namespace PEF;
using PPCVM::Execution::NativeCall;
using Common::UInt32;

namespace
{
	inline const UInt32* SectionBegin(const InstantiableSection& section)
	{
		return reinterpret_cast<UInt32*>(section.Data);
	}
	
	inline const UInt32* SectionEnd(const InstantiableSection& section)
	{
		return SectionBegin(section) + section.Size() / 4;
	}
	
	const char endline = '\n';
}

namespace PPCVM
{
	namespace Disassembly
	{
#pragma mark -
#pragma mark Section Disassembler
		
		SectionDisassembler::SectionDisassembler(Common::IAllocator* allocator, uint32_t sectionNumber, const InstantiableSection& section)
		: allocator(allocator), section(section)
		{
			disasm = new Disassembler(allocator, SectionBegin(section), SectionEnd(section));
			this->sectionNumber = sectionNumber;
		}
		
		SectionDisassembler::SectionDisassembler(SectionDisassembler&& that)
		: allocator(that.allocator), section(that.section), sectionNumber(that.sectionNumber)
		{
			disasm = that.disasm;
			that.disasm = nullptr;
		}
		
		void SectionDisassembler::WriteTo(DisassemblyWriter& writer, MetadataMap metadata) const
		{
			writer.EnterSection(section, sectionNumber);
			
			for (auto iter = disasm->Begin(); iter != disasm->End(); iter++)
			{
				auto& section = iter->second;
				writer.EnterLabel(section, allocator->ToIntPtr(section.Begin));
				
				for (int i = 0; i < section.Opcodes.size(); i++)
				{
					intptr_t opcodeAddress = allocator->ToIntPtr(section.Begin + i);
					MetadataMap::iterator iter = metadata.find(opcodeAddress);
					const std::string* stringMetadata = iter == metadata.end() ? nullptr : &iter->second;
					writer.VisitOpcode(section.Opcodes[i], opcodeAddress, stringMetadata);
				}
			}
		}
		
		SectionDisassembler::~SectionDisassembler()
		{
			delete disasm;
		}
		
#pragma mark -
#pragma mark Fancy Disassembler
		FancyDisassembler::FancyDisassembler(Common::IAllocator* allocator)
		: allocator(allocator)
		{ }
		
		void FancyDisassembler::DoDisassemble(const Container &container)
		{
			for (uint32_t i = 0; i < container.Size(); i++)
			{
				const InstantiableSection& section = container.GetSection(i);
				if (section.GetSectionType() != SectionType::Code && section.GetSectionType() != SectionType::ExecutableData)
					continue;
				
				SectionDisassembler sectionDisasm(allocator, i, section);
				Disassembler* disasm = sectionDisasm.disasm;
				for (auto iter = disasm->Begin(); iter != disasm->End(); iter++)
					rangeToDisasm[&iter->second] = disasm;
				
				sections.insert(std::make_pair(i, std::move(sectionDisasm)));
			}
		}
		
		void FancyDisassembler::TryInitR2WithMainSymbol(const Container& container)
		{
			const LoaderSection* loaderSection = container.LoaderSection();
			const auto& mainLocation = loaderSection->Header->Main;
			const InstantiableSection& mainSection = container.GetSection(mainLocation.Section);
			// only search for a transition vector inside data segments
			switch (mainSection.GetSectionType())
			{
				case SectionType::UnpackedData:
				case SectionType::PatternInitializedData:
				case SectionType::Constant:
				{
					const uint8_t* vectorAddress = mainSection.Data + mainLocation.Offset;
					const TransitionVector* vector = reinterpret_cast<const TransitionVector*>(vectorAddress);
					const UInt32* realAddress = allocator->ToPointer<UInt32>(vector->EntryPoint);
					
					// find in which section the real address belongs
					for (auto& pair : sections)
					{
						if (auto range = pair.second.disasm->FindRange(realAddress))
						{
							r2Values[range] = allocator->ToPointer<uint8_t>(vector->TableOfContents);
							break;
						}
					}
				}
					
				default: break;
			}
		}
		
		void FancyDisassembler::ProcessRange(PPCVM::Disassembly::InstructionRange& range, const uint8_t *r2)
		{
			uint32_t r12 = 0;
			for (size_t i = 0; i < range.Opcodes.size(); i++)
			{
				auto& opcode = range.Opcodes[i];
				// if it's a static branch, resolve it and propagate r2
				if (opcode.Instruction.OPCD == 16 || opcode.Instruction.OPCD == 18)
				{
					const UInt32* targetAddress = range.Begin + i + opcode.Arguments.back().Value / 4;
					TryFollowBranch(&range, range.Begin + i, targetAddress, r2);
				}
				// otherwise, only act when r2 is not null
				else if (r2 != nullptr)
				{
					// assume that each time we lwz something from r2 into r12 we're dealing with a transition vector
					if (opcode.Opcode == "lwz")
					{
						if (opcode.Arguments[0].IsGPR(12) && opcode.Arguments[2].IsGPR(2))
						{
							int32_t offset = opcode.Arguments[1].Value;
							r12 = *reinterpret_cast<const UInt32*>(r2 + offset);
						}
					}
					else if (opcode.Opcode == "bctr")
					{
						const TransitionVector* target = allocator->ToPointer<TransitionVector>(r12);
						const NativeCall* native = allocator->ToPointer<NativeCall>(target->EntryPoint);
						if (native->Tag == PPCVM::Execution::NativeTag)
						{
							// if it's a native call, add the function name as metadata
							Dl_info info;
							if (dladdr(reinterpret_cast<void*>(native->Callback), &info))
							{
								intptr_t opcodeAddress = allocator->ToIntPtr(range.Begin + i);
								metadata.insert(std::make_pair(opcodeAddress, info.dli_sname));
							}
						}
						else
						{
							// this probably points to another section of the executable.
							// I'm not too sure how it works because most applications only have
							// one code section.
							const UInt32* targetLabel = allocator->ToPointer<UInt32>(target->EntryPoint);
							const uint8_t* targetToc = allocator->ToPointer<uint8_t>(target->TableOfContents);
							
							// For now, let's hope that this points to the same executable section.
							TryFollowBranch(&range, range.Begin + i, targetLabel, targetToc);
						}
					}
				}
			}
		}
		
		void FancyDisassembler::TryFollowBranch(InstructionRange* range, const UInt32* currentAddress, const UInt32 *targetAddress, const uint8_t* r2)
		{
			if (auto targetRange = rangeToDisasm[range]->FindRange(targetAddress))
			{
				intptr_t address = allocator->ToIntPtr(currentAddress);
				metadata.insert(std::make_pair(address, targetRange->Name));
				if (r2 != nullptr && unprocessedRanges.count(targetRange))
					r2Values.insert(std::make_pair(targetRange, r2));
			}
		}
		
		void FancyDisassembler::Disassemble(const Container &container, DisassemblyWriter& writer)
		{
			metadata.clear();
			sections.clear();
			rangeToDisasm.clear();
			unprocessedRanges.clear();
			r2Values.clear();
			
			DoDisassemble(container);
			
			// run through disassembly to add metadata where it belongs
			
			for (auto& pair : sections)
			{
				auto& disasm = pair.second.disasm;
				for (auto iter = disasm->Begin(); iter != disasm->End(); iter++)
					unprocessedRanges.insert(&iter->second);
			}
			
			// first try to process all instruction ranges with a known table of contents;
			// this will only work if we start with a transition vector
			TryInitR2WithMainSymbol(container);
			
			// now, process all instruction ranges for which we have a known table of contents
			while (r2Values.size() > 0)
			{
				auto iter = r2Values.begin();
				auto& pair = *iter;
				ProcessRange(*pair.first, pair.second);
				
				unprocessedRanges.erase(pair.first);
				r2Values.erase(iter);
			}
			
			// now, process all remaining ranges: we cannot resolve indirect branches, but we can still resolve static branches
			while (unprocessedRanges.size() > 0)
			{
				auto iter = unprocessedRanges.begin();
				ProcessRange(**iter, nullptr);
				unprocessedRanges.erase(iter);
			}
			
			// and finally, print all sections
			for (auto& pair : sections)
				pair.second.WriteTo(writer, metadata);
		}
		
		DisassemblyWriter::~DisassemblyWriter()
		{ }
	}
}