//
//  PEFSymbolResolver.h
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__PEFSymbolResolver__
#define __pefdump__PEFSymbolResolver__

#include "SymbolResolver.h"
#include "Container.h"
#include "FileMapping.h"
#include "FragmentManager.h"
#include "MemoryManager.h"
#include "MemoryManagerAllocator.h"

namespace CFM
{
	class PEFSymbolResolver : public virtual SymbolResolver
	{
		FileMapping mapping;
		PPCVM::MemoryManagerAllocator allocator;
		PEF::Container container;
		
		FragmentManager& cfm;
		PPCVM::MemoryManager& memoryManager;
		
		ResolvedSymbol Symbolize(const uint8_t* address);
		
	public:
		PEFSymbolResolver(PPCVM::MemoryManager& memMan, FragmentManager& cfm, const std::string& filePath);
		PEFSymbolResolver(PPCVM::MemoryManager& memMan, FragmentManager& cfm, FileMapping&& mapping);
		
		virtual SymbolResolver::MainSymbol GetMainSymbol();
		virtual ResolvedSymbol ResolveSymbol(const std::string& symbolName);
		
		virtual ~PEFSymbolResolver();
	};
}

#endif /* defined(__pefdump__PEFSymbolResolver__) */
