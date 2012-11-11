//
//  LibraryResolver.h
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__LibraryResolver__
#define __pefdump__LibraryResolver__

#import "SymbolResolver.h"

namespace CFM
{
	class LibraryResolver
	{
	public:
		virtual SymbolResolver* ResolveLibrary(const std::string& name) = 0;
		virtual ~LibraryResolver() = 0;
	};
}

#endif /* defined(__pefdump__LibraryResolver__) */
