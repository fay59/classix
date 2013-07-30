//
// DummyLibraryResolver.cpp
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

#include "DummyLibraryResolver.h"
#include "DummySymbolResolver.h"

#include <iostream>

namespace CFM
{
	DummyLibraryResolver::DummyLibraryResolver(Common::Allocator& allocator)
	: allocator(allocator)
	{ }
	
	const DummySymbolResolver* DummyLibraryResolver::ResolverForAddress(uint32_t address) const
	{
		auto iter = resolvers.find(address);
		if (iter != resolvers.end())
		{
			return &iter->second;
		}
		return nullptr;
	}

	SymbolResolver* DummyLibraryResolver::ResolveLibrary(const std::string &name)
	{
		std::cerr << "*** resolving " << name << " as a dummy library\n";
		uint32_t address = allocator.CreateInvalidAddress("Fake " + name + " Resolved Address");
		resolvers.emplace(std::make_pair(address, DummySymbolResolver(address, name)));
		return &resolvers.find(address)->second;
	}
	
	DummyLibraryResolver::~DummyLibraryResolver()
	{ }
}