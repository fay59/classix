//
//  FragmentManager.h
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__FragmentManager__
#define __pefdump__FragmentManager__

#include <vector>
#include <map>
#include <string>
#include <list>

#include "Container.h"
#include "SymbolResolver.h"
#include "LibraryResolver.h"

namespace CFM
{
	class FragmentManager
	{
		std::map<std::string, SymbolResolver*> resolvers;
		PEF::Container* main;
		
	public:
		FragmentManager();
		FragmentManager(const FragmentManager& that) = delete;
		
		std::list<LibraryResolver*> Resolvers;
		
		bool LoadContainer(const std::string& name);
		intptr_t ResolveSymbol(const std::string& container, const std::string& name);
	};
}

#endif /* defined(__pefdump__FragmentManager__) */
