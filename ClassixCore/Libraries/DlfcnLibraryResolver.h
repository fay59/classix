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
#include "NativeLibrary.h"
#include "MachineState.h"
#include "IAllocator.h"
#include "SymbolType.h"
#include "NativeSymbolResolver.h"

namespace ClassixCore
{
	class DlfcnLibrary : public NativeLibrary
	{
		void* dlHandle;
		
	public:
		DlfcnLibrary(const std::string& path);
		DlfcnLibrary(DlfcnLibrary&& that);
		
		virtual ~DlfcnLibrary() override;
	};
	
	class DlfcnLibraryResolver : public CFM::LibraryResolver
	{
		Common::IAllocator& allocator;
		OSEnvironment::Managers& managers;
		std::unordered_map<std::string, DlfcnLibrary> libraries;
		std::deque<NativeSymbolResolver> resolvers;
		
	public:
		DlfcnLibraryResolver(Common::IAllocator& allocator, OSEnvironment::Managers& managers);
		DlfcnLibraryResolver(const DlfcnLibraryResolver&) = delete;
		
		void RegisterLibrary(const std::string& cfmName);
		void RegisterLibrary(const std::string& cfmName, const std::string& path);
		void RegisterLibrary(const std::string& cfmName, DlfcnLibrary&& library);
		
		virtual CFM::SymbolResolver* ResolveLibrary(const std::string& name) override;
		virtual ~DlfcnLibraryResolver() override;
	};
}

#endif /* defined(__Classix__DlfcnLibraryResolver__) */
