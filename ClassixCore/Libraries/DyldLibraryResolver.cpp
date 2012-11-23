//
// DyldLibraryResolver.cpp
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

#include <dlfcn.h>
#include "DyldLibraryResolver.h"
#include "DyldSymbolResolver.h"

namespace ClassixCore
{
	namespace
	{
		template<typename T>
		inline T dlsym(void* handle, const char* symbol)
		{
			void* ptr = ::dlsym(handle, symbol);
			return reinterpret_cast<T>(ptr);
		}
	}
	
	DyldLibrary::DyldLibrary(const std::string& name, InitFunction init, LookupFunction lookup, FinitFunction finit)
	: Name(name)
	{
		Init = init;
		Lookup = lookup;
		Finit = finit;
		dlHandle = nullptr;
	}
	
	DyldLibrary::DyldLibrary(DyldLibrary&& that)
	{
		Init = that.Init;
		Lookup = that.Lookup;
		Finit = that.Finit;
		dlHandle = that.dlHandle;
		that.dlHandle = nullptr;
	}
	
	DyldLibrary::DyldLibrary(const std::string& path)
	{
		dlHandle = dlopen(path.c_str(), RTLD_LOCAL | RTLD_LAZY);
		if (dlHandle == nullptr)
			throw std::logic_error(dlerror());
		
		// this works even if there are no slashes in the name because string::npos + 1 == 0
		Name = path.substr(path.find_last_of('/') + 1);
		
		Init = dlsym<InitFunction>(dlHandle, "LibraryInit");
		Lookup = dlsym<LookupFunction>(dlHandle, "LibraryLookup");
		Finit = dlsym<FinitFunction>(dlHandle, "LibraryFinit");
		
		if (Init == nullptr || Lookup == nullptr || Finit == nullptr)
			throw std::logic_error("Incomplete library");
	}
	
	DyldLibrary::~DyldLibrary()
	{
		dlclose(dlHandle);
	}
	
	DyldLibraryResolver::DyldLibraryResolver(Common::IAllocator* allocator)
	: allocator(allocator)
	{ }
	
	void DyldLibraryResolver::RegisterLibrary(const std::string& cfmName)
	{
		RegisterLibrary(cfmName, DyldLibrary("lib" + cfmName + ".dylib"));
	}
	
	void DyldLibraryResolver::RegisterLibrary(const std::string& cfmName, const std::string& path)
	{
		RegisterLibrary(cfmName, DyldLibrary(path));
	}
	
	void DyldLibraryResolver::RegisterLibrary(const std::string& cfmName, DyldLibrary&& library)
	{
		libraries.emplace(std::make_pair(cfmName, std::move(library)));
	}
	
	CFM::SymbolResolver* DyldLibraryResolver::ResolveLibrary(const std::string& name)
	{
		auto iter = libraries.find(name);
		if (iter == libraries.end())
			return nullptr;
		
		resolvers.emplace_back(allocator, iter->second);
		return &resolvers.back();
	}
	
	DyldLibraryResolver::~DyldLibraryResolver()
	{ }
}
