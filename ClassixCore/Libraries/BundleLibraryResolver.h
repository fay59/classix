//
// BundleLibraryResolver.h
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

#ifndef __Classix__BundleLibraryResolver__
#define __Classix__BundleLibraryResolver__

// The contents of this file is specific to Mac OS X. Don't even try to port it.

#include <deque>
#include <unordered_set>
#include <unordered_map>

#include "LibraryResolver.h"
#include "NativeSymbolResolver.h"

typedef struct __CFBundle* CFBundleRef;

namespace ClassixCore
{
	class BundleLibrary : public ClassixCore::NativeLibrary
	{
		CFBundleRef bundle;
		
	public:
		BundleLibrary(CFBundleRef bundle);
		BundleLibrary(BundleLibrary&& that);
		
		virtual ~BundleLibrary();
	};
	
	class BundleLibraryResolver : public CFM::LibraryResolver
	{
		Common::IAllocator& allocator;
		std::unordered_map<std::string, CFBundleRef> allBundles;
		std::unordered_set<std::string> allowedBundles;
		std::deque<BundleLibrary> loadedLibraries;
		std::deque<NativeSymbolResolver> resolvers;
		
	public:
		explicit BundleLibraryResolver(Common::IAllocator& allocator, const std::string& directoryPath = ".");
		BundleLibraryResolver(const BundleLibraryResolver& resolver);
		BundleLibraryResolver(BundleLibraryResolver&& resolver);
		
		void AllowLibrary(const std::string& cfmName);
		
		virtual CFM::SymbolResolver* ResolveLibrary(const std::string& name) override;
		virtual ~BundleLibraryResolver() override;
	};
}

#endif /* defined(__Classix__BundleLibraryResolver__) */
