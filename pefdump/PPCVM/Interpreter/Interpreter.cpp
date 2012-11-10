#include "Interpreter.h"
#include "Disassembler.h"
#include "NativeCall.h"
#include <iostream>

#ifdef DEBUG
# define CHECK_JUMP_TARGET()	(*(uint8_t*)branchAddress = *(uint8_t*)branchAddress)
#else
# define CHECK_JUMP_TARGET()
#endif

namespace
{
	// If the interpreter reaches the address of that symbol, it knows that it needs to stop interpreting.
	uint32_t EndLocation = 0xdeaddead;
	
	template<typename T>
	struct EndAddress
	{
		static const T Value;
	};
	
	template<typename T>
	const T EndAddress<T>::Value = reinterpret_cast<T>(&EndLocation);
	
	inline int32_t SignExt16(int16_t x)
	{
		return x;
	}
	
	inline int32_t SignExt26(uint32_t x)
	{
		return (x & 0x02000000) ? (x | 0xfc000000) : x;
	}
	
	inline bool GetCRBit(MachineState* state, int bit)
	{
		return (state->cr[bit >> 2] >> (3 - (bit & 3))) & 1;
	}
	
	enum 
	{
		BO_BRANCH_IF_CTR_0		=  2, // 3
		BO_DONT_DECREMENT_FLAG	=  4, // 2
		BO_BRANCH_IF_TRUE		=  8, // 1
		BO_DONT_CHECK_CONDITION	= 16, // 0
	};
}

namespace PPCVM
{
	namespace Execution
	{
		Interpreter::Interpreter(MachineState* state)
		: state(state)
		{ }

		void Interpreter::Panic(const std::string& error)
		{
			std::cerr << error << std::endl;
			abort(); // now that's an ugly panic
		}

		void Interpreter::unknown(Instruction inst)
		{
			Disassembler::DisassembledInstruction disassembly;
			if (Disassembler::Disassemble(inst.hex, disassembly))
			{
				Panic("Unknown instruction " + disassembly.Opcode + " " + disassembly.Arguments);
			}
			else
			{
				Panic("Unknown instruction");
			}
		}
		
		const void* Interpreter::ExecuteNative(const void *address)
		{
			NativeCall function = reinterpret_cast<NativeCall>(const_cast<void*>(address));
			function(state);
			return reinterpret_cast<const void*>(state->lr);
		}

		const void* Interpreter::ExecuteUntilBranch(const void* address)
		{
			currentAddress = reinterpret_cast<const Common::UInt32*>(address);
			branchAddress = nullptr;
			do
			{
				uint32_t instructionCode = *currentAddress;
				if (instructionCode == 0xffffffff)
				{
					branchAddress = ExecuteNative(currentAddress + 1);
				}
				else
				{
					Dispatch(instructionCode);
					currentAddress++;
				}
			} while (branchAddress == nullptr);
			return branchAddress;
		}

		void Interpreter::Execute(const void* address)
		{
			state->gpr[31] = EndAddress<uint32_t>::Value;
			while (address != EndAddress<const void*>::Value)
			{
				address = ExecuteUntilBranch(address);
			}
		}

		void Interpreter::bx(Instruction inst)
		{
			intptr_t address = reinterpret_cast<intptr_t>(currentAddress);
			if (inst.LK)
				state->lr = address + 4;
			
			intptr_t target = SignExt26(inst.LI << 2);
			if (!inst.AA)
				target += address;
			
			branchAddress = reinterpret_cast<const void*>(target);
			CHECK_JUMP_TARGET();
		}

		void Interpreter::bcx(Instruction inst)
		{
			if ((inst.BO & BO_DONT_DECREMENT_FLAG) == 0)
				state->ctr--;

			const bool true_false = ((inst.BO >> 3) & 1);
			const bool only_counter_check = ((inst.BO >> 4) & 1);
			const bool only_condition_check = ((inst.BO >> 2) & 1);
			int ctr_check = ((state->ctr != 0) ^ (inst.BO >> 1)) & 1;
			bool counter = only_condition_check || ctr_check;
			bool condition = only_counter_check || (GetCRBit(state, inst.BI) == uint32_t(true_false));
			
			intptr_t address = reinterpret_cast<intptr_t>(currentAddress);
			if (counter && condition)
			{
				if (inst.LK)
					state->lr = address + 4;
					
				intptr_t target = SignExt16(inst.BD << 2);
				if (!inst.AA)
					target += address;
				branchAddress = reinterpret_cast<const void*>(target);
			}
			CHECK_JUMP_TARGET();
		}

		void Interpreter::bclrx(Instruction inst)
		{
			if ((inst.BO_2 & BO_DONT_DECREMENT_FLAG) == 0)
				state->ctr--;

			int counter = ((inst.BO_2 >> 2) | ((state->ctr != 0) ^ (inst.BO_2 >> 1))) & 1;
			int condition = ((inst.BO_2 >> 4) | (GetCRBit(state, inst.BI_2) == ((inst.BO_2 >> 3) & 1))) & 1;

			if (counter & condition)
			{
				branchAddress = reinterpret_cast<const void*>(state->lr & ~3);
				if (inst.LK_3)
					state->lr = reinterpret_cast<intptr_t>(currentAddress + 1);
			}
			CHECK_JUMP_TARGET();
		}

		void Interpreter::bcctrx(Instruction inst)
		{
			int condition = ((inst.BO_2>>4) | (GetCRBit(state, inst.BI_2) == ((inst.BO_2>>3) & 1))) & 1;

			if (condition)
			{
				if (inst.LK_3)
					state->lr = reinterpret_cast<intptr_t>(currentAddress + 1);
				
				branchAddress = reinterpret_cast<const void*>(state->ctr & ~3);
			}
			CHECK_JUMP_TARGET();
		}

		void Interpreter::sc(Instruction inst)
		{
			abort();
		}

		// supervisor mode instructions (not implemented)
		void Interpreter::dcba(Instruction inst)
		{
			abort();
		}

		void Interpreter::dcbf(Instruction inst)
		{
			abort();
		}

		void Interpreter::dcbst(Instruction inst)
		{
			abort();
		}

		void Interpreter::dcbt(Instruction inst)
		{
			abort();
		}

		void Interpreter::dcbtst(Instruction inst)
		{
			abort();
		}

		void Interpreter::dcbz(Instruction inst)
		{
			abort();
		}

		void Interpreter::dcbi(Instruction inst)
		{
			abort();
		}

		void Interpreter::eciwx(Instruction inst)
		{
			abort();
		}

		void Interpreter::ecowx(Instruction inst)
		{
			abort();
		}

		void Interpreter::icbi(Instruction inst)
		{
			abort();
		}

		void Interpreter::mfmsr(Instruction inst)
		{
			abort();
		}

		void Interpreter::mfsr(Instruction inst)
		{
			abort();
		}

		void Interpreter::mfsrin(Instruction inst)
		{
			abort();
		}

		void Interpreter::mtmsr(Instruction inst)
		{
			abort();
		}

		void Interpreter::mtsr(Instruction inst)
		{
			abort();
		}

		void Interpreter::mtsrin(Instruction inst)
		{
			abort();
		}

		void Interpreter::rfi(Instruction inst)
		{
			abort();
		}

		void Interpreter::tlbia(Instruction inst)
		{
			abort();
		}

		void Interpreter::tlbie(Instruction inst)
		{
			abort();
		}

		void Interpreter::tlbsync(Instruction inst)
		{
			abort();
		}
	}
}
