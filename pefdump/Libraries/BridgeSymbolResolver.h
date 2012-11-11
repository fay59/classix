//
//  BridgeSymbolResolver.h
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__BridgeSymbolResolver__
#define __pefdump__BridgeSymbolResolver__

#include <map>
#include <list>
#include "Structures.h"
#include "SymbolResolver.h"
#include "MachineState.h"
#include "IAllocator.h"
#include "STAllocator.h"

namespace ObjCBridge
{
	using namespace CFM;
	
	class BridgeSymbolResolver : public virtual SymbolResolver
	{
		void* library;
		std::string libraryName;
		Common::IAllocator* allocator;
		Common::STAllocator<PEF::TransitionVector> listNodeAllocator;
		
		std::map<std::string, ResolvedSymbol> symbols;
		std::list<void*> trampolines;
		std::list<PEF::TransitionVector, Common::STAllocator<PEF::TransitionVector>> transitions;
		
		ResolvedSymbol& CacheSymbol(const std::string& name, void* address);
		ResolvedSymbol& CacheDataSymbol(const std::string& name, void* address);
		ResolvedSymbol& CacheCodeSymbol(const std::string& name, void* address);
		
	public:
		BridgeSymbolResolver(Common::IAllocator* allocator, void* libraryAsVoid);
		
		std::string& LibraryName();
		const std::string& LibraryName() const;
		
		virtual ResolvedSymbol GetInitAddress();
		virtual ResolvedSymbol GetMainAddress();
		virtual ResolvedSymbol GetTermAddress();
		
		virtual ResolvedSymbol ResolveSymbol(const std::string& name);
		virtual ~BridgeSymbolResolver();
	};
}

#endif /* defined(__pefdump__BridgeSymbolResolver__) */
