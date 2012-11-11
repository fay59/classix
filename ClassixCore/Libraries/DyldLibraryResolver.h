//
//  DyldLibraryResolver.h
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__DyldLibraryResolver__
#define __Classix__DyldLibraryResolver__

#include <string>
#include <unordered_map>
#include <deque>
#include "LibraryResolver.h"
#include "MachineState.h"
#include "IAllocator.h"
#include "SymbolType.h"
#include "DyldSymbolResolver.h"

namespace ClassixCore
{
	struct DyldLibrary
	{
	private:
		void* dlHandle;
		
	public:
		typedef void* (*InitFunction)(Common::IAllocator*);
		typedef SymbolType (*LookupFunction)(void*, const char*, void**);
		typedef void (*FinitFunction)(void*);
		
		std::string Name;
		InitFunction Init;
		LookupFunction Lookup;
		FinitFunction Finit;
		
		DyldLibrary(const std::string& Name, InitFunction init, LookupFunction lookup, FinitFunction finit);
		DyldLibrary(const std::string& path);
		DyldLibrary(const DyldLibrary& that) = delete;
		DyldLibrary(DyldLibrary&& that);
		
		~DyldLibrary();
	};
	
	class DyldSymbolResolver;
	
	class DyldLibraryResolver : public CFM::LibraryResolver
	{
		Common::IAllocator* allocator;
		std::unordered_map<std::string, DyldLibrary> libraries;
		std::deque<DyldSymbolResolver> resolvers;
		
	public:
		DyldLibraryResolver(Common::IAllocator* allocator);
		
		void RegisterLibrary(const std::string& cfmName);
		void RegisterLibrary(const std::string& cfmName, const std::string& path);
		void RegisterLibrary(const std::string& cfmName, DyldLibrary&& library);
		
		virtual CFM::SymbolResolver* ResolveLibrary(const std::string& name);
		virtual ~DyldLibraryResolver();
	};
}

#endif /* defined(__Classix__DyldLibraryResolver__) */
