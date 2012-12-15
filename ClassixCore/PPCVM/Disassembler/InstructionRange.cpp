//
// InstructionRange.cpp
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

#include "InstructionRange.h"
#include "InstructionDecoder.h"
#include <cassert>
#include <cstring>

namespace PPCVM
{
	namespace Disassembly
	{
		InstructionRange::InstructionRange(Common::IAllocator* allocator, const Common::UInt32* begin)
		: Begin(begin), End(nullptr), allocator(allocator), TableOfContents(nullptr)
		{
			Instruction first = begin->Get();
			IsFunction = InstructionDecoder::Decode(first).Opcode == "mflr";
		}
		
		void InstructionRange::CompleteRange(const Common::UInt32 *base, const Common::UInt32 *end)
		{
			assert(end >= Begin && "End before Begin");
			End = end;
			
			char functionName[] = "00000000";
			sprintf(functionName, "%08x", allocator->ToIntPtr(const_cast<Common::UInt32*>(Begin)));
			Name = functionName;
			
			InstructionDecoder decoder;
			Opcodes.reserve(End - Begin);
			for (auto iter = Begin; iter != End; iter++)
			{
				decoder.Dispatch(PPCVM::Instruction(*iter));
				Opcodes.push_back(decoder.Opcode);
			}
		}
	}
}
