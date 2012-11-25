//
// DlfcnLibraryResolver.cpp
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
#include "DlfcnLibraryResolver.h"
#include "DlfcnSymbolResolver.h"

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
	
	DlfcnLibrary::DlfcnLibrary(DlfcnLibrary&& that)
	: Path(that.Path), Name(that.Name)
	{
		Init = that.Init;
		Lookup = that.Lookup;
		Finit = that.Finit;
		dlHandle = that.dlHandle;
		Symbols = that.Symbols;
		that.dlHandle = nullptr;
	}
	
	DlfcnLibrary::DlfcnLibrary(const std::string& path)
	{
		dlHandle = dlopen(path.c_str(), RTLD_LOCAL | RTLD_LAZY);
		if (dlHandle == nullptr)
			throw std::logic_error(dlerror());
		
		Init = dlsym<InitFunction>(dlHandle, "LibraryInit");
		Lookup = dlsym<LookupFunction>(dlHandle, "LibraryLookup");
		Finit = dlsym<FinitFunction>(dlHandle, "LibraryFinit");
		Symbols = dlsym<const char**>(dlHandle, "LibrarySymbolNames");
		
		if (Init == nullptr || Lookup == nullptr || Finit == nullptr)
			throw std::logic_error("Incomplete library");
		
		// get the full file path
		Dl_info info;
		Path = dladdr(reinterpret_cast<void*>(Init), &info) == 0 ? path : info.dli_fname;
		Name = Path.substr(path.find_last_of('/') + 1);
	}
	
	DlfcnLibrary::~DlfcnLibrary()
	{
		dlclose(dlHandle);
	}
	
	DlfcnLibraryResolver::DlfcnLibraryResolver(Common::IAllocator* allocator)
	: allocator(allocator)
	{ }
	
	void DlfcnLibraryResolver::RegisterLibrary(const std::string& cfmName)
	{
		RegisterLibrary(cfmName, DlfcnLibrary("lib" + cfmName + ".dylib"));
	}
	
	void DlfcnLibraryResolver::RegisterLibrary(const std::string& cfmName, const std::string& path)
	{
		RegisterLibrary(cfmName, DlfcnLibrary(path));
	}
	
	void DlfcnLibraryResolver::RegisterLibrary(const std::string& cfmName, DlfcnLibrary&& library)
	{
		libraries.emplace(std::make_pair(cfmName, std::move(library)));
	}
	
	CFM::SymbolResolver* DlfcnLibraryResolver::ResolveLibrary(const std::string& name)
	{
		auto iter = libraries.find(name);
		if (iter == libraries.end())
			return nullptr;
		
		resolvers.emplace_back(allocator, iter->second);
		return &resolvers.back();
	}
	
	DlfcnLibraryResolver::~DlfcnLibraryResolver()
	{ }
}
