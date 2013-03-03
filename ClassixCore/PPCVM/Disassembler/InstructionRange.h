//
// InstructionRange.h
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

#ifndef __Classix__InstructionRange__
#define __Classix__InstructionRange__

#include <string>
#include <vector>
#include <iostream>

#include "InstructionDispatcher.h"
#include "BigEndian.h"
#include "IAllocator.h"
#include "Structures.h"
#include "DisassembledOpcode.h"

namespace PPCVM
{
	namespace Disassembly
	{
		class InstructionRange
		{
			Common::IAllocator& allocator;
			
		public:
			std::string Name;
			bool IsFunction;
			const Common::UInt32* Begin;
			const Common::UInt32* End;
			const Common::UInt32* TableOfContents;
			std::vector<DisassembledOpcode> Opcodes;
			
			InstructionRange(Common::IAllocator& allocator, const Common::UInt32* begin);
			
			void CompleteRange(const Common::UInt32* base, const Common::UInt32* end);
		};
	}
}

#endif /* defined(__Classix__InstructionRange__) */
