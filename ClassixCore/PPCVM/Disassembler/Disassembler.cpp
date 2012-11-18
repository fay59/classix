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
		
		void Disassembler::bx(Common::IAllocator* allocator, const Common::UInt32* address)
		{
			Instruction inst = address->Get();
			if (inst.LK == 0)
			{
				const Common::UInt32* lr = address + 1;
				labels.insert(std::make_pair(lr, InstructionRange(allocator, lr)));
			}
			
			const Common::UInt32* target = inst.AA
				? allocator->ToPointer<const Common::UInt32>(inst.LI << 2)
				: address + inst.LI;
			auto iter = labels.insert(std::make_pair(target, InstructionRange(allocator, target))).first;
			iter->second.IsFunction |= inst.LK;
		}
		
		void Disassembler::bcx(Common::IAllocator *allocator, const Common::UInt32 *address)
		{
			Instruction inst = address->Get();
			if (inst.LK == 0 && (inst.BO & 0b10100) == 0b10100)
			{
				const Common::UInt32* lr = address + 1;
				labels.insert(std::make_pair(lr, InstructionRange(allocator, lr)));
			}
			
			const Common::UInt32* target = inst.AA
				? allocator->ToPointer<const Common::UInt32>(inst.BD << 2)
				: address + inst.BD;
			auto iter = labels.insert(std::make_pair(target, InstructionRange(allocator, target))).first;
			iter->second.IsFunction |= inst.LK;
		}
		
		void Disassembler::bcctrx(Common::IAllocator *allocator, const Common::UInt32 *address)
		{
			Instruction inst = address->Get();
			if (inst.LK == 0 && (inst.BO & 0b10100) == 0b10100)
			{
				const Common::UInt32* lr = address + 1;
				labels.insert(std::make_pair(lr, InstructionRange(allocator, lr)));
			}
			
			// indirect branch, no idea where this is going.
		}
		
		void Disassembler::bclrx(Common::IAllocator *allocator, const Common::UInt32 *address)
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