//
// Interpreter.h
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

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "NativeCall.h"
#include "IAllocator.h"
#include "MachineState.h"
#include "Instruction.h"
#include "InstructionDispatcher.h"
#include "BigEndian.h"
#include <string>
#include <iostream>
#include "PPCRuntimeException.h"
#include "InterpreterException.h"

namespace PPCVM
{
	namespace Execution
	{
		class Interpreter : public InstructionDispatcher<Interpreter>
		{
			MachineState* state;
			Common::IAllocator* allocator;
			const Common::UInt32* currentAddress;
			const void* branchAddress;
			const void* ExecuteUntilBranch(const void* address);
			const void* ExecuteNative(const NativeCall* address);
			
		public:
			Interpreter(Common::IAllocator* allocator, MachineState* state);
			
			void Execute(const void* address);
			const void* ExecuteOne(const void* address);
			
			template<typename TBreakpointSet>
			const void* ExecuteUntil(const void* address, const TBreakpointSet& breakpoints);
			
			// problems
			void Panic(const std::string& errorMessage);
			void unknown(Instruction inst);

			// Floating Point Instructions
			void fabsx(Instruction inst);
			void faddsx(Instruction inst);
			void faddx(Instruction inst);
			void fcmpo(Instruction inst);
			void fcmpu(Instruction inst);
			void fctiwx(Instruction inst);
			void fctiwzx(Instruction inst);
			void fdivsx(Instruction inst);
			void fdivx(Instruction inst);
			void fmaddsx(Instruction inst);
			void fmaddx(Instruction inst);
			void fmrx(Instruction inst);
			void fmsubsx(Instruction inst);
			void fmsubx(Instruction inst);
			void fmulsx(Instruction inst);
			void fmulx(Instruction inst);
			void fnabsx(Instruction inst);
			void fnegx(Instruction inst);
			void fnmaddsx(Instruction inst);
			void fnmaddx(Instruction inst);
			void fnmsubsx(Instruction inst);
			void fnmsubx(Instruction inst);
			void fresx(Instruction inst);
			void frspx(Instruction inst);
			void frsqrtex(Instruction inst);
			void fselx(Instruction inst);
			void fsqrtsx(Instruction inst);
			void fsqrtx(Instruction inst);
			void fsubsx(Instruction inst);
			void fsubx(Instruction inst);

			// Integer Instructions
			void addi(Instruction inst);
			void addic(Instruction inst);
			void addic_rc(Instruction inst);
			void addis(Instruction inst);
			void andi_rc(Instruction inst);
			void andis_rc(Instruction inst);
			void cmpi(Instruction inst);
			void cmpli(Instruction inst);
			void mulli(Instruction inst);
			void ori(Instruction inst);
			void oris(Instruction inst);
			void subfic(Instruction inst);
			void twi(Instruction inst);
			void xori(Instruction inst);
			void xoris(Instruction inst);
			void rlwimix(Instruction inst);
			void rlwinmx(Instruction inst);
			void rlwnmx(Instruction inst);
			void andx(Instruction inst);
			void andcx(Instruction inst);
			void cmp(Instruction inst);
			void cmpl(Instruction inst);
			void cntlzwx(Instruction inst);
			void eqvx(Instruction inst);
			void extsbx(Instruction inst);
			void extshx(Instruction inst);
			void nandx(Instruction inst);
			void norx(Instruction inst);
			void orx(Instruction inst);
			void orcx(Instruction inst);
			void slwx(Instruction inst);
			void srawx(Instruction inst);
			void srawix(Instruction inst);
			void srwx(Instruction inst);
			void tw(Instruction inst);
			void xorx(Instruction inst);
			void addx(Instruction inst);
			void addcx(Instruction inst);
			void addex(Instruction inst);
			void addmex(Instruction inst);
			void addzex(Instruction inst);
			void divwx(Instruction inst);
			void divwux(Instruction inst);
			void mulhwx(Instruction inst);
			void mulhwux(Instruction inst);
			void mullwx(Instruction inst);
			void negx(Instruction inst);
			void subfx(Instruction inst);
			void subfcx(Instruction inst);
			void subfex(Instruction inst);
			void subfmex(Instruction inst);
			void subfzex(Instruction inst);

			// Load/Store Instructions
			void lbz(Instruction inst);
			void lbzu(Instruction inst);
			void lfd(Instruction inst);
			void lfdu(Instruction inst);
			void lfs(Instruction inst);
			void lfsu(Instruction inst);
			void lha(Instruction inst);
			void lhau(Instruction inst);
			void lhz(Instruction inst);
			void lhzu(Instruction inst);
			void lmw(Instruction inst);
			void lwz(Instruction inst);
			void lwzu(Instruction inst);
			void stb(Instruction inst);
			void stbu(Instruction inst);
			void stfd(Instruction inst);
			void stfdu(Instruction inst);
			void stfs(Instruction inst);
			void stfsu(Instruction inst);
			void sth(Instruction inst);
			void sthu(Instruction inst);
			void stmw(Instruction inst);
			void stw(Instruction inst);
			void stwu(Instruction inst);
			void eieio(Instruction inst);
			void lbzux(Instruction inst);
			void lbzx(Instruction inst);
			void lfdux(Instruction inst);
			void lfdx(Instruction inst);
			void lfsux(Instruction inst);
			void lfsx(Instruction inst);
			void lhaux(Instruction inst);
			void lhax(Instruction inst);
			void lhbrx(Instruction inst);
			void lhzux(Instruction inst);
			void lhzx(Instruction inst);
			void lswi(Instruction inst);
			void lswx(Instruction inst);
			void lwarx(Instruction inst);
			void lwbrx(Instruction inst);
			void lwzux(Instruction inst);
			void lwzx(Instruction inst);
			void stbux(Instruction inst);
			void stbx(Instruction inst);
			void stfdux(Instruction inst);
			void stfdx(Instruction inst);
			void stfiwx(Instruction inst);
			void stfsux(Instruction inst);
			void stfsx(Instruction inst);
			void sthbrx(Instruction inst);
			void sthux(Instruction inst);
			void sthx(Instruction inst);
			void stswi(Instruction inst);
			void stswx(Instruction inst);
			void stwbrx(Instruction inst);
			void stwcxd(Instruction inst);
			void stwux(Instruction inst);
			void stwx(Instruction inst);

			// System Registers Instructions
			void mcrfs(Instruction inst);
			void mffsx(Instruction inst);
			void mtfsb0x(Instruction inst);
			void mtfsb1x(Instruction inst);
			void mtfsfix(Instruction inst);
			void mtfsfx(Instruction inst);
			void mcrxr(Instruction inst);
			void mfcr(Instruction inst);
			void mfspr(Instruction inst);
			void mftb(Instruction inst);
			void mtcrf(Instruction inst);
			void mtspr(Instruction inst);
			void crand(Instruction inst);
			void crandc(Instruction inst);
			void creqv(Instruction inst);
			void crnand(Instruction inst);
			void crnor(Instruction inst);
			void cror(Instruction inst);
			void crorc(Instruction inst);
			void crxor(Instruction inst);
			void mcrf(Instruction inst);
			void rfid(Instruction inst);
			void sync(Instruction inst);
			void isync(Instruction inst);
			
			// branching
			void bx(Instruction inst);
			void bcx(Instruction inst);
			void bcctrx(Instruction inst);
			void bclrx(Instruction inst);
			void sc(Instruction inst);
			
			// supervisor mode (not implemented)
			void dcba(Instruction inst);
			void dcbf(Instruction inst);
			void dcbi(Instruction inst);
			void dcbst(Instruction inst);
			void dcbt(Instruction inst);
			void dcbtst(Instruction inst);
			void dcbz(Instruction inst);
			void eciwx(Instruction inst);
			void ecowx(Instruction inst);
			void icbi(Instruction inst);
			void mfmsr(Instruction inst);
			void mfsr(Instruction inst);
			void mfsrin(Instruction inst);
			void mtmsr(Instruction inst);
			void mtsr(Instruction inst);
			void mtsrin(Instruction inst);
			void rfi(Instruction inst);
			void tlbia(Instruction inst);
			void tlbie(Instruction inst);
			void tlbsync(Instruction inst);
		};
		
		template<typename TBreakpointSet>
		const void* Interpreter::ExecuteUntil(const void *address, const TBreakpointSet &breakpoints)
		{
			currentAddress = reinterpret_cast<const Common::UInt32*>(address);
			branchAddress = nullptr;
			
			do
			{
				const Common::UInt32& instructionCode = *currentAddress;
				
				if (instructionCode.AsBigEndian == NativeTag)
				{
					const NativeCall* call = reinterpret_cast<const NativeCall*>(currentAddress);
					branchAddress = ExecuteNative(call);
				}
				else
				{
					Instruction inst = instructionCode.Get();
					try
					{
						Dispatch(inst);
						currentAddress++;
					}
					catch (Common::PPCRuntimeException& ex)
					{
						uint32_t pc = allocator->ToIntPtr(currentAddress);
						throw InterpreterException(pc, ex);
					}
				}
				
				if (branchAddress != nullptr)
				{
					currentAddress = reinterpret_cast<const Common::UInt32*>(branchAddress);
					branchAddress = nullptr;
				}
			}
			while (breakpoints.count(currentAddress) == 0);
			return currentAddress;
		}
	}
}

#endif