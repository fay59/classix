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
#include "InvalidInstructionException.h"
#include "NativeCall.h"
#include "PanicException.h"
#include "TrapException.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <dlfcn.h>
#include "Todo.h"

using namespace Common;

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
	
	inline bool GetCRBit(MachineState& state, int bit)
	{
		return (state.cr[bit / 4] >> (3 - (bit & 3))) & 1;
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
		Interpreter::Interpreter(Allocator& allocator, MachineState& state)
		: state(state), allocator(allocator),
			endAddress(allocator.AllocateAuto("Interpreter End Address", 4)),
			interruptAddress(allocator.AllocateAuto("Interpreter Interrupt Address", 4))
		{ }
		
		Interpreter::~Interpreter()
		{ }
		
		const UInt32* Interpreter::GetEndAddress() const
		{
			return static_cast<const UInt32*>(*endAddress);
		}

		void Interpreter::Panic(const std::string& error)
		{
			throw PanicException(error);
		}

		void Interpreter::unknown(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::SetBranchAddress(uint32_t target)
		{
			const UInt32* voidTarget = allocator.ToPointer<UInt32>(target);
			const UInt32* oldBranch = branchAddress.exchange(voidTarget);
			
			// We know Interrupt() was called if oldBranch is not null at this point. Normally this is picked up
			// at branching time (with ExecuteUntilBranch returning), but if we're inside a branch instruction,
			// we risk overwriting the branch, so check here too. Also, we can't risk letting the execution loop
			// increment PC if we're not going to branch.
			if (oldBranch != nullptr)
			{
				branchAddress.store(oldBranch);
				throw TrapException("interrupted");
			}
		}
		
		void Interpreter::Interrupt()
		{
			const UInt32* expected = nullptr;
			const UInt32* target = static_cast<UInt32*>(*interruptAddress);
			
			// Don't stop if a branch is already happening. I can see a lot of things going wrong
			// if this happens. Just try again until we're good, it shouldn't take long anyways.
			// Spurious fails are OK because the interpreter shouldn't be back on its feet before the interrupting
			// thread gives its go anyways.
			while (!branchAddress.compare_exchange_weak(expected, target))
				expected = nullptr;
		}
		
		const UInt32* Interpreter::ExecuteNative(const NativeCall* function)
		{
			if (function->Tag != NativeTag)
			{
				Panic("Call doesn't have a native tag");
			}
			
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
			
			void* libGlobals = allocator.ToPointer<void>(state.r2);
			function->Callback(libGlobals, &state);
			return allocator.ToPointer<UInt32>(state.lr);
		}

		void Interpreter::ExecuteUntilBranch(const UInt32* address)
		{
			currentAddress = address;
			branchAddress = nullptr;
			do
			{
				try
				{
					if (currentAddress->AsBigEndian == NativeTag)
					{
						const NativeCall* call = reinterpret_cast<const NativeCall*>(currentAddress);
						const UInt32* returnAddress = ExecuteNative(call);
						
						// Native calls absolutely have to be atomic, so if we were interrupted in the middle of one,
						// set currentAddress to the branch address and then give up. Otherwise set branchAddress
						// to the return address and keep going.
						const void* oldBranch = branchAddress.exchange(returnAddress);
						if (oldBranch == *interruptAddress)
						{
							currentAddress = returnAddress;
							throw TrapException("interrupted");
						}
					}
					else
					{
						Instruction instruction = currentAddress->Get();
						Dispatch(instruction);
						currentAddress++;
					}
				}
				catch (PPCRuntimeException& ex)
				{
					uint32_t pc = allocator.ToIntPtr(currentAddress);
					throw InterpreterException(pc, ex);
				}
			} while (branchAddress == nullptr);
		}
		
		UInt32* Interpreter::ExecuteOne(UInt32* address)
		{
			return const_cast<UInt32*>(ExecuteOne(static_cast<const UInt32*>(address)));
		}
		
		UInt32* Interpreter::ExecuteOne(UInt32 *address, Instruction instruction)
		{
			return const_cast<UInt32*>(ExecuteOne(static_cast<const UInt32*>(address), instruction));
		}
		
		const UInt32* Interpreter::ExecuteOne(const UInt32 *address)
		{
			// ExecuteOne is not interruptible
			currentAddress = address;
			branchAddress = nullptr;
			
			try
			{
				if (address->AsBigEndian == NativeTag)
				{
					const NativeCall* call = reinterpret_cast<const NativeCall*>(address);
					branchAddress = ExecuteNative(call);
				}
				else
				{
					Instruction instruction(currentAddress->Get());
					Dispatch(instruction);
					currentAddress++;
				}
			}
			catch (PPCRuntimeException& ex)
			{
				uint32_t pc = allocator.ToIntPtr(currentAddress);
				throw InterpreterException(pc, ex);
			}
			
			const UInt32* br = branchAddress.load(std::memory_order_relaxed);
			return br == nullptr ? currentAddress : br;
		}
		
		const UInt32* Interpreter::ExecuteOne(const UInt32 *baseAddress, Instruction instruction)
		{
			// ExecuteOne is not interruptible
			currentAddress = baseAddress;
			branchAddress = nullptr;
			
			try
			{
				assert(instruction.hex != NativeTag && "Cannot simulate a native call");
				Dispatch(instruction);
				currentAddress++;
			}
			catch (PPCRuntimeException& ex)
			{
				uint32_t pc = allocator.ToIntPtr(currentAddress);
				throw InterpreterException(pc, ex);
			}
			
			const UInt32* br = branchAddress.load(std::memory_order_relaxed);
			return br == nullptr ? currentAddress : br;
		}

		void Interpreter::Execute(const UInt32* address)
		{
			const void* interrupt = *interruptAddress;
			state.lr = endAddress.GetVirtualAddress();
			while (address != *endAddress)
			{
				ExecuteUntilBranch(address);
				
				address = branchAddress.load();
				if (address == interrupt)
					throw TrapException("interrupted");
			}
		}

		void Interpreter::bx(Instruction inst)
		{
			uint32_t address = allocator.ToIntPtr(currentAddress);
			if (inst.LK)
				state.lr = address + 4;
			
			uint32_t target = SignExt26(inst.LI << 2);
			if (!inst.AA)
				target += address;
			
			SetBranchAddress(target);
		}

		void Interpreter::bcx(Instruction inst)
		{
			if ((inst.BO & BO_DONT_DECREMENT_FLAG) == 0)
				state.ctr--;

			const bool true_false = ((inst.BO >> 3) & 1);
			const bool only_counter_check = ((inst.BO >> 4) & 1);
			const bool only_condition_check = ((inst.BO >> 2) & 1);
			int ctr_check = ((state.ctr != 0) ^ (inst.BO >> 1)) & 1;
			bool counter = only_condition_check || ctr_check;
			bool condition = only_counter_check || (GetCRBit(state, inst.BI) == uint32_t(true_false));
			
			uint32_t address = allocator.ToIntPtr(currentAddress);
			if (counter && condition)
			{
				if (inst.LK)
					state.lr = address + 4;
				
				int16_t bd = static_cast<int16_t>(inst.BD << 2);
				uint32_t target = SignExt16(bd);
				if (!inst.AA)
					target += address;
				SetBranchAddress(target);
			}
		}

		void Interpreter::bclrx(Instruction inst)
		{
			if ((inst.BO & BO_DONT_DECREMENT_FLAG) == 0)
				state.ctr--;

			int counter = ((inst.BO >> 2) | ((state.ctr != 0) ^ (inst.BO >> 1))) & 1;
			int condition = ((inst.BO >> 4) | (GetCRBit(state, inst.BI) == ((inst.BO >> 3) & 1))) & 1;

			if (counter & condition)
			{
				if (inst.LK)
					state.lr = allocator.ToIntPtr(currentAddress + 1);
				
				SetBranchAddress(state.lr & ~3);
			}
		}

		void Interpreter::bcctrx(Instruction inst)
		{
			int condition = ((inst.BO>>4) | (GetCRBit(state, inst.BI) == ((inst.BO>>3) & 1))) & 1;

			if (condition)
			{
				if (inst.LK)
					state.lr = allocator.ToIntPtr(currentAddress + 1);
				
				SetBranchAddress(state.ctr & ~3);
			}
		}

		void Interpreter::sc(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		// supervisor mode instructions (not implemented)
		void Interpreter::dcba(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::dcbf(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::dcbst(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::dcbt(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::dcbtst(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::dcbz(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::dcbi(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::eciwx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::ecowx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::icbi(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::mfmsr(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::mfsr(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::mfsrin(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::mtmsr(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::mtsr(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::mtsrin(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::rfi(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::tlbia(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::tlbie(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::tlbsync(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
	}
}
