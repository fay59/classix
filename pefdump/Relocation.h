//
//  Relocation.h
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__Relocation__
#define __pefdump__Relocation__

#include "Structures.h"

namespace PEF
{
	class Relocation
	{
		const RelocationHeader* header;
		const uint16_t* relocationBase;
		
	public:
		typedef const uint16_t* iterator;
		
		Relocation(const RelocationHeader* header, const uint8_t* relocationBase);
		
		uint16_t GetSectionIndex() const;
		
		iterator begin() const;
		iterator end() const;
	};
}

#endif /* defined(__pefdump__Relocation__) */
