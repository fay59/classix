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
	inline T* GetEffectivePointer(MachineState* state, Instruction inst)
	{
		intptr_t address = inst.RA
			? state->gpr[inst.RA] + inst.SIMM_16
			: inst.SIMM_16;
		return reinterpret_cast<T*>(address);
	}

	template<typename T>
	inline T* GetEffectivePointerU(MachineState* state, Instruction inst)
	{
		intptr_t address = state->gpr[inst.RA] + inst.SIMM_16;;
		return reinterpret_cast<T*>(address);
	}

	template<typename T>
	inline T* GetEffectivePointerX(MachineState* state, Instruction inst)
	{
		intptr_t address = inst.RA
			? state->gpr[inst.RA] + state->gpr[inst.RB]
			: state->gpr[inst.RB];
		return reinterpret_cast<T*>(address);
	}

	template<typename T>
	inline T* GetEffectivePointerUX(MachineState* state, Instruction inst)
	{
		intptr_t address = state->gpr[inst.RA] + state->gpr[inst.RB];
		return reinterpret_cast<T*>(address);
	}

	template<typename T>
	inline intptr_t ToIntPtr(const T* address)
	{
		return reinterpret_cast<intptr_t>(address);
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
			state->gpr[inst.RD] = *GetEffectivePointer<uint8_t>(state, inst);
		}

		void Interpreter::lbzu(Instruction inst)
		{
			uint8_t* address = GetEffectivePointerU<uint8_t>(state, inst);
			state->gpr[inst.RD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lbzux(Instruction inst)
		{
			uint8_t* address = GetEffectivePointerUX<uint8_t>(state, inst);
			state->gpr[inst.RD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lbzx(Instruction inst)
		{
			state->gpr[inst.RD] = *GetEffectivePointerX<uint8_t>(state, inst);
		}

		void Interpreter::lfd(Instruction inst)
		{
			state->fpr[inst.FD] = *GetEffectivePointer<Real64>(state, inst);
		}

		void Interpreter::lfdu(Instruction inst)
		{
			Real64* address = GetEffectivePointerU<Real64>(state, inst);
			state->fpr[inst.FD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lfdux(Instruction inst)
		{
			Real64* address = GetEffectivePointerUX<Real64>(state, inst);
			state->fpr[inst.FD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lfdx(Instruction inst)
		{
			Real64* address = GetEffectivePointerX<Real64>(state, inst);
			state->gpr[inst.FD] = *address;
		}

		void Interpreter::lfs(Instruction inst)
		{
			state->fpr[inst.FD] = *GetEffectivePointer<Real32>(state, inst);
		}

		void Interpreter::lfsu(Instruction inst)
		{
			Real32* address = GetEffectivePointerU<Real32>(state, inst);
			state->fpr[inst.FD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lfsux(Instruction inst)
		{
			Real32* address = GetEffectivePointerUX<Real32>(state, inst);
			state->fpr[inst.FD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lfsx(Instruction inst)
		{
			Real32* address = GetEffectivePointerX<Real32>(state, inst);
			state->gpr[inst.FD] = *address;
		}

		void Interpreter::lha(Instruction inst)
		{
			state->gpr[inst.RD] = *GetEffectivePointer<SInt16>(state, inst);
		}

		void Interpreter::lhau(Instruction inst)
		{
			SInt16* address = GetEffectivePointerU<SInt16>(state, inst);
			state->gpr[inst.RD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lhaux(Instruction inst)
		{
			SInt16* address = GetEffectivePointerUX<SInt16>(state, inst);
			state->gpr[inst.RD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lhax(Instruction inst)
		{
			state->gpr[inst.RD] = *GetEffectivePointerX<SInt16>(state, inst);
		}

		void Interpreter::lhbrx(Instruction inst)
		{
			uint16_t halfWord = *GetEffectivePointerX<uint16_t>(state, inst);
			state->gpr[inst.RD] = EndianU16_LtoN(halfWord);
		}

		void Interpreter::lhz(Instruction inst)
		{
			state->gpr[inst.RD] = *GetEffectivePointer<UInt16>(state, inst);
		}

		void Interpreter::lhzu(Instruction inst)
		{
			UInt16* address = GetEffectivePointerU<UInt16>(state, inst);
			state->gpr[inst.RD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lhzux(Instruction inst)
		{
			UInt16* address = GetEffectivePointerUX<UInt16>(state, inst);
			state->gpr[inst.RD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lhzx(Instruction inst)
		{
			state->gpr[inst.RD] = *GetEffectivePointerX<UInt16>(state, inst);
		}

		void Interpreter::lmw(Instruction inst)
		{
			// first copy locally, then write to registers: that way, if a segfault
			// occurs, registers are still clean.
			uint32_t words[32];
			UInt32* address = GetEffectivePointer<UInt32>(state, inst);
			for (int i = inst.RD; i < 32; i++)
			{
				words[i] = *address;
				address++;
			}

			memcpy(state->gpr + inst.RD, words + inst.RD, 32 - inst.RD);
		}

		void Interpreter::lswi(Instruction inst)
		{
			uint8_t* EA = inst.RA == 0 ? nullptr : reinterpret_cast<uint8_t*>(state->gpr[inst.RA]);

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
					state->gpr[r] = 0;
				}

				uint32_t TempValue = *EA << (24 - i);

				state->gpr[r] |= TempValue;

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
			uint8_t* address = GetEffectivePointerX<uint8_t>(state, inst);
			uint32_t n = state->xer & 0x7f;
			uint32_t r = inst.RD;
			uint32_t i = 0;
			
			if (n != 0)
			{
				state->gpr[r] = 0;
				do
				{
					uint32_t value = *address << (24 - i * 8);
					state->gpr[r] |= value;
					address++;
					i++;
					n--;
					if (i == 4)
					{
						i = 0;
						r++;
						r %= 32;
						state->gpr[r] = 0;
					}
				}
				while (n > 0);
			}
		}

		void Interpreter::lwarx(Instruction inst)
		{
			// since we emulate only one CPU, treat lwarx as a regular load
			state->gpr[inst.RD] = *GetEffectivePointerX<UInt32>(state, inst);
		}

		void Interpreter::lwbrx(Instruction inst)
		{
			uint32_t word = *GetEffectivePointerX<uint32_t>(state, inst);
			state->gpr[inst.RD] = EndianU32_NtoL(word);
		}

		void Interpreter::lwz(Instruction inst)
		{
			state->gpr[inst.RD] = *GetEffectivePointer<UInt32>(state, inst);
		}

		void Interpreter::lwzu(Instruction inst)
		{
			UInt32* address = GetEffectivePointerU<UInt32>(state, inst);
			state->gpr[inst.RD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lwzux(Instruction inst)
		{
			UInt32* address = GetEffectivePointerUX<UInt32>(state, inst);
			state->gpr[inst.RD] = *address;
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::lwzx(Instruction inst)
		{
			state->gpr[inst.RD] = *GetEffectivePointerX<UInt32>(state, inst);
		}

		void Interpreter::stb(Instruction inst)
		{
			*GetEffectivePointer<uint8_t>(state, inst) = static_cast<uint8_t>(state->gpr[inst.RS]);
		}

		void Interpreter::stbu(Instruction inst)
		{
			uint8_t* address = GetEffectivePointerU<uint8_t>(state, inst);
			*address = static_cast<uint8_t>(state->gpr[inst.RS]);
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::stbux(Instruction inst)
		{
			uint8_t* address = GetEffectivePointerUX<uint8_t>(state, inst);
			*address = static_cast<uint8_t>(state->gpr[inst.RS]);
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::stbx(Instruction inst)
		{
			*GetEffectivePointerX<uint8_t>(state, inst) = static_cast<uint8_t>(state->gpr[inst.RS]);
		}

		void Interpreter::stfd(Instruction inst)
		{
			Real64* address = GetEffectivePointer<Real64>(state, inst);
			*address = state->fpr[inst.FS];
		}

		void Interpreter::stfdu(Instruction inst)
		{
			Real64* address = GetEffectivePointerU<Real64>(state, inst);
			*address = state->fpr[inst.FS];
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::stfdux(Instruction inst)
		{
			Real64* address = GetEffectivePointerUX<Real64>(state, inst);
			*address = state->fpr[inst.FS];
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::stfdx(Instruction inst)
		{
			*GetEffectivePointerX<Real64>(state, inst) = state->fpr[inst.RA];
		}

		void Interpreter::stfiwx(Instruction inst)
		{
			UInt32* address = GetEffectivePointerX<UInt32>(state, inst);
			uint32_t* fprAsInteger = reinterpret_cast<uint32_t*>(&state->fpr[inst.FS]);
			*address = fprAsInteger[IsBigEndian ? 1 : 0];
		}

		void Interpreter::stfs(Instruction inst)
		{
			Real32* address = GetEffectivePointer<Real32>(state, inst);
			*address = static_cast<float>(state->fpr[inst.FS]);
		}

		void Interpreter::stfsu(Instruction inst)
		{
			Real32* address = GetEffectivePointerU<Real32>(state, inst);
			*address = static_cast<float>(state->fpr[inst.FS]);
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::stfsux(Instruction inst)
		{
			Real32* address = GetEffectivePointerUX<Real32>(state, inst);
			*address = static_cast<float>(state->fpr[inst.FS]);
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::stfsx(Instruction inst)
		{
			*GetEffectivePointerX<Real32>(state, inst) = static_cast<float>(state->fpr[inst.FS]);
		}

		void Interpreter::sth(Instruction inst)
		{
			*GetEffectivePointer<UInt16>(state, inst) = static_cast<uint16_t>(state->gpr[inst.RS]);
		}

		void Interpreter::sthbrx(Instruction inst)
		{
			uint16_t halfWord = EndianU16_NtoL(static_cast<uint16_t>(state->gpr[inst.RS]));
			*GetEffectivePointerX<uint16_t>(state, inst) = halfWord;
		}

		void Interpreter::sthu(Instruction inst)
		{
			UInt16* address = GetEffectivePointerU<UInt16>(state, inst);
			*address = static_cast<uint16_t>(state->gpr[inst.RS]);
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::sthux(Instruction inst)
		{
			UInt16* address = GetEffectivePointerUX<UInt16>(state, inst);
			*address = static_cast<uint16_t>(state->gpr[inst.RS]);
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::sthx(Instruction inst)
		{
			*GetEffectivePointerX<UInt16>(state, inst) = static_cast<uint16_t>(state->gpr[inst.RS]);
		}

		void Interpreter::stmw(Instruction inst)
		{
			UInt32* address = GetEffectivePointer<UInt32>(state, inst);
			for (int i = inst.RS; i < 32; i++)
			{
				*address = state->gpr[i];
				address++;
			}
		}

		void Interpreter::stswi(Instruction inst)
		{
			uint8_t* EA = inst.RA == 0 ? nullptr : reinterpret_cast<uint8_t*>(state->gpr[inst.RA]);

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
				
				*EA = static_cast<uint8_t>(state->gpr[r] >> (24 - i));
				
				i += 8;
				if (i == 32)
					i = 0;
				EA++;
				n--;
			}
		}

		void Interpreter::stswx(Instruction inst)
		{
			uint8_t* EA = GetEffectivePointerX<uint8_t>(state, inst);
			uint32_t n = state->xer & 0x7F;
			int r = inst.RS;
			int i = 0;

			while (n > 0)
			{
				*EA = static_cast<uint8_t>(state->gpr[r] >> (24 - i));

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
			*GetEffectivePointer<UInt32>(state, inst) = state->gpr[inst.RS];
		}

		void Interpreter::stwbrx(Instruction inst)
		{
			*GetEffectivePointerX<uint32_t>(state, inst) = EndianU32_NtoL(state->gpr[inst.RS]);
		}

		void Interpreter::stwcxd(Instruction inst)
		{
			// since we emulate only one CPU, treat stwcxd as a regular store
			*GetEffectivePointerX<UInt32>(state, inst) = state->gpr[inst.RS];
			state->cr[0] = state->xer_so;
		}

		void Interpreter::stwu(Instruction inst)
		{
			UInt32* address = GetEffectivePointerU<UInt32>(state, inst);
			*address = state->gpr[inst.RS];
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::stwux(Instruction inst)
		{
			UInt32* address = GetEffectivePointerUX<UInt32>(state, inst);
			*address = state->gpr[inst.RS];
			state->gpr[inst.RA] = ToIntPtr(address);
		}

		void Interpreter::stwx(Instruction inst)
		{
			*GetEffectivePointerX<UInt32>(state, inst) = state->gpr[inst.RS];
		}

		void Interpreter::sync(Instruction inst)
		{
			__sync_synchronize();
		}		
	}
}