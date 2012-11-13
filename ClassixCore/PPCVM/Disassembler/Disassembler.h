//
//  Disassembler.h
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__Disassembler__
#define __Classix__Disassembler__

#include "BigEndian.h"
#include "IAllocator.h"
#include "Instruction.h"
#include "InstructionRange.h"
#include <map>

namespace PPCVM
{
	namespace Disassembly
	{
		class Disassembler
		{
			const Common::UInt32* begin;
			const Common::UInt32* end;
			
			std::map<const Common::UInt32*, InstructionRange> labels;
			
			void bcx(Instruction inst);
			void bx(Instruction inst);
			void bcctrx(Instruction inst);
			void bclrx(Instruction inst);
			
		public:
			typedef std::map<const Common::UInt32*, InstructionRange>::iterator iterator;
			typedef std::map<const Common::UInt32*, InstructionRange>::const_iterator const_iterator;
			
			Disassembler(Common::IAllocator* allocator, const Common::UInt32* begin, const Common::UInt32* end);
			
			iterator Begin();
			iterator End();
			const_iterator Begin() const;
			const_iterator End() const;
		};
	}
}

#endif /* defined(__Classix__Disassembler__) */
