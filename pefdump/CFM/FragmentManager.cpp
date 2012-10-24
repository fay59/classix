//
//  FragmentManager.cpp
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "FragmentManager.h"
#include "PEFSymbolResolver.h"

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
		
		for (auto libraryResolver : Resolvers)
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
	
	intptr_t FragmentManager::ResolveSymbol(const std::string &container, const std::string &name)
	{
		if (!LoadContainer(container))
			throw std::logic_error("Cannot resolve symbol because container cannot be openend");
		
		return resolvers[name]->ResolveSymbol(name);
	}
}
