//
// Interpreter.cpp
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
#include "InstructionDecoder.h"
#include "NativeCall.h"
#include "PanicException.h"
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
		return (state->cr[bit / 4] >> (3 - (bit & 3))) & 1;
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
		: state(state), allocator(allocator), endAddress(allocator->AllocateAuto("Interpreter End Address", 4))
		{ }

		void Interpreter::Panic(const std::string& error)
		{
			throw PanicException(error);
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
			
#ifdef DEBUG_DISASSEMBLE
			if (getenv("DEBUG_DISASSEMBLE"))
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
				try
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
						Dispatch(inst);
						currentAddress++;
					}
				}
				catch (Common::PPCRuntimeException& ex)
				{
					uint32_t pc = allocator->ToIntPtr(const_cast<Common::UInt32*>(currentAddress));
					throw InterpreterException(pc, ex);
				}
			} while (branchAddress == nullptr);
			return branchAddress;
		}
		
		const void* Interpreter::ExecuteOne(const void *address)
		{
			currentAddress = reinterpret_cast<const Common::UInt32*>(address);
			branchAddress = nullptr;
			
			try
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
					Dispatch(inst);
					currentAddress++;
				}
			}
			catch (Common::PPCRuntimeException& ex)
			{
				uint32_t pc = allocator->ToIntPtr(const_cast<Common::UInt32*>(currentAddress));
				throw InterpreterException(pc, ex);
			}
			
			return branchAddress == nullptr ? currentAddress : branchAddress;
		}

		void Interpreter::Execute(const void* address)
		{
			state->lr = endAddress.GetVirtualAddress();
			while (address != *endAddress)
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
			Panic("sc is not implemented");
		}
		
		// supervisor mode instructions (not implemented)
		void Interpreter::dcba(Instruction inst)
		{
			Panic("dcba is not implemented");
		}
		
		void Interpreter::dcbf(Instruction inst)
		{
			Panic("dcbf is not implemented");
		}
		
		void Interpreter::dcbst(Instruction inst)
		{
			Panic("dcbst is not implemented");
		}
		
		void Interpreter::dcbt(Instruction inst)
		{
			Panic("dcbt is not implemented");
		}
		
		void Interpreter::dcbtst(Instruction inst)
		{
			Panic("dcbtst is not implemented");
		}
		
		void Interpreter::dcbz(Instruction inst)
		{
			Panic("dcbz is not implemented");
		}
		
		void Interpreter::dcbi(Instruction inst)
		{
			Panic("dcbi is not implemented");
		}
		
		void Interpreter::eciwx(Instruction inst)
		{
			Panic("eciwx is not implemented");
		}
		
		void Interpreter::ecowx(Instruction inst)
		{
			Panic("ecowx is not implemented");
		}
		
		void Interpreter::icbi(Instruction inst)
		{
			Panic("icbi is not implemented");
		}
		
		void Interpreter::mfmsr(Instruction inst)
		{
			Panic("mfmsr is not implemented");
		}
		
		void Interpreter::mfsr(Instruction inst)
		{
			Panic("mfsr is not implemented");
		}
		
		void Interpreter::mfsrin(Instruction inst)
		{
			Panic("mfsrin is not implemented");
		}
		
		void Interpreter::mtmsr(Instruction inst)
		{
			Panic("mtmsr is not implemented");
		}
		
		void Interpreter::mtsr(Instruction inst)
		{
			Panic("mtsr is not implemented");
		}
		
		void Interpreter::mtsrin(Instruction inst)
		{
			Panic("mtsrin is not implemented");
		}
		
		void Interpreter::rfi(Instruction inst)
		{
			Panic("rfi is not implemented");
		}
		
		void Interpreter::tlbia(Instruction inst)
		{
			Panic("tlbia is not implemented");
		}
		
		void Interpreter::tlbie(Instruction inst)
		{
			Panic("tlbie is not implemented");
		}
		
		void Interpreter::tlbsync(Instruction inst)
		{
			Panic("tlbsync is not implemented");
		}
	}
}
