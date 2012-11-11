//
//  DyldSymbolResolver.cpp
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "DyldSymbolResolver.h"
#include "DyldLibraryResolver.h"

namespace ClassixCore
{
	DyldSymbolResolver::DyldSymbolResolver(Common::IAllocator* allocator, const DyldLibrary& library)
	: library(library)
	, allocator(allocator)
	, stlAllocator(allocator)
	, transitions(stlAllocator)
	, nativeCalls(stlAllocator)
	{
		globals = library.Init(allocator);
	}
	
	ResolvedSymbol DyldSymbolResolver::GetInitAddress()
	{
		return ResolvedSymbol::Invalid;
	}
	
	ResolvedSymbol DyldSymbolResolver::GetMainAddress()
	{
		return ResolvedSymbol::Invalid;
	}
	
	ResolvedSymbol DyldSymbolResolver::GetTermAddress()
	{
		return ResolvedSymbol::Invalid;
	}
	
	ResolvedSymbol& DyldSymbolResolver::CacheSymbol(const std::string& name, void* address)
	{
		intptr_t ppcAddress = allocator->ToIntPtr(address);
		return symbols.emplace(std::make_pair(name, ResolvedSymbol::IntelSymbol(ppcAddress))).first->second;
	}
	
	PEF::TransitionVector& DyldSymbolResolver::MakeTransitionVector(const std::string& symbolName, void* address)
	{
		stlAllocator.NextName = "Native Call Trampoline Block";
		nativeCalls.emplace_back((NativeCallback)address);
		NativeCall& call = nativeCalls.back();
		
		PEF::TransitionVector vector;
		vector.EntryPoint = allocator->ToIntPtr(&call);
		vector.TableOfContents = allocator->ToIntPtr(globals);
		
		stlAllocator.NextName = "Transition Vector [" + symbolName + "]";
		return *transitions.emplace(transitions.end(), vector);
	}
	
	ResolvedSymbol DyldSymbolResolver::ResolveSymbol(const std::string& name)
	{
		// do we have a cached version?
		auto iter = symbols.find(name);
		if (iter != symbols.end())
			return iter->second;
		
		void* symbol;
		SymbolType type = library.Lookup(globals, name.c_str(), &symbol);
		
		if (type == DataSymbol)
		{
			return CacheSymbol(name, symbol);
		}
		else if (type == CodeSymbol)
		{
			return CacheSymbol(name, &MakeTransitionVector(name, symbol));
		}
		else
		{
			return ResolvedSymbol::Invalid;
		}
	}
	
	DyldSymbolResolver::~DyldSymbolResolver()
	{ }
}
