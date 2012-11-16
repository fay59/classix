//
//  InstructionRange.h
//  Classix
//
//  Created by Félix on 2012-11-12.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__InstructionRange__
#define __Classix__InstructionRange__

#include <string>
#include <vector>
#include <iostream>

#include "InstructionDispatcher.h"
#include "BigEndian.h"
#include "IAllocator.h"
#include "Structures.h"

namespace PPCVM
{
	namespace Disassembly
	{
		struct DisassembledOpcode
		{
			std::string Opcode;
			std::vector<std::string> Arguments;
			std::string Complement;
			
			template<typename... TParams>
			DisassembledOpcode(const std::string& opcode, TParams... arguments)
			: Opcode(opcode)
			{
				AddArguments(arguments...);
			}
			
		private:
			template<typename... TParams>
			void AddArguments(const std::string& arg, TParams... args)
			{
				Arguments.push_back(arg);
				AddArguments(args...);
			}
			
			void AddArguments() {}
		};
		
		std::ostream& operator<<(std::ostream& into, const DisassembledOpcode& opcode);
		
		struct InstructionRange : public InstructionDispatcher<InstructionRange>
		{
			std::string Name;
			bool IsFunction;
			const Common::UInt32* Begin;
			const Common::UInt32* End;
			std::vector<DisassembledOpcode> Opcodes;
			
			InstructionRange(Common::IAllocator* allocator, const Common::UInt32* begin);
			
			void SetEnd(const Common::UInt32* end);
			
		private:
			Common::IAllocator* allocator;
			const PEF::TransitionVector* r12;
			
			template<typename... TParams>
			void Emit(TParams... params)
			{
				Opcodes.emplace_back(params...);
			}
			
		public:
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
			void addcx(Instruction inst);
			void addex(Instruction inst);
			void addi(Instruction inst);
			void addic_rc(Instruction inst);
			void addic(Instruction inst);
			void addis(Instruction inst);
			void addmex(Instruction inst);
			void addx(Instruction inst);
			void addzex(Instruction inst);
			void andcx(Instruction inst);
			void andi_rc(Instruction inst);
			void andis_rc(Instruction inst);
			void andx(Instruction inst);
			void cmp(Instruction inst);
			void cmpi(Instruction inst);
			void cmpl(Instruction inst);
			void cmpli(Instruction inst);
			void cntlzwx(Instruction inst);
			void divwux(Instruction inst);
			void divwx(Instruction inst);
			void eqvx(Instruction inst);
			void extsbx(Instruction inst);
			void extshx(Instruction inst);
			void mulhwux(Instruction inst);
			void mulhwx(Instruction inst);
			void mulli(Instruction inst);
			void mullwx(Instruction inst);
			void nandx(Instruction inst);
			void negx(Instruction inst);
			void norx(Instruction inst);
			void orcx(Instruction inst);
			void ori(Instruction inst);
			void oris(Instruction inst);
			void orx(Instruction inst);
			void rlwimix(Instruction inst);
			void rlwinmx(Instruction inst);
			void rlwnmx(Instruction inst);
			void slwx(Instruction inst);
			void srawix(Instruction inst);
			void srawx(Instruction inst);
			void srwx(Instruction inst);
			void subfcx(Instruction inst);
			void subfex(Instruction inst);
			void subfic(Instruction inst);
			void subfmex(Instruction inst);
			void subfx(Instruction inst);
			void subfzex(Instruction inst);
			void tw(Instruction inst);
			void twi(Instruction inst);
			void xori(Instruction inst);
			void xoris(Instruction inst);
			void xorx(Instruction inst);
			
			// Load/Store Instructions
			void eieio(Instruction inst);
			void lbz(Instruction inst);
			void lbzu(Instruction inst);
			void lbzux(Instruction inst);
			void lbzx(Instruction inst);
			void lfd(Instruction inst);
			void lfdu(Instruction inst);
			void lfdux(Instruction inst);
			void lfdx(Instruction inst);
			void lfs(Instruction inst);
			void lfsu(Instruction inst);
			void lfsux(Instruction inst);
			void lfsx(Instruction inst);
			void lha(Instruction inst);
			void lhau(Instruction inst);
			void lhaux(Instruction inst);
			void lhax(Instruction inst);
			void lhbrx(Instruction inst);
			void lhz(Instruction inst);
			void lhzu(Instruction inst);
			void lhzux(Instruction inst);
			void lhzx(Instruction inst);
			void lmw(Instruction inst);
			void lswi(Instruction inst);
			void lswx(Instruction inst);
			void lwarx(Instruction inst);
			void lwbrx(Instruction inst);
			void lwz(Instruction inst);
			void lwzu(Instruction inst);
			void lwzux(Instruction inst);
			void lwzx(Instruction inst);
			void stb(Instruction inst);
			void stbu(Instruction inst);
			void stbux(Instruction inst);
			void stbx(Instruction inst);
			void stfd(Instruction inst);
			void stfdu(Instruction inst);
			void stfdux(Instruction inst);
			void stfdx(Instruction inst);
			void stfiwx(Instruction inst);
			void stfs(Instruction inst);
			void stfsu(Instruction inst);
			void stfsux(Instruction inst);
			void stfsx(Instruction inst);
			void sth(Instruction inst);
			void sthbrx(Instruction inst);
			void sthu(Instruction inst);
			void sthux(Instruction inst);
			void sthx(Instruction inst);
			void stmw(Instruction inst);
			void stswi(Instruction inst);
			void stswx(Instruction inst);
			void stw(Instruction inst);
			void stwbrx(Instruction inst);
			void stwcxd(Instruction inst);
			void stwu(Instruction inst);
			void stwux(Instruction inst);
			void stwx(Instruction inst);
			
			// System Registers Instructions
			void crand(Instruction inst);
			void crandc(Instruction inst);
			void creqv(Instruction inst);
			void crnand(Instruction inst);
			void crnor(Instruction inst);
			void cror(Instruction inst);
			void crorc(Instruction inst);
			void crxor(Instruction inst);
			void isync(Instruction inst);
			void mcrf(Instruction inst);
			void mcrfs(Instruction inst);
			void mcrxr(Instruction inst);
			void mfcr(Instruction inst);
			void mffsx(Instruction inst);
			void mfspr(Instruction inst);
			void mftb(Instruction inst);
			void mtcrf(Instruction inst);
			void mtfsb0x(Instruction inst);
			void mtfsb1x(Instruction inst);
			void mtfsfix(Instruction inst);
			void mtfsfx(Instruction inst);
			void mtspr(Instruction inst);
			void rfid(Instruction inst);
			void sync(Instruction inst);
			
			// branching
			void bcctrx(Instruction inst);
			void bclrx(Instruction inst);
			void bcx(Instruction inst);
			void bx(Instruction inst);
			void sc(Instruction inst);

			// supervisor mode (not implemented)
			void dcba(Instruction inst);
			void dcbf(Instruction inst);
			void dcbst(Instruction inst);
			void dcbt(Instruction inst);
			void dcbtst(Instruction inst);
			void dcbz(Instruction inst);
			void dcbi(Instruction inst);
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
	}
}

#endif /* defined(__Classix__InstructionRange__) */
