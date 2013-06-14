//
// Instruction.h
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
			uint32_t			:	6;
		};
		struct
		{
			uint32_t			:	1;
			uint32_t			:	10;
			uint32_t 			:	5;
			uint32_t 			:	5;
			uint32_t RS			:	5;
			uint32_t			:	6;
		};
		struct
		{
			uint32_t UIMM		:	16;
			uint32_t 			:	5;
			uint32_t 			:	5;
			uint32_t			:	6;
		};
		struct
		{
			uint32_t LK			:	1;
			uint32_t AA			:	1;
			int32_t LI			:	24;
			uint32_t			:	6;
		};
		struct
		{     
			uint32_t			:	1;
			uint32_t			:	1;
			int32_t BD			:	14;
			uint32_t BI			:	5;
			uint32_t BO			:	5;
			uint32_t			:	6;
		};
		struct
		{
			uint32_t			:	11;
			uint32_t			:	5;
			uint32_t			:	5;
			uint32_t L			:	1;
			uint32_t 			:	1;
			uint32_t CRFD		:	3;
			uint32_t			:	6;
		};
		struct
		{
			uint32_t			:	16;
			uint32_t SR			:	4;
			uint32_t 			:	1;
			uint32_t			:	5;
			uint32_t			:	6;
		};

		// Table 59
		struct
		{
			uint32_t			:	1;
			uint32_t SUBOP5		:	5;
			uint32_t RC			:	5;
			uint32_t			:	5;
			uint32_t			:	5;
			uint32_t			:	5;
			uint32_t			:	6;
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
			uint32_t			:	1;
			uint32_t SPRU		:	5;
			uint32_t SPRL		:	5;
			uint32_t			:	11;
		};

		// rlwinmx
		struct
		{
			uint32_t			:	1;
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
			uint32_t			:	3;
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
			uint32_t			:   11;
			uint32_t NB			:	5;
		};
	};
}

#endif