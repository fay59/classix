//
// FragmentManager.cpp
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

#include "FragmentManager.h"
#include "PEFSymbolResolver.h"
#include "LibraryResolutionException.h"
#include "SymbolResolutionException.h"

namespace CFM
{
	FragmentManager::FragmentManager()
	{ }
	
	bool FragmentManager::LoadContainer(const std::string &name)
	{
		auto findResult = resolvers.find(name);
		if (findResult != resolvers.end())
			return true;
		
		for (LibraryResolver* libraryResolver : LibraryResolvers)
		{
			if (SymbolResolver* symbolResolver = libraryResolver->ResolveLibrary(name))
			{
				resolvers[name] = symbolResolver;
				return true;
			}
		}
		
		return false;
	}
	
	ResolvedSymbol FragmentManager::ResolveSymbol(const std::string &container, const std::string &name)
	{
		if (!LoadContainer(container))
			throw CFM::LibraryResolutionException(container);
		
		ResolvedSymbol symbol = resolvers[container]->ResolveSymbol(name);
		if (symbol.Universe == CFM::SymbolUniverse::LostInTimeAndSpace)
			throw CFM::SymbolResolutionException(container, name);
		
		return symbol;
	}
	
	FragmentManager::iterator FragmentManager::begin()
	{
		return resolvers.begin();
	}
	
	FragmentManager::iterator FragmentManager::end()
	{
		return resolvers.end();
	}
	
	FragmentManager::const_iterator FragmentManager::begin() const
	{
		return resolvers.begin();
	}
	
	FragmentManager::const_iterator FragmentManager::end() const
	{
		return resolvers.end();
	}
	
	SymbolResolver* FragmentManager::GetSymbolResolver(const std::string &resolver)
	{
		auto iter = resolvers.find(resolver);
		if (iter == resolvers.end())
			return nullptr;
		return iter->second;
	}
}
