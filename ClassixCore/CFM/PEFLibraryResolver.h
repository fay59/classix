//
// PEFLibraryResolver.h
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

#ifndef __pefdump__PEFLibraryResolver__
#define __pefdump__PEFLibraryResolver__

#include <deque>
#include "LibraryResolver.h"
#include "FragmentManager.h"
#include "IAllocator.h"
#include "PEFSymbolResolver.h"

namespace CFM
{
	class PEFLibraryResolver : public virtual LibraryResolver
	{
		FragmentManager& cfm;
		Common::IAllocator* allocator;
		std::deque<PEFSymbolResolver> resolvers;
		
	public:
		PEFLibraryResolver(Common::IAllocator* allocator, FragmentManager& manager);
						   
		virtual SymbolResolver* ResolveLibrary(const std::string& name);
		virtual ~PEFLibraryResolver();
	};
}

#endif /* defined(__pefdump__PEFLibraryResolver__) */
