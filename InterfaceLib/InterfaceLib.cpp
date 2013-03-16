//
//  InterfaceLib.cpp
//  Classix
//
//  Created by Félix on 2013-03-16.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#include "InterfaceLib.h"
#include <cassert>
#include <dlfcn.h>

namespace InterfaceLib
{
	struct Globals {};
}

InterfaceLib::Globals* LibraryInit(Common::IAllocator* allocator)
{
	return nullptr;
}

SymbolType LibraryLookup(InterfaceLib::Globals* globals, const char* symbolName, void** result)
{
	char functionName[56] = "InterfaceLib_";
	char* end = stpncpy(functionName + 13, symbolName, 42);
	assert(*end == '\0' && "symbol name is too long");
	
	if (void* symbol = dlsym(RTLD_SELF, functionName))
	{
		*result = symbol;
		return CodeSymbol;
	}
	
	*result = nullptr;
	return SymbolNotFound;
}

void LibraryFinit(InterfaceLib::Globals* context)
{
	
}