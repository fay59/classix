//
//  StdCLib.h
//  Classix
//
//  Created by Félix on 2012-11-11.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef Classix_StdCLib_h
#define Classix_StdCLib_h

#include "IAllocator.h"
#include "MachineState.h"
#include "SymbolType.h"

extern "C"
{
	struct StdCLibGlobals;

	StdCLibGlobals* LibraryInit(Common::IAllocator* allocator);
	SymbolType LibraryLookup(StdCLibGlobals* globals, const char* symbolName, void** symbol);
	void LibraryFinit(StdCLibGlobals* context);
}

#endif
