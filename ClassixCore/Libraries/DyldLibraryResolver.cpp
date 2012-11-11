//
//  DyldLibraryResolver.cpp
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
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
