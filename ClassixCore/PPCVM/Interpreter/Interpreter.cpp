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
#include "TrapException.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <dlfcn.h>
#include "Todo.h"

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
		Interpreter::Interpreter(Common::Allocator& allocator, MachineState& state)
		: state(state), allocator(allocator),
			endAddress(allocator.AllocateAuto("Interpreter End Address", 4)),
			interruptAddress(allocator.AllocateAuto("Interpreter Interrupt Address", 4))
		{ }
		
		Interpreter::~Interpreter()
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
		
		void Interpreter::SetBranchAddress(uint32_t target)
		{
			const void* voidTarget = allocator.ToPointer<const void>(target);
			const void* oldBranch = branchAddress.exchange(voidTarget);
			
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
			const void* expected = nullptr;
			const void* target = *interruptAddress;
			
			// Don't stop if a branch is already happening. I can see a lot of things going wrong
			// if this happens. Just try again until we're good, it shouldn't take long anyways.
			// Spurious fails are OK because the interpreter shouldn't be back on its feet before the interrupting
			// thread gives its go anyways.
			while (!branchAddress.compare_exchange_weak(expected, target))
				expected = nullptr;
		}
		
		const void* Interpreter::ExecuteNative(const NativeCall* function)
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
			return allocator.ToPointer<const void>(state.lr);
		}

		void Interpreter::ExecuteUntilBranch(const void* address)
		{
			currentAddress = reinterpret_cast<const Common::UInt32*>(address);
			branchAddress = nullptr;
			do
			{
				try
				{
					Common::UInt32 instructionCode = *currentAddress;
					if (instructionCode.AsBigEndian == NativeTag)
					{
						const NativeCall* call = static_cast<const NativeCall*>(address);
						const void* returnAddress = ExecuteNative(call);
						
						// Native calls absolutely have to be atomic, so if we were interrupted in the middle of one,
						// set currentAddress to the branch address and then give up. Otherwise set branchAddress
						// to the return address and keep going.
						const void* oldBranch = branchAddress.exchange(returnAddress);
						if (oldBranch == *interruptAddress)
						{
							currentAddress = reinterpret_cast<const Common::UInt32*>(returnAddress);
							throw TrapException("interrupted");
						}
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
					uint32_t pc = allocator.ToIntPtr(currentAddress);
					throw InterpreterException(pc, ex);
				}
			} while (branchAddress == nullptr);
		}
		
		const void* Interpreter::ExecuteOne(const void *address)
		{
			// ExecuteOne is not interruptible
			currentAddress = reinterpret_cast<const Common::UInt32*>(address);
			branchAddress = nullptr;
			
			try
			{
				Common::UInt32 instructionCode = *currentAddress;
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
				uint32_t pc = allocator.ToIntPtr(currentAddress);
				throw InterpreterException(pc, ex);
			}
			
			const void* br = branchAddress.load(std::memory_order_relaxed);
			return br == nullptr ? currentAddress : br;
		}

		void Interpreter::Execute(const void* address)
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
