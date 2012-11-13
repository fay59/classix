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
					case 16: bcx(instruction); break;
					case 18: bx(instruction); break;
					case 19:
						switch (instruction.SUBOP10)
						{
							case 16: bclrx(instruction); break;
							case 528: bcctrx(instruction); break;
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