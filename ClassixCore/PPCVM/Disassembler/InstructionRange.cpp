//
//  InstructionRange.cpp
//  Classix
//
//  Created by Félix on 2012-11-12.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "InstructionRange.h"
#include <cassert>
#include <sstream>

namespace
{
	const char* numbers[] = {
		"0", "1", "2", "3", "4", "5", "6", "7", "8",
		"9", "10", "11", "12", "13", "14", "15", "16",
		"17", "18", "19", "20", "21", "22", "23", "24",
		"25", "26", "27", "28", "29", "30", "31"
	};
	
	const char* sprList[1024] = {
		[1] = "xer",
		[8] = "lr",
		[9] = "ctr",
		[18] = "dsisr",
		[19] = "dar",
		[22] = "dec",
		[25] = "sdr1",
		[26] = "srr0",
		[27] = "srr1",
		[272] = "sprg0",
		[273] = "sprg1",
		[274] = "sprg2",
		[275] = "sprg3",
		[282] = "ear",
		[287] = "pvr",
		[528] = "ibat0u",
		[529] = "ibat0l",
		[530] = "ibat1u",
		[531] = "ibat1l",
		[532] = "ibat2u",
		[533] = "ibat2l",
		[534] = "ibat3u",
		[535] = "ibat3l",
		[536] = "dbat0u",
		[537] = "dbat0l",
		[538] = "dbat1u",
		[539] = "dbat1l",
		[540] = "dbat2u",
		[541] = "dbat2l",
		[542] = "dbat3u",
		[543] = "dbat3l",
		[1013] = "dabr",
	};
	
	std::string spr(uint32_t reg)
	{
		if (const char* sprName = sprList[reg])
			return sprName;
		
		return std::string("spr") + numbers[reg];
	}
	
	std::string g(uint32_t reg)
	{
		return std::string("r") + numbers[reg];
	}
	
	std::string f(uint32_t reg)
	{
		return std::string("fr") + numbers[reg];
	}
	
	std::string cr(uint32_t cr)
	{
		return std::string("crf") + static_cast<char>('0' + cr);
	}
	
	std::string hex(uint32_t h)
	{
		std::stringstream ss;
		ss << "0x" << std::hex << h;
		return ss.str();
	}
	
	std::string hex(int32_t h)
	{
		if (h < 0)
			return '-' + hex(static_cast<uint32_t>(abs(h)));
		return hex(static_cast<uint32_t>(h));
	}
	
	std::string opX(const std::string& x, bool setsFlags)
	{
		if (!setsFlags)
			return x;
		return x + '.';
	}
	
	std::string opXO(std::string x, bool setsFlags, bool overflows)
	{
		if (overflows)
			x += 'o';
		if (setsFlags)
			x += '.';
		return x;
	}
	
	std::string opLK(std::string x, bool lk)
	{
		if (lk)
			return x + 'l';
		return x;
	}
	
	std::string offset(uint32_t r, int32_t offset)
	{
		std::stringstream ss;
		ss << hex(offset) << '(' << g(r) << ')';
		return ss.str();
	}
}

namespace PPCVM
{
	namespace Disassembly
	{
		std::ostream& operator<<(std::ostream& into, const DisassembledOpcode& opcode)
		{
			into << opcode.Opcode << '\t';
			
			auto iter = opcode.Arguments.cbegin();
			if (iter != opcode.Arguments.cend())
				into << *iter;
			
			for (iter++; iter != opcode.Arguments.cend(); iter++)
				into << ", " << *iter;
			
			if (opcode.Complement.length() > 0)
				into << " <" << opcode.Complement << '>';
			return into;
		}
		
		InstructionRange::InstructionRange(Common::IAllocator* allocator, const Common::UInt32* begin)
		: Begin(begin), End(nullptr), allocator(allocator), r12(nullptr)
		{ }
		
		void InstructionRange::SetEnd(const Common::UInt32 *end)
		{
			assert(end >= Begin && "End before Begin");
			End = end;
			
			Opcodes.reserve(End - Begin);
			for (auto iter = Begin; iter != End; iter++)
				Dispatch(PPCVM::Instruction(*iter));
		}
		
#pragma mark -
#define IMPL(x)	void InstructionRange::x(PPCVM::Instruction i)
#define BODY(x, c)	IMPL(x) { c; }
#define OP(name)	BODY(name, Emit(i, #name))
		void InstructionRange::unknown(PPCVM::Instruction i)
		{
			Emit(i, ".word", hex(i.hex));
		}
		
#pragma mark -
#pragma mark Floating Point Instructions
#define FX2(name)	IMPL(name##x) { Emit(i, opX(#name, i.RC), f(i.RD), f(i.RB)); }
#define FX3(name)	IMPL(name##x) { Emit(i, opX(#name, i.RC), f(i.RD), f(i.RA), f(i.RB)); }
#define FX4(name)	IMPL(name##x) { Emit(i, opX(#name, i.RC), f(i.RD), f(i.RA), f(i.RC), f(i.RB)); }
#define FX2S(name)	FX2(name) FX2(name##s)
#define FX3S(name)	FX3(name) FX3(name##s)
#define FX2Z(name)	FX2(name) FX2(name##z)
#define FX4S(name)	FX4(name) FX4(name##s)
		
		FX2(fabs);
		FX3S(fadd);
		
		IMPL(fcmpo)
		{
			Emit(i, "fcmpo", cr(i.CRFD), f(i.RA), f(i.RB));
		}
		
		IMPL(fcmpu)
		{
			Emit(i, "fcmpu", cr(i.CRFD), f(i.RA), f(i.RB));
		}
		
		FX2Z(fctiw);
		FX3S(fdiv);
		FX4S(fmadd);
		FX2(fmr);
		FX4S(fmsub);
		FX3S(fmul);
		FX2(fnabs);
		FX2(fneg);
		FX4S(fnmadd);
		FX4S(fnmsub);
		FX2(fres);
		FX2(frsp);
		FX2(frsqrte);
		FX4(fsel);
		FX2S(fsqrt);
		FX2S(fsub);
		
#pragma mark -
#pragma mark Integer Instructions
#define IADA(name)	BODY(name##x, Emit(i, opXO(#name, i.RC, i.OE), g(i.RD), g(i.RA)))
#define ILRR(name, a, b) BODY(name##x, Emit(i, opX(#name, i.RC), g(i.R##a), g(i.R##b)))
#define ILRRR(name, a, b, c) BODY(name##x, Emit(i, opX(#name, i.RC), g(i.R##a), g(i.R##b), g(i.R##c)))
#define ILDAB(name) ILRRR(name, D, A, B)
#define IADAB(name)	IMPL(name##x) { Emit(i, opXO(#name, i.RC, i.OE), g(i.RD), g(i.RA), g(i.RB)); }
#define ILASB(name)	BODY(name##x, Emit(i, opX(#name, i.RC), g(i.RA), g(i.RS), g(i.RB)))
#define IADAI(name, dname)	BODY(name, Emit(i, #dname, g(i.RD), g(i.RA), hex(i.SIMM_16)))
#define ICMP(name, to)	IMPL(name) { if (i.L) unknown(i); else if (i.CRFD == 0) Emit(i, #name "d", g(i.RA), to); else if (i.CRFD == 3) Emit(i, #name "w", g(i.RA), to); else Emit(i, #name, cr(i.CRFD), g(i.RA), to); }
#define IROT(name)	BODY(name##x, Emit(i, opX(#name, i.RC), g(i.RA), g(i.RS), hex(i.SH), hex(i.MB), hex(i.ME)))
		
		IADAB(addc);
		IADAB(adde);
		IADAI(addi, addi);
		IADAI(addic, addic);
		IADAI(addic_rc, addic.);
		IADAI(addis, addis);
		IADA(addme);
		IADA(addze);
		IADAB(add);
		ILASB(and);
		ILASB(andc);
		BODY(andi_rc, Emit(i, "andi.", g(i.RA), g(i.RS), hex(i.UIMM)));
		BODY(andis_rc, Emit(i, "andis.", g(i.RA), g(i.RS), hex(i.UIMM)));
		
		ICMP(cmp, g(i.RB));
		ICMP(cmpi, hex(i.SIMM_16));
		ICMP(cmpl, g(i.RB));
		ICMP(cmpli, hex(i.UIMM));
		
		ILRR(cntlzw, A, S);
		IADAB(divwu);
		IADAB(divw);
		
		ILASB(eqv);
		ILRR(extsb, A, S);
		ILRR(extsh, A, S);
		
		ILDAB(mulhw);
		ILDAB(mulhwu);
		
		IADAI(mulli, mulli);
		IADAB(mullw);
		ILASB(nand);
		IADA(neg);
		ILASB(nor);
		ILASB(or);
		ILASB(orc);
		BODY(ori, Emit(i, "ori", g(i.RA), g(i.RS), g(i.UIMM)));
		BODY(oris, Emit(i, "oris", g(i.RA), g(i.RS), g(i.UIMM)));
		
		IROT(rlwimi);
		IROT(rlwinm); // TODO simplified mnemonics (8-164)
		IROT(rlwnm);
		
		ILASB(slw);
		ILASB(sraw);
		BODY(srawix, Emit(i, opX("srawi", i.RC), g(i.RA), g(i.RS), hex(i.SH)));
		ILASB(srw);
		
		IADAB(subf);
		IADAB(subfc);
		IADAB(subfe);
		BODY(subfic, Emit(i, "subfic", g(i.RD), g(i.RA), hex(i.SIMM_16)));
		IADA(subfme);
		IADA(subfze);
		
		IMPL(tw)
		{
			if (i.TO == 4)
				Emit(i, "tweq", g(i.RA), g(i.RB));
			else if (i.TO == 5)
				Emit(i, "twge", g(i.RA), g(i.RB));
			else if (i.TO == 31 && i.RA == 0 && i.RB == 0)
				Emit(i, "trap");
			else
				Emit(i, "tw", hex(i.TO), g(i.RA), g(i.RB));
		}
		
		IMPL(twi)
		{
			if (i.TO == 8)
				Emit(i, "twgti", g(i.RA), hex(i.SIMM_16));
			else if (i.TO == 6)
				Emit(i, "twllei", g(i.RA), hex(i.SIMM_16));
			else
				Emit(i, "twi", hex(i.TO), g(i.RA), hex(i.SIMM_16));
		}
		
		ILASB(xor);
		BODY(xori, Emit(i, "xori", g(i.RA), g(i.RS), hex(i.UIMM)));
		BODY(xoris, Emit(i, "xori", g(i.RA), g(i.RS), hex(i.UIMM)));
		
#pragma mark -
#pragma mark Load/Store
#define ADDR			i.RA == 0 ? hex(i.SIMM_16) : offset(i.RA, i.SIMM_16)
#define ADDRU			offset(i.RA, i.SIMM_16)
#define ADDRX			i.RA == 0 ? g(i.RB) : (g(i.RA)+'+'+g(i.RB))
#define ADDRUX			g(i.RA)+'+'+g(i.RB)
#define LOADI_(name, addr)			BODY(name, Emit(i, #name, g(i.RD), addr))
#define LOADI(name)					BODY(name, Emit(i, #name, g(i.RD), ADDR)) LOADI_(name##x, ADDRX) LOADI_(name##u, ADDRU) LOADI_(name##ux, ADDRUX)
#define LOADF_(name, addr)			BODY(name, Emit(i, #name, f(i.RD), addr))
#define LOADF(name)					BODY(name, Emit(i, #name, f(i.RD), ADDR)) LOADF_(name##x, ADDRX) LOADF_(name##u, ADDRU) LOADF_(name##ux, ADDRUX)
		
		OP(eieio);
		LOADI(lbz);
		LOADF(lfd);
		LOADF(lfs);
		LOADI(lha);
		LOADI_(lhbrx, ADDRX);
		LOADI(lhz);
		LOADI_(lmw, ADDR);
		BODY(lswi, Emit(i, "lswi", g(i.RD), g(i.RA), hex(i.NB)));
		BODY(lswx, Emit(i, "lswx", g(i.RD), g(i.RA), g(i.RB)));
		LOADI_(lwarx, ADDRX);
		LOADI_(lwbrx, ADDRX);
		LOADI(lwz);
		LOADI(stb);
		LOADI(stfd);
		BODY(stfiwx, Emit(i, "stfiwx", f(i.RD), g(i.RA), g(i.RB)));
		LOADI(stfs);
		LOADI(sth);
		LOADI_(sthbrx, ADDRX);
		LOADI_(stmw, ADDR);
		BODY(stswi, Emit(i, "stswi", g(i.RD), g(i.RA), hex(i.NB)));
		BODY(stswx, Emit(i, "stswx", g(i.RD), g(i.RA), g(i.RB)));
		LOADI(stw);
		LOADI_(stwbrx, ADDRX);
		BODY(stwcxd, Emit(i, "stwcx.", g(i.RD), ADDRX));
		
#pragma mark -
#pragma mark System Registers
#define CROP(name)	BODY(name, Emit(i, #name, cr(i.CRBD), cr(i.CRBA), cr(i.CRBB)))
		
		CROP(crand);
		CROP(crandc);
		CROP(creqv);
		CROP(crnand);
		CROP(crnor);
		CROP(cror);
		CROP(crorc);
		CROP(crxor);
		OP(isync);
		
		BODY(mcrf, Emit(i, "mcrfs", cr(i.CRFD), cr(i.CRFD_5)));
		BODY(mcrfs, Emit(i, "mcrfs", cr(i.CRFD), cr(i.CRFD_5)));
		BODY(mcrxr, Emit(i, "mcrxr", cr(i.CRFD)));
		BODY(mfcr, Emit(i, "mfcr", g(i.RD)));
		BODY(mffsx, Emit(i, opX("mffs", i.RC), f(i.RD)));
		
		IMPL(mfspr)
		{
			std::string d = g(i.RD);
			uint8_t spr = (i.RB << 5) | i.RA;
			switch (spr)
			{
				case 1: Emit(i, "mfxer", d); return;
				case 8: Emit(i, "mflr", d); return;
				case 9: Emit(i, "mfctr", d); return;
			}
			Emit(i, "mfspr", d, ::spr(spr));
		}
		
		IMPL(mftb)
		{
			std::string d = g(i.RD);
			uint8_t tbl = (i.RB << 5) | i.RA;
			if (tbl == 268)
				Emit(i, "mftb", d);
			else if (tbl == 269)
				Emit(i, "mftbu", d);
			else
				unknown(i);
		}
		
		BODY(mtcrf, Emit(i, "mtcrf", cr(i.CRM), g(i.RS)));
		BODY(mtfsb0x, Emit(i, opX("mtfsb0", i.RC), cr(i.CRBD)));
		BODY(mtfsb1x, Emit(i, opX("mtfsb0", i.RC), cr(i.CRBD)));
		BODY(mtfsfix, Emit(i, opX("mtfsfi", i.RC), cr(i.CRFD), hex(i.SR)));
		BODY(mtfsfx, Emit(i, opX("mtfsf", i.RC), hex(i.FM), f(i.RB)));
		
		IMPL(mtspr)
		{
			std::string d = g(i.RD);
			uint8_t spr = (i.RB << 5) | i.RA;
			switch (spr)
			{
				case 1: Emit(i, "mtxer", d); return;
				case 8: Emit(i, "mtlr", d); return;
				case 9: Emit(i, "mtctr", d); return;
			}
			Emit(i, "mtspr", d, ::spr(spr));
		}
		
		OP(rfi);
		OP(rfid);
		OP(sync);
		
#pragma mark -
#pragma mark Branching
		IMPL(bcctrx)
		{
			if ((i.BO & 0b10100) == 0b10100)
				Emit(i, opLK("bctr", i.LK));
			else if (i.BO == 12 && i.BI == 0)
				Emit(i, opLK("bltctr", i.LK));
			else if (i.BO == 4 && i.BI == 10)
				Emit(i, opLK("bnectr", i.LK), "cr2");
			else
				Emit(i, opLK("bcctr", i.LK), hex(i.BO), hex(i.BI));
		}
		
		IMPL(bclrx)
		{
			if ((i.BO & 0b10100) == 0b10100)
				Emit(i, opLK("blr", i.LK));
			else if (i.BO == 12 && i.BI == 0)
				Emit(i, opLK("bltlr", i.LK));
			else if (i.BO == 4 && i.BI == 10)
				Emit(i, opLK("bnelr", i.LK), "cr2");
			else if (i.BO == 16 && i.BI == 0)
				Emit(i, opLK("bdnzlr", i.LK));
			else
				Emit(i, opLK("bclr", i.LK), hex(i.BO), hex(i.BI));
		}
		
		IMPL(bcx)
		{
			std::string suffix;
			if (i.LK) suffix += 'l';
			if (i.AA) suffix += 'a';
			
			std::string target = hex(i.BD << 2);
			if ((i.BO & 0b10100) == 0b10100)
				Emit(i, "blt" + suffix, target);
			else if (i.BO == 12 && i.BI == 0)
				Emit(i, "blt" + suffix, target);
			else if (i.BO == 4 && i.BI == 10)
				Emit(i, "bne" + suffix, "cr2", target);
			else if (i.BO == 16 && i.BI == 0)
				Emit(i, "bdnz" + suffix, target);
			else
				Emit(i, "bc" + suffix, hex(i.BO), hex(i.BI), target);
		}
		
		IMPL(bx)
		{
			std::string suffix;
			if (i.LK) suffix += 'l';
			if (i.AA) suffix += 'a';
			
			Emit(i, "b" + suffix, hex(i.LI << 2));
		}
		
		OP(sc);
		
#pragma mark -
#pragma mark Supervisor Mode
#define DCB(name)	BODY(name, Emit(i, #name, g(i.RA), g(i.RB)))
#define A(name) BODY(name, Emit(i, #name, g(i.RA)))
		DCB(dcba);
		DCB(dcbf);
		DCB(dcbt);
		DCB(dcbst);
		DCB(dcbtst);
		DCB(dcbz);
		DCB(dcbi);
		
		BODY(eciwx, Emit(i, "eciwx", g(i.RD), g(i.RA), g(i.RB)));
		BODY(ecowx, Emit(i, "ecowx", g(i.RS), g(i.RA), g(i.RB)));
		DCB(icbi);
		A(mfmsr);
		BODY(mfsr, Emit(i, "mfsr", g(i.RD), spr(i.SR)));
		BODY(mfsrin, Emit(i, "mfsrin", g(i.RD), g(i.RB)));
		BODY(mtmsr, Emit(i, "mtmsr", g(i.RS)));
		BODY(mtsr, Emit(i, "mtsr", spr(i.SR), g(i.RD)));
		BODY(mtsrin, Emit(i, "mfsrin", g(i.RS), g(i.RB)));
		OP(tlbia);
		OP(tlbie);
		OP(tlbsync);
	}
}
