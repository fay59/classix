//
//  Disassembler.cpp
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "Disassembler.h"
#include <cassert>

namespace PPCVM
{
	namespace Disassembly
	{
		Disassembler::Disassembler(Common::IAllocator* allocator, const Common::UInt32* begin, const Common::UInt32* end)
		: begin(begin), end(end)
		{
			assert(begin <= end && "End before begin");
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
				iter->second.SetEnd(labelEnd);
			}
		}
		
		void Disassembler::bx(Common::IAllocator* allocator, const Common::UInt32* address)
		{
			Instruction inst = address->Get();
			address++;
			
			if (inst.LK == 0)
				labels.insert(std::make_pair(address, InstructionRange(allocator, address)));
			
			const Common::UInt32* target = address + inst.LI;
			labels.insert(std::make_pair(target, InstructionRange(allocator, target)));
		}
		
		void Disassembler::bcx(Common::IAllocator *allocator, const Common::UInt32 *address)
		{
			Instruction inst = address->Get();
			address++;
			
			if (inst.LK == 0 && (inst.BO & 0b10100) == 0b10100)
				labels.insert(std::make_pair(address, InstructionRange(allocator, address)));
			
			const Common::UInt32* target = address + inst.BD;
			labels.insert(std::make_pair(target, InstructionRange(allocator, target)));
		}
		
		void Disassembler::bcctrx(Common::IAllocator *allocator, const Common::UInt32 *address)
		{
			Instruction inst = address->Get();
			address++;
			
			if (inst.LK == 0 && (inst.BO & 0b10100) == 0b10100)
				labels.insert(std::make_pair(address, InstructionRange(allocator, address)));
			
			// indirect branch, no idea where this is going.
		}
		
		void Disassembler::bclrx(Common::IAllocator *allocator, const Common::UInt32 *address)
		{
			Instruction inst = address->Get();
			address++;
			
			if (inst.LK == 0 && (inst.BO & 0b10100) == 0b10100)
				labels.insert(std::make_pair(address, InstructionRange(allocator, address)));
			
			// indirect branch, most likely a return
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