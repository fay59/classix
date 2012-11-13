//
//  MachineState.cpp
//  pefdump
//
//  Created by Félix on 2012-10-26.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "MachineState.h"
#include <cstring>

namespace PPCVM
{
	MachineState::MachineState()
	{
		memset(this, 0, sizeof *this);
	}

	void MachineState::SetCR(uint32_t value)
	{
		for (int i = 0; i < 8; i++)
			cr[i] = (value >> (28 - i * 4)) & 0xf;
	}

	uint32_t MachineState::GetCR() const
	{
		uint32_t crValue = cr[0] << 28;
		for (int i = 0; i < 8; i++)
			crValue |= cr[i] << (28 - i * 4);
		
		return crValue;
	}
}
