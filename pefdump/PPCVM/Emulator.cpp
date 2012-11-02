//
//  Emulator.cpp
//  pefdump
//
//  Created by Félix on 2012-10-28.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "Emulator.h"

namespace
{
	bool NativeCall(const Common::UInt32* address, MachineState& state)
	{
		const void* nativeAddress = address + 1;
		((PPCVM::NativeCall)nativeAddress)(&state);
		state.pc = state.lr + 1;
		return false;
	}
	
	union Instruction
	{
		uint32_t full;
		struct
		{
			unsigned Prefix : 6;
			unsigned rD : 5;
			unsigned rA : 5;
			unsigned rB : 5;
			unsigned OE : 1;
			unsigned Infix : 9;
			unsigned Rc : 1;
		};
		
		Instruction(uint32_t value)
		{
			full = value;
		}
	};
	
	inline void Add(MachineState& state, Instruction i)
	{
		
	}
	
	inline void Prefix31(MachineState& state, Instruction instruction)
	{
		switch (instruction.Infix)
		{
			case 266: Add(state, instruction); break;
		}
	}
}

namespace PPCVM
{
	Emulator::Emulator(MachineState& state, const Common::UInt32* base)
	: Executor(state, base)
	{ }
	
	void Emulator::ExecuteFunction()
	{
		const Common::UInt32* instructions = base + state.pc;
		if (*instructions == 0xffffffff)
		{
			::NativeCall(instructions, state);
			return;
		}
		
		bool interpretNext = false;
		do
		{
			
		} while (interpretNext);
	}
	
	void Emulator::ExecuteSingleInstruction()
	{
		const Common::UInt32* instructions = base + state.pc;
		if (*instructions == 0xffffffff)
		{
			::NativeCall(instructions, state);
			return;
		}
		
		Instruction instr = instructions->Get();
		switch (instr.Prefix)
		{
			case 31: Prefix31(state, instr); break;
		}
	}
	
	Emulator::~Emulator()
	{ }
}
