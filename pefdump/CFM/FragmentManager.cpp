//
//  FragmentManager.cpp
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "FragmentManager.h"
#include "PEFSymbolResolver.h"
#include "LibraryResolutionException.h"
#include "SymbolResolutionException.h"

namespace CFM
{
	FragmentManager::FragmentManager()
	{
		main = nullptr;
	}
	
	bool FragmentManager::LoadContainer(const std::string &name)
	{
		auto findResult = resolvers.find(name);
		if (findResult != resolvers.end())
			return true;
		
		for (LibraryResolver* libraryResolver : Resolvers)
		{
			SymbolResolver* symbolResolver = libraryResolver->ResolveLibrary(name);
			if (symbolResolver != nullptr)
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
		
		ResolvedSymbol symbol = resolvers[name]->ResolveSymbol(name);
		if (symbol.Universe == CFM::SymbolUniverse::LostInTimeAndSpace)
			throw CFM::SymbolResolutionException(container, name);
		
		return symbol;
	}
}
