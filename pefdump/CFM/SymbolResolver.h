//
//  SymbolResolver.h
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__SymbolResolver__
#define __pefdump__SymbolResolver__

#include <cstdint>
#include <string>

namespace CFM
{
	class SymbolResolver
	{
	public:
		typedef int (*MainSymbol)(int argc, const char** argv);
		
		virtual MainSymbol GetMainSymbol() = 0;
		virtual void* ResolveSymbol(const std::string& name) = 0;
		virtual ~SymbolResolver() = 0;
	};
}

#endif /* defined(__pefdump__SymbolResolver__) */
