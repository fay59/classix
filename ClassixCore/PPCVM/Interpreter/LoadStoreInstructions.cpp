//
// LoadStoreInstructions.cpp
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

#include "Interpreter.h"
#include "BigEndian.h"

using namespace Common;

namespace
{
	using namespace PPCVM;
	
#if TARGET_RT_BIG_ENDIAN
	const bool IsBigEndian = true;
#else
	const bool IsBigEndian = false;
#endif
	
	template<typename T>
	inline T* GetEffectivePointer(Common::IAllocator& allocator, MachineState& state, Instruction inst)
	{
		uint32_t address = inst.RA
			? state.gpr[inst.RA] + inst.SIMM_16
			: inst.SIMM_16;
		return allocator.ToPointer<T>(address);
	}
	
	template<typename T>
	inline T* GetEffectivePointerU(Common::IAllocator& allocator, MachineState& state, Instruction inst)
	{
		uint32_t address = state.gpr[inst.RA] + inst.SIMM_16;
		return allocator.ToPointer<T>(address);
	}
	
	template<typename T>
	inline T* GetEffectivePointerX(Common::IAllocator& allocator, MachineState& state, Instruction inst)
	{
		uint32_t address = inst.RA
			? state.gpr[inst.RA] + state.gpr[inst.RB]
			: state.gpr[inst.RB];
		return allocator.ToPointer<T>(address);
	}
	
	template<typename T>
	inline T* GetEffectivePointerUX(Common::IAllocator& allocator, MachineState& state, Instruction inst)
	{
		uint32_t address = state.gpr[inst.RA] + state.gpr[inst.RB];
		return allocator.ToPointer<T>(address);
	}
}

namespace PPCVM
{
	namespace Execution
	{
		void Interpreter::eieio(Instruction inst)
		{
			__sync_synchronize();
		}
		
		void Interpreter::lbz(Instruction inst)
		{
			state.gpr[inst.RD] = *GetEffectivePointer<uint8_t>(allocator, state, inst);
		}
		
		void Interpreter::lbzu(Instruction inst)
		{
			uint8_t* address = GetEffectivePointerU<uint8_t>(allocator, state, inst);
			state.gpr[inst.RD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lbzux(Instruction inst)
		{
			uint8_t* address = GetEffectivePointerUX<uint8_t>(allocator, state, inst);
			state.gpr[inst.RD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lbzx(Instruction inst)
		{
			state.gpr[inst.RD] = *GetEffectivePointerX<uint8_t>(allocator, state, inst);
		}
		
		void Interpreter::lfd(Instruction inst)
		{
			Real64* ptr = GetEffectivePointer<Real64>(allocator, state, inst);
			state.fpr[inst.FD] = *ptr;
		}
		
		void Interpreter::lfdu(Instruction inst)
		{
			Real64* address = GetEffectivePointerU<Real64>(allocator, state, inst);
			state.fpr[inst.FD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lfdux(Instruction inst)
		{
			Real64* address = GetEffectivePointerUX<Real64>(allocator, state, inst);
			state.fpr[inst.FD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lfdx(Instruction inst)
		{
			Real64* address = GetEffectivePointerX<Real64>(allocator, state, inst);
			state.gpr[inst.FD] = *address;
		}
		
		void Interpreter::lfs(Instruction inst)
		{
			state.fpr[inst.FD] = *GetEffectivePointer<Real32>(allocator, state, inst);
		}
		
		void Interpreter::lfsu(Instruction inst)
		{
			Real32* address = GetEffectivePointerU<Real32>(allocator, state, inst);
			state.fpr[inst.FD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lfsux(Instruction inst)
		{
			Real32* address = GetEffectivePointerUX<Real32>(allocator, state, inst);
			state.fpr[inst.FD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lfsx(Instruction inst)
		{
			Real32* address = GetEffectivePointerX<Real32>(allocator, state, inst);
			state.gpr[inst.FD] = *address;
		}
		
		void Interpreter::lha(Instruction inst)
		{
			state.gpr[inst.RD] = *GetEffectivePointer<SInt16>(allocator, state, inst);
		}
		
		void Interpreter::lhau(Instruction inst)
		{
			SInt16* address = GetEffectivePointerU<SInt16>(allocator, state, inst);
			state.gpr[inst.RD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lhaux(Instruction inst)
		{
			SInt16* address = GetEffectivePointerUX<SInt16>(allocator, state, inst);
			state.gpr[inst.RD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lhax(Instruction inst)
		{
			state.gpr[inst.RD] = *GetEffectivePointerX<SInt16>(allocator, state, inst);
		}
		
		void Interpreter::lhbrx(Instruction inst)
		{
			uint16_t halfWord = *GetEffectivePointerX<uint16_t>(allocator, state, inst);
			state.gpr[inst.RD] = HostToLittle(halfWord);
		}
		
		void Interpreter::lhz(Instruction inst)
		{
			state.gpr[inst.RD] = *GetEffectivePointer<UInt16>(allocator, state, inst);
		}
		
		void Interpreter::lhzu(Instruction inst)
		{
			UInt16* address = GetEffectivePointerU<UInt16>(allocator, state, inst);
			state.gpr[inst.RD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lhzux(Instruction inst)
		{
			UInt16* address = GetEffectivePointerUX<UInt16>(allocator, state, inst);
			state.gpr[inst.RD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lhzx(Instruction inst)
		{
			state.gpr[inst.RD] = *GetEffectivePointerX<UInt16>(allocator, state, inst);
		}
		
		void Interpreter::lmw(Instruction inst)
		{
			// first copy locally, then write to registers: that way, if a segfault
			// occurs, registers are still clean.
			uint32_t words[32];
			UInt32* address = GetEffectivePointer<UInt32>(allocator, state, inst);
			for (int i = inst.RD; i < 32; i++)
			{
				words[i] = *address;
				address++;
			}
			
			memcpy(state.gpr + inst.RD, words + inst.RD, (32 - inst.RD) * sizeof(uint32_t));
		}
		
		void Interpreter::lswi(Instruction inst)
		{
			uint8_t* EA = inst.RA == 0 ? nullptr : reinterpret_cast<uint8_t*>(state.gpr[inst.RA]);
			
			uint32_t n;
			if (inst.NB == 0)
				n = 32;
			else
				n = inst.NB;
			
			int r = inst.RD - 1;
			int i = 0;
			while (n>0)
			{
				if (i==0)
				{
					r++;
					r &= 31;
					state.gpr[r] = 0;
				}
				
				uint32_t TempValue = *EA << (24 - i);
				
				state.gpr[r] |= TempValue;
				
				i += 8;
				if (i == 32)
					i = 0;
				EA++;
				n--;
			}
		}
		
		void Interpreter::lswx(Instruction inst)
		{
			// according to the guys behind the Dolphin InterpreterBase, this could be
			// incorrect
			uint8_t* address = GetEffectivePointerX<uint8_t>(allocator, state, inst);
			uint32_t n = state.xer & 0x7f;
			uint32_t r = inst.RD;
			uint32_t i = 0;
			
			if (n != 0)
			{
				state.gpr[r] = 0;
				do
				{
					uint32_t value = *address << (24 - i * 8);
					state.gpr[r] |= value;
					address++;
					i++;
					n--;
					if (i == 4)
					{
						i = 0;
						r++;
						r %= 32;
						state.gpr[r] = 0;
					}
				}
				while (n > 0);
			}
		}
		
		void Interpreter::lwarx(Instruction inst)
		{
			// since we emulate only one CPU, treat lwarx as a regular load
			state.gpr[inst.RD] = *GetEffectivePointerX<UInt32>(allocator, state, inst);
		}
		
		void Interpreter::lwbrx(Instruction inst)
		{
			uint32_t word = *GetEffectivePointerX<uint32_t>(allocator, state, inst);
			state.gpr[inst.RD] = HostToLittle(word);
		}
		
		void Interpreter::lwz(Instruction inst)
		{
			state.gpr[inst.RD] = *GetEffectivePointer<UInt32>(allocator, state, inst);
		}
		
		void Interpreter::lwzu(Instruction inst)
		{
			UInt32* address = GetEffectivePointerU<UInt32>(allocator, state, inst);
			state.gpr[inst.RD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lwzux(Instruction inst)
		{
			UInt32* address = GetEffectivePointerUX<UInt32>(allocator, state, inst);
			state.gpr[inst.RD] = *address;
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::lwzx(Instruction inst)
		{
			state.gpr[inst.RD] = *GetEffectivePointerX<UInt32>(allocator, state, inst);
		}
		
		void Interpreter::stb(Instruction inst)
		{
			*GetEffectivePointer<uint8_t>(allocator, state, inst) = static_cast<uint8_t>(state.gpr[inst.RS]);
		}
		
		void Interpreter::stbu(Instruction inst)
		{
			uint8_t* address = GetEffectivePointerU<uint8_t>(allocator, state, inst);
			*address = static_cast<uint8_t>(state.gpr[inst.RS]);
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::stbux(Instruction inst)
		{
			uint8_t* address = GetEffectivePointerUX<uint8_t>(allocator, state, inst);
			*address = static_cast<uint8_t>(state.gpr[inst.RS]);
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::stbx(Instruction inst)
		{
			*GetEffectivePointerX<uint8_t>(allocator, state, inst) = static_cast<uint8_t>(state.gpr[inst.RS]);
		}
		
		void Interpreter::stfd(Instruction inst)
		{
			Real64* address = GetEffectivePointer<Real64>(allocator, state, inst);
			*address = state.fpr[inst.FS];
		}
		
		void Interpreter::stfdu(Instruction inst)
		{
			Real64* address = GetEffectivePointerU<Real64>(allocator, state, inst);
			*address = state.fpr[inst.FS];
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::stfdux(Instruction inst)
		{
			Real64* address = GetEffectivePointerUX<Real64>(allocator, state, inst);
			*address = state.fpr[inst.FS];
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::stfdx(Instruction inst)
		{
			*GetEffectivePointerX<Real64>(allocator, state, inst) = state.fpr[inst.RA];
		}
		
		void Interpreter::stfiwx(Instruction inst)
		{
			UInt32* address = GetEffectivePointerX<UInt32>(allocator, state, inst);
			uint32_t* fprAsInteger = reinterpret_cast<uint32_t*>(&state.fpr[inst.FS]);
			*address = fprAsInteger[IsBigEndian ? 1 : 0];
		}
		
		void Interpreter::stfs(Instruction inst)
		{
			Real32* address = GetEffectivePointer<Real32>(allocator, state, inst);
			*address = static_cast<float>(state.fpr[inst.FS]);
		}
		
		void Interpreter::stfsu(Instruction inst)
		{
			Real32* address = GetEffectivePointerU<Real32>(allocator, state, inst);
			*address = static_cast<float>(state.fpr[inst.FS]);
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::stfsux(Instruction inst)
		{
			Real32* address = GetEffectivePointerUX<Real32>(allocator, state, inst);
			*address = static_cast<float>(state.fpr[inst.FS]);
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::stfsx(Instruction inst)
		{
			*GetEffectivePointerX<Real32>(allocator, state, inst) = static_cast<float>(state.fpr[inst.FS]);
		}
		
		void Interpreter::sth(Instruction inst)
		{
			*GetEffectivePointer<UInt16>(allocator, state, inst) = static_cast<uint16_t>(state.gpr[inst.RS]);
		}
		
		void Interpreter::sthbrx(Instruction inst)
		{
			uint16_t halfWord = HostToLittle(static_cast<uint16_t>(state.gpr[inst.RS]));
			*GetEffectivePointerX<uint16_t>(allocator, state, inst) = halfWord;
		}
		
		void Interpreter::sthu(Instruction inst)
		{
			UInt16* address = GetEffectivePointerU<UInt16>(allocator, state, inst);
			*address = static_cast<uint16_t>(state.gpr[inst.RS]);
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::sthux(Instruction inst)
		{
			UInt16* address = GetEffectivePointerUX<UInt16>(allocator, state, inst);
			*address = static_cast<uint16_t>(state.gpr[inst.RS]);
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::sthx(Instruction inst)
		{
			*GetEffectivePointerX<UInt16>(allocator, state, inst) = static_cast<uint16_t>(state.gpr[inst.RS]);
		}
		
		void Interpreter::stmw(Instruction inst)
		{
			UInt32* address = GetEffectivePointer<UInt32>(allocator, state, inst);
			for (int i = inst.RS; i < 32; i++)
			{
				*address = state.gpr[i];
				address++;
			}
		}
		
		void Interpreter::stswi(Instruction inst)
		{
			uint8_t* EA = inst.RA == 0 ? nullptr : reinterpret_cast<uint8_t*>(state.gpr[inst.RA]);
			
			uint32_t n;
			if (inst.NB == 0)
				n = 32;
			else
				n = inst.NB;
			
			int r = inst.RS - 1;
			int i = 0;
			while (n > 0)
			{
				if (i == 0)
				{
					r++;
					r &= 31;
				}
				
				*EA = static_cast<uint8_t>(state.gpr[r] >> (24 - i));
				
				i += 8;
				if (i == 32)
					i = 0;
				EA++;
				n--;
			}
		}
		
		void Interpreter::stswx(Instruction inst)
		{
			uint8_t* EA = GetEffectivePointerX<uint8_t>(allocator, state, inst);
			uint32_t n = state.xer & 0x7F;
			int r = inst.RS;
			int i = 0;
			
			while (n > 0)
			{
				*EA = static_cast<uint8_t>(state.gpr[r] >> (24 - i));
				
				EA++;
				n--;
				i += 8;
				if (i == 32)
				{
					i = 0;
					r++;
				}
			}
		}
		
		void Interpreter::stw(Instruction inst)
		{
			*GetEffectivePointer<UInt32>(allocator, state, inst) = state.gpr[inst.RS];
		}
		
		void Interpreter::stwbrx(Instruction inst)
		{
			*GetEffectivePointerX<uint32_t>(allocator, state, inst) = HostToLittle(state.gpr[inst.RS]);
		}
		
		void Interpreter::stwcxd(Instruction inst)
		{
			// since we emulate only one CPU, treat stwcxd as a regular store
			*GetEffectivePointerX<UInt32>(allocator, state, inst) = state.gpr[inst.RS];
			state.cr[0] = state.xer_so;
		}
		
		void Interpreter::stwu(Instruction inst)
		{
			UInt32* address = GetEffectivePointerU<UInt32>(allocator, state, inst);
			*address = state.gpr[inst.RS];
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::stwux(Instruction inst)
		{
			UInt32* address = GetEffectivePointerUX<UInt32>(allocator, state, inst);
			*address = state.gpr[inst.RS];
			state.gpr[inst.RA] = allocator.ToIntPtr(address);
		}
		
		void Interpreter::stwx(Instruction inst)
		{
			*GetEffectivePointerX<UInt32>(allocator, state, inst) = state.gpr[inst.RS];
		}
		
		void Interpreter::sync(Instruction inst)
		{
			__sync_synchronize();
		}		
	}
}