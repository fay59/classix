// Copyright (C) 2003 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/

//
// IntegerInstructions.cpp
// Fork of Interpreter_Integer.cpp, from Dolphin Project.
// Largely modified to fit the needs of Classix.
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

#include "Interpreter.h"
#include "TrapException.h"

namespace
{
	using namespace PPCVM;
	
	inline void UpdateCRx(MachineState& state, int x, uint32_t value)
	{
		// CR bits:
		// d c b a (d: most significant)
		// a: overflow
		// b: eq
		// c: lt
		// d: gt
		
		uint8_t result;
		if (value == 0)
			result = 0b0010;
		else if ((value & 0x80000000) == 0x80000000)
			result = 0b1000;
		else
			result = 0b0100;
		
		result |= state.xer_so;
		state.cr[x] = result;
	}
	
	inline void UpdateCR0(MachineState& state, uint32_t value)
	{
		UpdateCRx(state, 0, value);
	}
	
	inline bool Carry(uint32_t a, uint32_t b)
	{
		return b > ~a;
	}
	
	inline uint32_t Mask(uint32_t mStart, uint32_t mStop)
	{
		uint32_t begin = 0xFFFFFFFF >> mStart;
		// this is *probably* not necessary, since mStop is always encoded on 5 bits it cannot be greater than 31,
		// but I'm scared to touch that code...
		// hopefully, llvm is able to optimize that correctly anyways
		uint32_t end = mStop < 31 ? (0xFFFFFFFF >> (mStop + 1)) : 0;
		uint32_t mask = begin ^ end;
		return mStop < mStart ? ~mask : mask;
	}
	
	template<typename IntType>
	inline IntType RotateLeft(IntType base, uint32_t offset)
	{
		const int bitCount = sizeof(IntType) * 8;
		offset &= bitCount - 1;
		return (base << offset) | (base >> (bitCount - offset));
	}
	
	template<typename IntType>
	inline IntType RotateRight(IntType base, uint32_t offset)
	{
		const int bitCount = sizeof(IntType) * 8;
		offset &= bitCount - 1;
		return (base >> offset) | (base << (bitCount - offset));
	}
}

namespace PPCVM
{
	namespace Execution
	{
		void Interpreter::addcx(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = state.gpr[inst.RB];
			state.gpr[inst.RD] = a + b;
			state.xer_ca = Carry(a,b);

			if (inst.OE) Panic("OE: addcx");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::addex(Instruction inst)
		{
			int carry = state.xer_ca;
			int a = state.gpr[inst.RA];
			int b = state.gpr[inst.RB];
			state.gpr[inst.RD] = a + b + carry;
			state.xer_ca = Carry(a, b) || (carry != 0 && Carry(a + b, carry));

			if (inst.OE) Panic("OE: addex");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::addi(Instruction inst)
		{
			if (inst.RA)
				state.gpr[inst.RD] = state.gpr[inst.RA] + inst.SIMM_16;
			else
				state.gpr[inst.RD] = inst.SIMM_16;
		}

		void Interpreter::addic(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t imm = (uint32_t)(int32_t)inst.SIMM_16;
			// TODO(ector): verify this thing
			state.gpr[inst.RD] = a + imm;
			state.xer_ca = Carry(a, imm);
		}

		void Interpreter::addic_rc(Instruction inst)
		{
			addic(inst);
			UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::addis(Instruction inst)
		{
			if (inst.RA)
				state.gpr[inst.RD] = state.gpr[inst.RA] + (inst.SIMM_16 << 16);
			else
				state.gpr[inst.RD] = (inst.SIMM_16 << 16);
		}

		void Interpreter::addmex(Instruction inst)
		{
			int carry = state.xer_ca;
			int a = state.gpr[inst.RA];
			state.gpr[inst.RD] = a + carry - 1;
			state.xer_ca = Carry(a, carry - 1);

			if (inst.OE) Panic("OE: addmex");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::addx(Instruction inst)
		{
			state.gpr[inst.RD] = state.gpr[inst.RA] + state.gpr[inst.RB];

			if (inst.OE) Panic("OE: addx");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::addzex(Instruction inst)
		{
			int carry = state.xer_ca;
			int a = state.gpr[inst.RA];
			state.gpr[inst.RD] = a + carry;
			state.xer_ca = Carry(a, carry);

			if (inst.OE) Panic("OE: addzex");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::andcx(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] & ~state.gpr[inst.RB];

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::andi_rc(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] & inst.UIMM;
			UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::andis_rc(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] & ((uint32_t)inst.UIMM<<16);
			UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::andx(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] & state.gpr[inst.RB];

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::cmp(Instruction inst)
		{
			int32_t a = (int32_t)state.gpr[inst.RA];
			int32_t b = (int32_t)state.gpr[inst.RB];
			uint8_t fTemp = 0x8;
			if (a > b)  fTemp = 0x4;
			else if (a == b) fTemp = 0x2;
			if (state.xer_so) Panic("cmp getting overflow flag"); // fTemp |= 0x1
			state.cr[inst.CRFD] = fTemp;
		}

		void Interpreter::cmpi(Instruction inst)
		{
			UpdateCRx(state, inst.CRFD, state.gpr[inst.RA] - inst.SIMM_16);
		}

		void Interpreter::cmpl(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = state.gpr[inst.RB];
			uint8_t fTemp = 0x8; // a < b

				//	if (a < b)  fTemp = 0x8;else
			if (a > b)  fTemp = 0x4;
			else if (a == b) fTemp = 0x2;
			if (state.xer_so) Panic("cmpl getting overflow flag"); // fTemp |= 0x1;
			state.cr[inst.CRFD] = fTemp;
		}

		void Interpreter::cmpli(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = inst.UIMM;
			uint8_t f;
			if (a < b)      f = 0x8;
			else if (a > b) f = 0x4;
			else            f = 0x2; //equals
			if (state.xer_so) f |= 0x1;
			state.cr[inst.CRFD] = f;
		}

		void Interpreter::cntlzwx(Instruction inst)
		{
			uint32_t val = state.gpr[inst.RS];
			uint32_t mask = 0x80000000;
			int i = 0;
			for (; i < 32; i++, mask >>= 1)
				if (val & mask)
					break;
			state.gpr[inst.RA] = i;
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::divwux(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = state.gpr[inst.RB];

			if (b == 0)
			{
				if (inst.OE)
					// should set OV
					Panic("OE: divwux");
				state.gpr[inst.RD] = 0;
			}
			else
				state.gpr[inst.RD] = a / b;

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::divwx(Instruction inst)
		{
			int32_t a = state.gpr[inst.RA];
			int32_t b = state.gpr[inst.RB];
			if (b == 0 || ((uint32_t)a == 0x80000000 && b == -1))
			{
				if (inst.OE)
					// should set OV
					Panic("OE: divwx");
				if (((uint32_t)a & 0x80000000) && b == 0)
					state.gpr[inst.RD] = -1;
				else
					state.gpr[inst.RD] = 0;
			}
			else
				state.gpr[inst.RD] = (uint32_t)(a / b);

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::eqvx(Instruction inst)
		{
			state.gpr[inst.RA] = ~(state.gpr[inst.RS] ^ state.gpr[inst.RB]);

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::extsbx(Instruction inst)
		{
			state.gpr[inst.RA] = (uint32_t)(int32_t)(int8_t)state.gpr[inst.RS];

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::extshx(Instruction inst)
		{
			state.gpr[inst.RA] = (uint32_t)(int32_t)(int16_t)state.gpr[inst.RS];

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::mulhwux(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = state.gpr[inst.RB];
			uint32_t d = (uint32_t)(((uint64_t)a * (uint64_t)b) >> 32);
			state.gpr[inst.RD] = d;
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::mulhwx(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = state.gpr[inst.RB];
			// This can be done better. Not in plain C/C++ though.
			uint32_t d = (uint32_t)((uint64_t)(((int64_t)(int32_t)a * (int64_t)(int32_t)b) ) >> 32);
			state.gpr[inst.RD] = d;
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::mulli(Instruction inst)
		{
			state.gpr[inst.RD] = (int32_t)state.gpr[inst.RA] * inst.SIMM_16;
		}

		void Interpreter::mullwx(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = state.gpr[inst.RB];
			uint32_t d = (uint32_t)((int32_t)a * (int32_t)b);
			state.gpr[inst.RD] = d;

			if (inst.OE) Panic("OE: mullwx");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::nandx(Instruction inst)
		{
			state.gpr[inst.RA] = ~(state.gpr[inst.RS] & state.gpr[inst.RB]);

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::negx(Instruction inst)
		{
			state.gpr[inst.RD] = (~state.gpr[inst.RA]) + 1;
			if (state.gpr[inst.RD] == 0x80000000)
			{
				if (inst.OE) Panic("OE: negx");
			}
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::norx(Instruction inst)
		{
			state.gpr[inst.RA] = ~(state.gpr[inst.RS] | state.gpr[inst.RB]);

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::orcx(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] | (~state.gpr[inst.RB]);

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::ori(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] | inst.UIMM;
		}

		void Interpreter::oris(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] | (inst.UIMM << 16);
		}

		void Interpreter::orx(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] | state.gpr[inst.RB];

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::rlwimix(Instruction inst)
		{
			uint32_t mask = Mask(inst.MB,inst.ME);
			state.gpr[inst.RA] = (state.gpr[inst.RA] & ~mask) | (RotateLeft(state.gpr[inst.RS],inst.SH) & mask);
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::rlwinmx(Instruction inst)
		{
			uint32_t n = inst.SH;
			uint32_t r = RotateLeft(state.gpr[inst.RS], n);
			uint32_t m = Mask(inst.MB, inst.ME);
			state.gpr[inst.RA] = r & m;
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::rlwnmx(Instruction inst)
		{
			uint32_t mask = Mask(inst.MB,inst.ME);
			state.gpr[inst.RA] = RotateLeft(state.gpr[inst.RS], state.gpr[inst.RB] & 0x1F) & mask;

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::slwx(Instruction inst)
		{
			uint32_t amount = state.gpr[inst.RB];
			state.gpr[inst.RA] = (amount & 0x20) ? 0 : state.gpr[inst.RS] << amount;

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::srawix(Instruction inst)
		{
			int amount = inst.SH;

			if (amount != 0)
			{
				int32_t rrs = state.gpr[inst.RS];
				state.gpr[inst.RA] = rrs >> amount;

				if ((rrs < 0) && (rrs << (32 - amount)))
					state.xer_ca = 1;
				else
					state.xer_ca = 0;
			}
			else
			{
				state.xer_ca = 0;
				state.gpr[inst.RA] = state.gpr[inst.RS];
			}

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::srawx(Instruction inst)
		{
			int rb = state.gpr[inst.RB];
			if (rb & 0x20)
			{
				if (state.gpr[inst.RS] & 0x80000000)
				{
					state.gpr[inst.RA] = 0xFFFFFFFF;
					state.xer_ca = 1;
				}
				else
				{
					state.gpr[inst.RA] = 0x00000000;
					state.xer_ca = 0;
				}
			}
			else
			{
				int amount = rb & 0x1f;
				if (amount == 0)
				{
					state.gpr[inst.RA] = state.gpr[inst.RS];
					state.xer_ca = 0;
				}
				else
				{
					state.gpr[inst.RA] = (uint32_t)((int32_t)state.gpr[inst.RS] >> amount);
					if (state.gpr[inst.RS] & 0x80000000)
						state.xer_ca = 1;
					else
						state.xer_ca = 0;
				}
			}

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::srwx(Instruction inst)
		{
			uint32_t amount = state.gpr[inst.RB];
			state.gpr[inst.RA] = (amount & 0x20) ? 0 : (state.gpr[inst.RS] >> (amount & 0x1f));

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}

		void Interpreter::subfcx(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = state.gpr[inst.RB];
			state.gpr[inst.RD] = b - a;
			state.xer_ca = a == 0 || Carry(b, 0-a);

			if (inst.OE) Panic("OE: subfcx");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::subfex(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			uint32_t b = state.gpr[inst.RB];
			int carry = state.xer_ca;
			state.gpr[inst.RD] = (~a) + b + carry;
			state.xer_ca = Carry(~a, b) || Carry((~a) + b, carry);

			if (inst.OE) Panic("OE: subfex");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::subfic(Instruction inst)
		{
			int32_t immediate = inst.SIMM_16;
			state.gpr[inst.RD] = immediate - (signed)state.gpr[inst.RA];
			state.xer_ca = (state.gpr[inst.RA] == 0) || (Carry(0-state.gpr[inst.RA], immediate));
		}

		void Interpreter::subfmex(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			int carry = state.xer_ca;
			state.gpr[inst.RD] = (~a) + carry - 1;
			state.xer_ca = Carry(~a, carry - 1);

			if (inst.OE) Panic("OE: subfmex");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::subfx(Instruction inst)
		{
			state.gpr[inst.RD] = state.gpr[inst.RB] - state.gpr[inst.RA];

			if (inst.OE) Panic("OE: subfx");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::subfzex(Instruction inst)
		{
			uint32_t a = state.gpr[inst.RA];
			int carry = state.xer_ca;
			state.gpr[inst.RD] = (~a) + carry;
			state.xer_ca = Carry(~a, carry);

			if (inst.OE) Panic("OE: subfzex");
			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RD]);
		}

		void Interpreter::tw(Instruction inst)
		{
			int32_t a = state.gpr[inst.RA];
			int32_t b = state.gpr[inst.RB];
			int32_t TO = inst.TO;

			if (   ((a < b) && (TO & 0x10))
				|| ((a > b) && (TO & 0x08))
				|| ((a ==b) && (TO & 0x04))
				|| (((uint32_t)a <(uint32_t)b) && (TO & 0x02))
				|| (((uint32_t)a >(uint32_t)b) && (TO & 0x01)))
			{
				throw PPCVM::TrapException("tw");
			}
		}

		void Interpreter::twi(Instruction inst)
		{
			int32_t a = state.gpr[inst.RA];
			int32_t b = inst.SIMM_16;
			int32_t TO = inst.TO;

			if (   ((a < b) && (TO & 0x10))
				|| ((a > b) && (TO & 0x08))
				|| ((a ==b) && (TO & 0x04))
				|| (((uint32_t)a <(uint32_t)b) && (TO & 0x02))
				|| (((uint32_t)a >(uint32_t)b) && (TO & 0x01)))
			{
				throw PPCVM::TrapException("twi");
			}
		}

		void Interpreter::xori(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] ^ inst.UIMM;
		}

		void Interpreter::xoris(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] ^ (inst.UIMM << 16);
		}

		void Interpreter::xorx(Instruction inst)
		{
			state.gpr[inst.RA] = state.gpr[inst.RS] ^ state.gpr[inst.RB];

			if (inst.Rc) UpdateCR0(state, state.gpr[inst.RA]);
		}		
	}
}