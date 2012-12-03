//
// DlfcnLibraryResolver.h
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

#ifndef __Classix__DlfcnLibraryResolver__
#define __Classix__DlfcnLibraryResolver__

#include <string>
#include <unordered_map>
#include <deque>

#include "LibraryResolver.h"
#include "MachineState.h"
#include "IAllocator.h"
#include "SymbolType.h"
#include "DlfcnSymbolResolver.h"

namespace ClassixCore
{
	struct DlfcnLibrary
	{
	private:
		void* dlHandle;
		
	public:
		typedef void* (*InitFunction)(Common::IAllocator*);
		typedef SymbolType (*LookupFunction)(void*, const char*, void**);
		typedef void (*FinitFunction)(void*);
		
		std::string Path;
		std::string Name;
		InitFunction Init;
		LookupFunction Lookup;
		FinitFunction Finit;
		const char** Symbols;
		
		DlfcnLibrary(const std::string& path);
		DlfcnLibrary(const DlfcnLibrary& that) = delete;
		DlfcnLibrary(DlfcnLibrary&& that);
		
		~DlfcnLibrary();
	};
	
	class DlfcnSymbolResolver;
	
	class DlfcnLibraryResolver : public CFM::LibraryResolver
	{
		Common::IAllocator* allocator;
		std::unordered_map<std::string, DlfcnLibrary> libraries;
		std::deque<DlfcnSymbolResolver> resolvers;
		
	public:
		DlfcnLibraryResolver(Common::IAllocator* allocator);
		
		void RegisterLibrary(const std::string& cfmName);
		void RegisterLibrary(const std::string& cfmName, const std::string& path);
		void RegisterLibrary(const std::string& cfmName, DlfcnLibrary&& library);
		
		virtual CFM::SymbolResolver* ResolveLibrary(const std::string& name) override;
		virtual ~DlfcnLibraryResolver() override;
	};
}

#endif /* defined(__Classix__DlfcnLibraryResolver__) */
