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
	{
		Path = std::move(that.Path);
		Name = std::move(that.Name);
		OnLoad = std::move(that.OnLoad);
		Lookup = std::move(that.Lookup);
		OnUnload = std::move(that.OnUnload);
		Symbols = that.Symbols;
		dlHandle = that.dlHandle;
		that.dlHandle = nullptr;
	}
	
	DlfcnLibrary::DlfcnLibrary(const std::string& path)
	{
		dlHandle = dlopen(path.c_str(), RTLD_LOCAL | RTLD_LAZY);
		if (dlHandle == nullptr)
			throw std::logic_error(dlerror());
		
		OnLoad = dlsym<OnLoadFunction>(dlHandle, "LibraryLoad");
		Lookup = dlsym<LookupFunction>(dlHandle, "LibraryLookup");
		OnUnload = dlsym<OnUnloadFunction>(dlHandle, "LibraryUnload");
		Symbols = dlsym<const char**>(dlHandle, "LibrarySymbolNames");
		
		if (OnLoad == nullptr || Lookup == nullptr || OnUnload == nullptr || Symbols == nullptr)
			throw std::logic_error("Incomplete library");
		
		// get the full file path
		Dl_info info;
		Path = dladdr(reinterpret_cast<void*>(OnLoad), &info) == 0 ? path : info.dli_fname;
		Name = Path.substr(path.find_last_of('/') + 1);
	}
	
	DlfcnLibrary::~DlfcnLibrary()
	{
		dlclose(dlHandle);
	}
	
	DlfcnLibraryResolver::DlfcnLibraryResolver(Common::IAllocator& allocator, OSEnvironment::Managers& managers)
	: allocator(allocator), managers(managers)
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
		
		resolvers.emplace_back(allocator, managers, iter->second);
		return &resolvers.back();
	}
	
	DlfcnLibraryResolver::~DlfcnLibraryResolver()
	{ }
}
