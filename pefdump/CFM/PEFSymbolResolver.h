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

namespace CFM
{
	class PEFSymbolResolver : public SymbolResolver
	{
		FragmentManager& cfm;
		FileMapping mapping;
		PEF::Container container;
		
		void Fixup();
		
	public:
		PEFSymbolResolver(FragmentManager& cfm, const std::string& filePath);
		PEFSymbolResolver(FragmentManager& cfm, FileMapping&& mapping);
		
		virtual SymbolResolver::MainSymbol GetMainSymbol();
		virtual void* ResolveSymbol(const std::string& symbolName);
		
		virtual ~PEFSymbolResolver();
	};
}

#endif /* defined(__pefdump__PEFSymbolResolver__) */
