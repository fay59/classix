//
//  Relocation.cpp
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "Relocation.h"

namespace PEF
{
	Relocation::Relocation(const RelocationHeader* header, const uint8_t* relocationBase)
	{
		this->header = header;
		this->relocationBase = reinterpret_cast<const UInt16*>(relocationBase + header->FirstRelocationOffset);
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
