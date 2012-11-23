//
// FragmentManager.h
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
		
	public:
		typedef std::map<std::string, SymbolResolver*>::const_iterator SymbolResolverIterator;
		FragmentManager();
		FragmentManager(const FragmentManager& that) = delete;
		
		std::list<LibraryResolver*> LibraryResolvers;
		
		bool LoadContainer(const std::string& name);
		ResolvedSymbol ResolveSymbol(const std::string& container, const std::string& name);
		
		SymbolResolverIterator Begin() const;
		SymbolResolverIterator End() const;
		SymbolResolver* GetSymbolResolver(const std::string& resolver);
	};
}

#endif /* defined(__pefdump__FragmentManager__) */
