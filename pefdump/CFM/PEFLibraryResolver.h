//
//  PEFLibraryResolver.h
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__PEFLibraryResolver__
#define __pefdump__PEFLibraryResolver__

#include "LibraryResolver.h"
#include "FragmentManager.h"
#include "IAllocator.h"

namespace CFM
{
	class PEFLibraryResolver : public LibraryResolver
	{
		FragmentManager& cfm;
		Common::IAllocator* allocator;
		
	public:
		PEFLibraryResolver(Common::IAllocator* allocator, FragmentManager& manager);
						   
		virtual SymbolResolver* ResolveLibrary(const std::string& name);
		virtual ~PEFLibraryResolver();
	};
}

#endif /* defined(__pefdump__PEFLibraryResolver__) */
