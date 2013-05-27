//
// DisassembledOpcode.cpp
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

#include "DisassembledOpcode.h"
#include <cassert>
#include <sstream>

namespace
{
	const char* sprList[1024] = {
		[1] = "xer",
		[4] = "rtcu",
		[5] = "rtcl",
		[8] = "lr",
		[9] = "ctr",
		[18] = "dsisr",
		[19] = "dar",
		[22] = "dec",
		[25] = "sdr1",
		[26] = "srr0",
		[27] = "srr1",
		[272] = "sprg0",
		[273] = "sprg1",
		[274] = "sprg2",
		[275] = "sprg3",
		[282] = "ear",
		[287] = "pvr",
		[528] = "ibat0u",
		[529] = "ibat0l",
		[530] = "ibat1u",
		[531] = "ibat1l",
		[532] = "ibat2u",
		[533] = "ibat2l",
		[534] = "ibat3u",
		[535] = "ibat3l",
		[536] = "dbat0u",
		[537] = "dbat0l",
		[538] = "dbat1u",
		[539] = "dbat1l",
		[540] = "dbat2u",
		[541] = "dbat2l",
		[542] = "dbat3u",
		[543] = "dbat3l",
		[1013] = "dabr",
	};
	
	template<typename TIter>
	void PrintOpcodeArgument(std::ostream& into, TIter& iter, TIter end)
	{
		assert(iter != end && "at end of collection");
		into << iter->ToString();
		if (iter->Format == PPCVM::Disassembly::OpcodeArgumentFormat::Offset)
		{
			iter++;
			assert(iter != end && "at end of collection");
			into << '(' << iter->ToString() << ')';
		}
	}
}

namespace PPCVM
{
	namespace Disassembly
	{
		OpcodeArgument OpcodeArgument::GPR(uint8_t gpr)
		{
			return OpcodeArgument(OpcodeArgumentFormat::GPR, gpr);
		}
		
		OpcodeArgument OpcodeArgument::FPR(uint8_t gpr)
		{
			return OpcodeArgument(OpcodeArgumentFormat::FPR, gpr);
		}
		
		OpcodeArgument OpcodeArgument::SPR(uint16_t spr)
		{
			return OpcodeArgument(OpcodeArgumentFormat::SPR, spr);
		}
		
		OpcodeArgument OpcodeArgument::CR(uint8_t cr)
		{
			return OpcodeArgument(OpcodeArgumentFormat::CR, cr);
		}
		
		OpcodeArgument OpcodeArgument::Offset(int32_t offset)
		{
			return OpcodeArgument(OpcodeArgumentFormat::Offset, offset);
		}
		
		OpcodeArgument OpcodeArgument::Literal(int32_t gpr)
		{
			return OpcodeArgument(OpcodeArgumentFormat::Literal, gpr);
		}
		
		OpcodeArgument::OpcodeArgument()
		: Format(OpcodeArgumentFormat::Null), Value(0)
		{ }
		
		OpcodeArgument::OpcodeArgument(OpcodeArgumentFormat format, int32_t value)
		: Format(format), Value(value)
		{ }
		
		std::string OpcodeArgument::ToString() const
		{
			switch (Format)
			{
				case OpcodeArgumentFormat::Null: return "";
					
				case OpcodeArgumentFormat::GPR:
				{
					char gpr[] = "r32";
					sprintf(gpr + 1, "%hhu", static_cast<uint8_t>(Value));
					return gpr;
				}
					
				case OpcodeArgumentFormat::FPR:
				{
					char fpr[] = "fr32";
					sprintf(fpr + 2, "%hhu", static_cast<uint8_t>(Value));
					return fpr;
				}
					
				case OpcodeArgumentFormat::SPR:
				{
					uint16_t value = static_cast<uint16_t>(Value);
					if (const char* sprName = sprList[value])
						return sprName;
					
					char buffer[] = "spr1024";
					sprintf(buffer + 3, "%hu", value);
					return buffer;
				}
					
				case OpcodeArgumentFormat::CR:
				{
					char cr[] = "cr8";
					cr[2] = static_cast<char>('0' + Value);
					return cr;
				}
					
				case OpcodeArgumentFormat::Offset:
				{
					char offset[] = "2147483648";
					sprintf(offset, "%i", static_cast<int32_t>(Value));
					return offset;
				}
					
				case OpcodeArgumentFormat::Literal:
				{
					char buffer[] = "-0x00000000";
					char format[] = "%08x";
					uint32_t value = abs(static_cast<int32_t>(Value));
					if (value < 0x100) format[2] = '2';
					else if (value < 0x10000) format[2] = '4';
					
					sprintf(buffer + 3, format, value);
					return buffer + (Value >= 0);
				}
					
				default:
				{
					assert(false && "format type is not handled");
					return "<invalid>";
				}
			}
		}
		
		DisassembledOpcode::DisassembledOpcode()
		: Instruction(0)
		{ }
		
		std::string DisassembledOpcode::ArgumentsString() const
		{
			std::stringstream ss;
			auto iter = Arguments.cbegin();
			auto end = Arguments.cend();
			if (iter != Arguments.cend())
			{
				PrintOpcodeArgument(ss, iter, end);
				for (iter++; iter != end; iter++)
				{
					ss << ", ";
					PrintOpcodeArgument(ss, iter, end);
				}
			}
			return ss.str();
		}
		
		std::ostream& operator<<(std::ostream& into, const DisassembledOpcode& opcode)
		{
			return into << opcode.Opcode << '\t' << opcode.ArgumentsString();
		}
	}
}