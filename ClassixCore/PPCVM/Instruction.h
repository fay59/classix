#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>

namespace PPCVM
{
	union Instruction
	{
		uint32_t hex;

		Instruction(uint32_t hex) : hex(hex) {}
		Instruction() : hex(0) {}

		struct
		{
			uint32_t Rc			:	1;
			uint32_t SUBOP10	:	10;
			uint32_t RB			:	5;
			uint32_t RA			:	5;
			uint32_t RD			:	5;
			uint32_t OPCD		:	6;
		};
		struct
		{
			signed SIMM_16		:	16;
			uint32_t 			:	5;
			uint32_t TO			:	5;
			uint32_t OPCD_2		:	6;
		};
		struct
		{
			uint32_t Rc_2		:	1;
			uint32_t			:	10;
			uint32_t 			:	5;
			uint32_t 			:	5;
			uint32_t RS			:	5;
			uint32_t OPCD_3		:	6;
		};
		struct
		{
			uint32_t UIMM		:	16;
			uint32_t 			:	5;
			uint32_t 			:	5;
			uint32_t OPCD_4		:	6;
		};
		struct
		{
			uint32_t LK			:	1;
			uint32_t AA			:	1;
			uint32_t LI			:	24;
			uint32_t OPCD_5		:	6;
		};
		struct
		{     
			uint32_t LK_2		:	1;
			uint32_t AA_2		:	1;
			uint32_t BD			:	14;
			uint32_t BI			:	5;
			uint32_t BO			:	5;
			uint32_t OPCD_6		:	6;
		};
		struct
		{
			uint32_t LK_3		:	1;
			uint32_t			:	10;
			uint32_t			:	5;
			uint32_t BI_2		:	5;
			uint32_t BO_2		:	5;
			uint32_t OPCD_7		:	6;
		};
		struct
		{
			uint32_t			:	11;
			uint32_t RB_2		:	5;
			uint32_t RA_2		:	5;
			uint32_t L			:	1;
			uint32_t 			:	1;
			uint32_t CRFD		:	3;
			uint32_t OPCD_8		:	6;
		};
		struct
		{
			signed  SIMM_16_2	:	16;
			uint32_t RA_3		:	5;
			uint32_t L_2		:	1;
			uint32_t			:	1;
			uint32_t CRFD_2		:	3;
			uint32_t OPCD_9		:	6;
		};
		struct
		{     
			uint32_t UIMM_2		:	16;
			uint32_t RA_4		:	5;
			uint32_t L_3		:	1;
			uint32_t dummy2		:	1;
			uint32_t CRFD_3		:	3;
			uint32_t OPCD_A		:	6;
		};
		struct
		{
			uint32_t			:	1;
			uint32_t SUBOP10_2	:	10;
			uint32_t RB_5		:	5;
			uint32_t RA_5		:	5;
			uint32_t L_4		:	1;
			uint32_t dummy3		:	1;
			uint32_t CRFD_4		:	3;
			uint32_t OPCD_B		:	6;
		};
		struct
		{
			uint32_t			:	16;
			uint32_t SR			:	4;
			uint32_t 			:	1;
			uint32_t RS_2		:	5;
			uint32_t OPCD_C		:	6;
		};

		// Table 59
		struct
		{
			uint32_t Rc_4		:	1;
			uint32_t SUBOP5		:	5;
			uint32_t RC			:	5;
			uint32_t			:	5;
			uint32_t RA_6		:	5;
			uint32_t RD_2		:	5;
			uint32_t OPCD_D		:	6;
		};

		struct
		{	uint32_t			:	10;
			uint32_t OE			:	1;
			uint32_t SPR		:	10;
			uint32_t			:	11;
		};
		struct
		{
			uint32_t			:	10;
			uint32_t OE_3		:	1;
			uint32_t SPRU		:	5;
			uint32_t SPRL		:	5;
			uint32_t			:	11;
		};

		// rlwinmx
		struct
		{
			uint32_t Rc_3		:	1;
			uint32_t ME			:	5;
			uint32_t MB			:	5;
			uint32_t SH			:	5;
			uint32_t			:	16;
		};

		// crxor
		struct 
		{
			uint32_t			:	11;
			uint32_t CRBB		:	5;
			uint32_t CRBA		:	5;
			uint32_t CRBD		:	5;
			uint32_t			:	6;
		};

		// mftb
		struct 
		{
			uint32_t			:	11;
			uint32_t TBR		:	10;
			uint32_t			:	11;
		};

		struct 
		{
			uint32_t			:	11;
			uint32_t TBRU		:	5;
			uint32_t TBRL		:	5;
			uint32_t			:	11;
		};

		struct 
		{
			uint32_t			:	18;
			uint32_t CRFS		:	3;
			uint32_t 			:	2;
			uint32_t CRFD_5		:	3;
			uint32_t			:	6;
		};

		// float
		struct 
		{
			uint32_t			:	12;
			uint32_t CRM		:	8;
			uint32_t			:	1;
			uint32_t FD			:	5;
			uint32_t			:	6;
		};
		struct 
		{
			uint32_t			:	6;
			uint32_t FC			:	5;
			uint32_t FB			:	5;
			uint32_t FA			:	5;
			uint32_t FS			:	5;
			uint32_t			:	6;
		};
		struct 
		{
			uint32_t OFS		:	16;
			uint32_t			:	16;
		};
		struct
		{
			uint32_t			:	17;
			uint32_t FM			:	8;
			uint32_t			:	7;
		};

		// paired
		struct 
		{
			uint32_t			:	7;
			uint32_t Ix			:	3;
			uint32_t Wx			:	1;
			uint32_t			:	1;
			uint32_t I			:	3;
			uint32_t W			:	1;
			uint32_t			:	16;
		};

		struct 
		{
			signed	SIMM_12		:	12;
			uint32_t			:	20;
		};

		struct 
		{
			uint32_t dummyX		:   11;
			uint32_t NB			: 5;
		};
	};
}

#endif