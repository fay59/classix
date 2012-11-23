//
// Relocation.cpp
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

#include "Relocation.h"

namespace PEF
{
	Relocation::Relocation(const RelocationHeader* header, const uint8_t* relocationBase)
	{
		this->header = header;
		this->relocationBase = reinterpret_cast<const Common::UInt16*>(relocationBase + header->FirstRelocationOffset);
	}
	
	uint16_t Relocation::GetSectionIndex() const
	{
		return header->SectionIndex;
	}
	
	Relocation::iterator Relocation::begin() const
	{
		return relocationBase;
	}
	
	Relocation::iterator Relocation::end() const
	{
		return relocationBase + header->RelocationCount;
	}
}
