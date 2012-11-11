//
//  DyldSymbolResolver.h
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__DyldSymbolResolver__
#define __Classix__DyldSymbolResolver__

#include <string>
#include <unordered_map>
#include <list>
#include <deque>
#include "IAllocator.h"
#include "STAllocator.h"
#include "Structures.h"
#include "SymbolResolver.h"
#include "NativeCall.h"

namespace ClassixCore
{
	using namespace CFM;
	using namespace PPCVM::Execution;
	
	class DyldLibrary;
	
	class DyldSymbolResolver : public SymbolResolver
	{
		const DyldLibrary& library;
		void* globals;
		
		// symbol cache
		Common::IAllocator* allocator;
		Common::STAllocator<uint8_t> stlAllocator;
		std::list<PEF::TransitionVector, Common::STAllocator<PEF::TransitionVector>> transitions;
		std::deque<NativeCall, Common::STAllocator<NativeCall>> nativeCalls;
		std::unordered_map<std::string, ResolvedSymbol> symbols;
		
		ResolvedSymbol& CacheSymbol(const std::string& name, void* address);
		PEF::TransitionVector& MakeTransitionVector(const std::string& symbolName, void* address);
		
	public:
		DyldSymbolResolver(Common::IAllocator* allocator, const DyldLibrary& library);
		
		virtual ResolvedSymbol GetInitAddress();
		virtual ResolvedSymbol GetMainAddress();
		virtual ResolvedSymbol GetTermAddress();
		
		virtual ResolvedSymbol ResolveSymbol(const std::string& name);
		virtual ~DyldSymbolResolver();
	};
}

#endif /* defined(__Classix__DyldSymbolResolver__) */
