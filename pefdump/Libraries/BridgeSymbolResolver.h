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
#include "SymbolResolver.h"
#include "IAllocator.h"

namespace ObjCBridge
{
	using namespace CFM;
	
	class BridgeSymbolResolver : public virtual SymbolResolver
	{
		void* machineState;
		void* library;
		std::string libraryName;
		std::map<std::string, ResolvedSymbol> symbols;
		Common::IAllocator* allocator;
		
		ResolvedSymbol& CacheSymbol(const std::string& name, void* address);
		
	public:
		BridgeSymbolResolver(Common::IAllocator* allocator, void* machineStateAsVoid, void* libraryAsVoid);
		
		std::string& LibraryName();
		const std::string& LibraryName() const;
		
		virtual SymbolResolver::MainSymbol GetMainSymbol();
		virtual ResolvedSymbol ResolveSymbol(const std::string& name);
		virtual ~BridgeSymbolResolver();
	};
}

#endif /* defined(__pefdump__BridgeSymbolResolver__) */
