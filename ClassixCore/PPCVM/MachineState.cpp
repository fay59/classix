//
// MachineState.cpp
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
