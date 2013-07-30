//
// Disassembler.cpp
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

#include "Disassembler.h"
#include <cassert>

namespace PPCVM
{
	namespace Disassembly
	{
		Disassembler::Disassembler(Common::Allocator& allocator, const Common::UInt32* begin, const Common::UInt32* end)
		: begin(begin), end(end)
		{
			assert(begin <= end && "End before begin");
			
			labels.insert(std::make_pair(begin, InstructionRange(allocator, begin)));
			for (auto iter = begin; iter != end; iter++)
			{
				Instruction instruction = iter->Get();
				switch (instruction.OPCD)
				{
					case 16: bcx(allocator, iter); break;
					case 18: bx(allocator, iter); break;
					case 19:
						switch (instruction.SUBOP10)
						{
							case 16: bclrx(allocator, iter); break;
							case 528: bcctrx(allocator, iter); break;
						}
						break;
				}
			}
			
			for (auto iter = Begin(); iter != End(); iter++)
			{
				auto next = iter;
				next++;
				
				const Common::UInt32* labelEnd = next == End() ? end : next->first;
				iter->second.CompleteRange(begin, labelEnd);
			}
		}
		
		void Disassembler::bx(Common::Allocator& allocator, const Common::UInt32* address)
		{
			Instruction inst = address->Get();
			if (inst.LK == 0)
			{
				const Common::UInt32* lr = address + 1;
				labels.insert(std::make_pair(lr, InstructionRange(allocator, lr)));
			}
			
			try
			{
				const Common::UInt32* target = inst.AA
					? allocator.ToPointer<Common::UInt32>(inst.LI << 2)
					: allocator.Bless(address + inst.LI);
				auto iter = labels.insert(std::make_pair(target, InstructionRange(allocator, target))).first;
				iter->second.IsFunction |= inst.LK;
			}
			catch (Common::AccessViolationException& ex)
			{ }
		}
		
		void Disassembler::bcx(Common::Allocator& allocator, const Common::UInt32 *address)
		{
			Instruction inst = address->Get();
			if (inst.LK == 0 && (inst.BO & 0b10100) == 0b10100)
			{
				const Common::UInt32* lr = address + 1;
				labels.insert(std::make_pair(lr, InstructionRange(allocator, lr)));
			}
			
			try
			{
				const Common::UInt32* target = inst.AA
					? allocator.ToPointer<Common::UInt32>(inst.BD << 2)
					: allocator.Bless(address + inst.BD);
				auto iter = labels.insert(std::make_pair(target, InstructionRange(allocator, target))).first;
				iter->second.IsFunction |= inst.LK;
			}
			catch (Common::AccessViolationException& ex)
			{ }
		}
		
		void Disassembler::bcctrx(Common::Allocator& allocator, const Common::UInt32 *address)
		{
			Instruction inst = address->Get();
			if (inst.LK == 0 && (inst.BO & 0b10100) == 0b10100)
			{
				const Common::UInt32* lr = address + 1;
				labels.insert(std::make_pair(lr, InstructionRange(allocator, lr)));
			}
			
			// indirect branch, no idea where this is going.
		}
		
		void Disassembler::bclrx(Common::Allocator& allocator, const Common::UInt32 *address)
		{
			Instruction inst = address->Get();
			if (inst.LK == 0 && (inst.BO & 0b10100) == 0b10100)
			{
				const Common::UInt32* lr = address + 1;
				labels.insert(std::make_pair(lr, InstructionRange(allocator, lr)));
			}
			
			// indirect branch, most likely a return
		}
		
		InstructionRange* Disassembler::FindRange(const Common::UInt32 *address)
		{
			auto iter = labels.find(address);
			if (iter != labels.end())
				return &iter->second;
			
			for (auto& range : labels)
			{
				if (range.first > address)
					break;
				
				if (range.second.End < address)
					continue;
				
				return &range.second;
			}
			
			return nullptr;
		}
		
		const InstructionRange* Disassembler::FindRange(const Common::UInt32 *address) const
		{
			auto iter = labels.find(address);
			if (iter != labels.end())
				return &iter->second;
			
			for (auto& range : labels)
			{
				if (range.first > address)
					break;
				
				if (range.second.End < address)
					continue;
				
				return &range.second;
			}
			
			return nullptr;
		}
		
		const Common::UInt32* Disassembler::StartAddress() const
		{
			return begin;
		}
		
		const Common::UInt32* Disassembler::EndAddress() const
		{
			return end;
		}
		
		Disassembler::iterator Disassembler::Begin()
		{
			return labels.begin();
		}
		
		Disassembler::iterator Disassembler::End()
		{
			return labels.end();
		}
		
		Disassembler::const_iterator Disassembler::Begin() const
		{
			return labels.begin();
		}
		
		Disassembler::const_iterator Disassembler::End() const
		{
			return labels.end();
		}
	}
}