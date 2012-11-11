#ifndef FLOATINGPOINTSTATUS_H
#define FLOATINGPOINTSTATUS_H

#include <cstdint>

enum class FloatingPointStatus : uint32_t
{
	FX = 0x80000000,      /* FPU exception summary */
	FEX = 0x40000000,     /* FPU enabled exception summary */
	VX = 0x20000000,      /* Invalid operation summary */
	OX = 0x10000000,      /* Overflow exception summary */
	UX = 0x08000000,      /* Underflow exception summary */
	ZX = 0x04000000,      /* Zero-divide exception summary */
	XX = 0x02000000,      /* Inexact exception summary */
	VXSNAN = 0x01000000,  /* Invalid op for SNaN */
	VXISI = 0x00800000,   /* Invalid op for Inv - Inv */
	VXIDI = 0x00400000,   /* Invalid op for Inv / Inv */
	VXZDZ = 0x00200000,   /* Invalid op for Zero / Zero */
	VXIMZ = 0x00100000,   /* Invalid op for Inv * Zero */
	VXVC = 0x00080000,    /* Invalid op for Compare */
	FR = 0x00040000,      /* Fraction rounded */
	FI = 0x00020000,      /* Fraction inexact */
	FPRF = 0x0001f000,    /* FPU Result Flags */
	FPCC = 0x0000f000,    /* FPU Condition Codes */
	VXSOFT = 0x00000400,  /* Invalid op for software request */
	VXSQRT = 0x00000200,  /* Invalid op for square root */
	VXCVI = 0x00000100,   /* Invalid op for integer convert */
	VE = 0x00000080,      /* Invalid op exception enable */
	OE = 0x00000040,      /* IEEE overflow exception enable */
	UE = 0x00000020,      /* IEEE underflow exception enable */
	ZE = 0x00000010,      /* IEEE zero divide exception enable */
	XE = 0x00000008,      /* FP inexact exception enable */
	NI = 0x00000004,      /* FPU non IEEE-Mode */
	RN = 0x00000003,      /* FPU rounding control */
};

#endif