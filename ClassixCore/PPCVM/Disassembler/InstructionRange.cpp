//
//  InstructionRange.cpp
//  Classix
//
//  Created by Félix on 2012-11-12.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "InstructionRange.h"
#include <cassert>
#include <sstream>

namespace
{
	const char* numbers[] = {
		"0", "1", "2", "3", "4", "5", "6", "7", "8",
		"9", "10", "11", "12", "13", "14", "15", "16",
		"17", "18", "19", "20", "21", "22", "23", "24",
		"25", "26", "27", "28", "29", "30", "31"
	};
	
	std::string g(uint32_t reg)
	{
		return std::string("r") + numbers[reg];
	}
	
	std::string f(uint32_t reg)
	{
		return std::string("f") + numbers[reg];
	}
	
	std::string cr(uint32_t cr)
	{
		return std::string("crf") + static_cast<char>('0' + cr);
	}
	
	std::string hex(uint32_t h)
	{
		std::stringstream ss;
		ss << "0x" << std::hex << h;
		return ss.str();
	}
	
	std::string opX(const std::string& x, bool setsFlags)
	{
		if (!setsFlags)
			return x;
		return x + '.';
	}
}

namespace PPCVM
{
	namespace Disassembly
	{
		std::ostream& operator<<(std::ostream& into, const DisassembledOpcode& opcode)
		{
			into << opcode.Opcode << '\t';
			
			auto iter = opcode.Arguments.cbegin();
			if (iter != opcode.Arguments.cend())
				into << *iter;
			
			for (iter++; iter != opcode.Arguments.cend(); iter++)
				into << ", " << *iter;
			
			if (opcode.Complement.length() > 0)
				into << " <" << opcode.Complement << '>';
			return into;
		}
		
		InstructionRange::InstructionRange(Common::IAllocator* allocator, const Common::UInt32* begin)
		: Begin(begin), End(nullptr), allocator(allocator), r12(nullptr)
		{ }
		
		void InstructionRange::SetEnd(const Common::UInt32 *end)
		{
			assert(end >= Begin && "End before Begin");
			End = end;
			
			Opcodes.reserve(End - Begin);
			for (auto iter = Begin; iter != End; iter++)
				Dispatch(PPCVM::Instruction(*iter));
		}
		
#pragma mark -
#define IMPL(x)	void InstructionRange::x(PPCVM::Instruction i)
		void InstructionRange::unknown(PPCVM::Instruction i)
		{
			Emit(".word", hex(i.hex));
		}
		
#pragma mark -
#pragma mark Floating Point Instructions
#define FX2(name)	IMPL(name) { Emit(opX(#name, i.RC), f(i.RD), f(i.RB)); }
#define FX3(name)	IMPL(name) { Emit(opX(#name, i.RC), f(i.RD), f(i.RA), f(i.RB)); }
#define FX4(name)	IMPL(name) { Emit(opX(#name, i.RC), f(i.RD), f(i.RA), f(i.RC), f(i.RB)); }
#define FX2S(name)	FX2(name##x) FX2(name##sx)
#define FX3S(name)	FX3(name##x) FX3(name##sx)
#define FX2Z(name)	FX2(name##x) FX2(name##zx)
#define FX4S(name)	FX4(name##x) FX4(name##sx)
		
		FX2(fabsx);
		FX3S(fadd);
		
		IMPL(fcmpo)
		{
			Emit("fcmpo", cr(i.CRFD), f(i.RA), f(i.RB));
		}
		
		IMPL(fcmpu)
		{
			Emit("fcmpu", cr(i.CRFD), f(i.RA), f(i.RB));
		}
		
		FX2Z(fctiw);
		FX3S(fdiv);
		FX4S(fmadd);
		FX2(fmrx);
		FX4S(fmsub);
		FX3S(fmul);
		FX2(fnabsx);
		FX2(fnegx);
		FX4S(fnmadd);
		FX4S(fnmsub);
		FX2(fresx);
		FX2(frspx);
		FX2(frsqrtex);
		FX4(fselx);
		FX2S(fsqrt);
		FX2S(fsub);
		
#pragma mark -
#pragma mark Integer Instructions
	}
}
