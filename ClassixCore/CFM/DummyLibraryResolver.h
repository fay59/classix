//
// DummyLibraryResolver.h
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

#ifndef __Classix__DummyLibraryResolver__
#define __Classix__DummyLibraryResolver__

#include <map>
#include "LibraryResolver.h"
#include "Allocator.h"

namespace CFM
{
	class DummySymbolResolver;
	
	class DummyLibraryResolver : public LibraryResolver
	{
		std::map<uint32_t, DummySymbolResolver> resolvers;
		Common::Allocator& allocator;
		
	public:
		DummyLibraryResolver(Common::Allocator& allocator);
		
		const DummySymbolResolver* ResolverForAddress(uint32_t address) const;
		
		virtual SymbolResolver* ResolveLibrary(const std::string& name) override;
		virtual ~DummyLibraryResolver() override;
	};
}

#endif /* defined(__Classix__DummyLibraryResolver__) */
