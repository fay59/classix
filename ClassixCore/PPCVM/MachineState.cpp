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
#include <mach/mach_time.h>

namespace
{
	inline mach_timebase_info_data_t GetTimebaseInfo()
	{
		mach_timebase_info_data_t data;
		mach_timebase_info(&data);
		return data;
	}
	
	mach_timebase_info_data_t timebaseInfo = GetTimebaseInfo();
	
	uint64_t GetElapsedNanos()
	{
		uint64_t now = mach_absolute_time();
		// hopefully this won't overflow
		return now * timebaseInfo.numer / timebaseInfo.denom;
	}
	
	const int kNanosInSeconds = 1000000000;
}

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
	
	uint32_t MachineState::GetRTCU() const
	{
		return static_cast<uint32_t>(GetElapsedNanos() / kNanosInSeconds);
	}
	
	uint32_t MachineState::GetRTCL() const
	{
		return GetElapsedNanos() % kNanosInSeconds;
	}
}
