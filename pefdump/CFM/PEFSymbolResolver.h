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
#include "IAllocator.h"

namespace CFM
{
	class PEFSymbolResolver : public SymbolResolver
	{
		FragmentManager& cfm;
		FileMapping mapping;
		PEF::Container container;
		Common::IAllocator* allocator;
		
		void Fixup();
		
	public:
		PEFSymbolResolver(Common::IAllocator* allocator, FragmentManager& cfm, const std::string& filePath);
		PEFSymbolResolver(Common::IAllocator* allocator, FragmentManager& cfm, FileMapping&& mapping);
		
		virtual SymbolResolver::MainSymbol GetMainSymbol();
		virtual intptr_t ResolveSymbol(const std::string& symbolName);
		
		virtual ~PEFSymbolResolver();
	};
}

#endif /* defined(__pefdump__PEFSymbolResolver__) */
