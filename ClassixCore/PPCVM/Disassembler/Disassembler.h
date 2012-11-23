//
// Disassembler.h
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
			
			void bcx(Common::IAllocator* allocator, const Common::UInt32* address);
			void bx(Common::IAllocator* allocator, const Common::UInt32* address);
			void bcctrx(Common::IAllocator* allocator, const Common::UInt32* address);
			void bclrx(Common::IAllocator* allocator, const Common::UInt32* address);
			
		public:
			typedef std::map<const Common::UInt32*, InstructionRange>::iterator iterator;
			typedef std::map<const Common::UInt32*, InstructionRange>::const_iterator const_iterator;
			
			Disassembler(Common::IAllocator* allocator, const Common::UInt32* begin, const Common::UInt32* end);
			
			InstructionRange* FindRange(const Common::UInt32* address);
			const InstructionRange* FindRange(const Common::UInt32* address) const;
			
			const Common::UInt32* StartAddress() const;
			const Common::UInt32* EndAddress() const;
			
			iterator Begin();
			iterator End();
			const_iterator Begin() const;
			const_iterator End() const;
		};
	}
}

#endif /* defined(__Classix__Disassembler__) */
