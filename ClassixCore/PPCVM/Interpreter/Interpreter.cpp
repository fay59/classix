#include "Interpreter.h"
#include "Disassembler.h"
#include "NativeCall.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <dlfcn.h>

#ifdef DEBUG
static uint8_t _writeBucket_;
# define CHECK_JUMP_TARGET()	(_writeBucket_ = *(uint8_t*)branchAddress)
#else
# define CHECK_JUMP_TARGET()
#endif

namespace
{
	using namespace PPCVM;
	
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
	
	const char* BaseName(const char* path)
	{
		const char* lastPathComponent = path;
		while (*path != 0)
		{
			if (*path == '/')
				lastPathComponent = path + 1;
			path++;
		}
		return lastPathComponent;
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
		Interpreter::Interpreter(Common::IAllocator* allocator, MachineState* state)
		: state(state), allocator(allocator)
		{ }

		void Interpreter::Panic(const std::string& error)
		{
			std::cerr << error << std::endl;
			abort(); // now that's an ugly panic
		}

		void Interpreter::unknown(Instruction inst)
		{
			std::stringstream ss;
			ss << "Unknown instruction " << Disassembly::InstructionDecoder::Decode(inst);
			Panic(ss.str());
		}
		
		const void* Interpreter::ExecuteNative(const NativeCall* function)
		{
			assert(function->Tag == NativeTag && "Call doesn't have native tag");
			
#ifdef DEBUG_TRACE_NATIVE
			if (getenv("DEBUG_TRACE_NATIVE"))
			{
				Dl_info symInfo;
				if (dladdr((const void*)function->Callback, &symInfo) != 0)
				{
					if (symInfo.dli_sname != nullptr)
					{
						std::cerr << "\t> Calling into [" << BaseName(symInfo.dli_fname) << "::" << symInfo.dli_sname << "]" << std::endl;
					}
					else
					{
						std::cerr << "\t> Calling into unidentified symbol from [" << symInfo.dli_fname << "]" << std::endl;
					}
				}
				else
				{
					std::cerr << "\t> Calling into unknown native function" << std::endl;
				}
			}
#endif
			
			void* libGlobals = allocator->ToPointer<void>(state->r2);
			function->Callback(libGlobals, state);
			return allocator->ToPointer<const void>(state->lr);
		}

		const void* Interpreter::ExecuteUntilBranch(const void* address)
		{
			currentAddress = reinterpret_cast<const Common::UInt32*>(address);
			branchAddress = nullptr;
			do
			{
				const Common::UInt32& instructionCode = *currentAddress;
				if (instructionCode.AsBigEndian == NativeTag)
				{
					const NativeCall* call = static_cast<const NativeCall*>(address);
					branchAddress = ExecuteNative(call);
				}
				else
				{
					Instruction inst = instructionCode.Get();
#ifdef DEBUG_DISASSEMBLE
					if (getenv("DEBUG_DISASSEMBLE"))
					{
						std::cerr << '\t' << Disassembly::InstructionDecoder::Decode(inst) << std::endl;
					}
#endif
					
					Dispatch(inst);
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
				CHECK_JUMP_TARGET();
			}
		}

		void Interpreter::bclrx(Instruction inst)
		{
			if ((inst.BO_2 & BO_DONT_DECREMENT_FLAG) == 0)
				state->ctr--;

			int counter = ((inst.BO_2 >> 2) | ((state->ctr != 0) ^ (inst.BO_2 >> 1))) & 1;
			int condition = ((inst.BO_2 >> 4) | (GetCRBit(state, inst.BI_2) == ((inst.BO_2 >> 3) & 1))) & 1;

			if (counter & condition)
			{
				if (inst.LK_3)
					state->lr = reinterpret_cast<intptr_t>(currentAddress + 1);
				
				branchAddress = reinterpret_cast<const void*>(state->lr & ~3);
				CHECK_JUMP_TARGET();
			}
		}

		void Interpreter::bcctrx(Instruction inst)
		{
			int condition = ((inst.BO_2>>4) | (GetCRBit(state, inst.BI_2) == ((inst.BO_2>>3) & 1))) & 1;

			if (condition)
			{
				if (inst.LK_3)
					state->lr = reinterpret_cast<intptr_t>(currentAddress + 1);
				
				branchAddress = reinterpret_cast<const void*>(state->ctr & ~3);
				CHECK_JUMP_TARGET();
			}
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
