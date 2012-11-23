//
// DlfcnSymbolResolver.cpp
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

#include "DlfcnSymbolResolver.h"
#include "DlfcnLibraryResolver.h"

namespace ClassixCore
{
	DlfcnSymbolResolver::DlfcnSymbolResolver(Common::IAllocator* allocator, const DlfcnLibrary& library)
	: library(library)
	, allocator(allocator)
	, stlAllocator(allocator)
	, transitions(stlAllocator)
	, nativeCalls(stlAllocator)
	{
		globals = library.Init(allocator);
	}
	
	ResolvedSymbol DlfcnSymbolResolver::GetInitAddress()
	{
		return ResolvedSymbol::Invalid;
	}
	
	ResolvedSymbol DlfcnSymbolResolver::GetMainAddress()
	{
		return ResolvedSymbol::Invalid;
	}
	
	ResolvedSymbol DlfcnSymbolResolver::GetTermAddress()
	{
		return ResolvedSymbol::Invalid;
	}
	
	ResolvedSymbol& DlfcnSymbolResolver::CacheSymbol(const std::string& name, void* address)
	{
		intptr_t ppcAddress = allocator->ToIntPtr(address);
		return symbols.emplace(std::make_pair(name, ResolvedSymbol::IntelSymbol(ppcAddress))).first->second;
	}
	
	PEF::TransitionVector& DlfcnSymbolResolver::MakeTransitionVector(const std::string& symbolName, void* address)
	{
		stlAllocator.SetNextName("Native Call Trampoline Block");
		nativeCalls.emplace_back((NativeCallback)address);
		NativeCall& call = nativeCalls.back();
		
		PEF::TransitionVector vector;
		vector.EntryPoint = allocator->ToIntPtr(&call);
		vector.TableOfContents = allocator->ToIntPtr(globals);
		
		stlAllocator.SetNextName("Transition Vector [" + symbolName + "]");
		return *transitions.emplace(transitions.end(), vector);
	}
	
	ResolvedSymbol DlfcnSymbolResolver::ResolveSymbol(const std::string& name)
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
	
	DlfcnSymbolResolver::~DlfcnSymbolResolver()
	{ }
}
