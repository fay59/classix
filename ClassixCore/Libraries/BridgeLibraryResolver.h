//
//  BridgeLibraryResolver.h
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__BridgeLibraryResolver__
#define __pefdump__BridgeLibraryResolver__

#include "LibraryResolver.h"
#include "MachineState.h"
#include "IAllocator.h"

namespace ObjCBridge
{
	class BridgeLibraryResolver : public virtual CFM::LibraryResolver
	{
		Common::IAllocator* allocator;
		void* objcAllocator;
		
	public:
		BridgeLibraryResolver(Common::IAllocator* allocator);
		
		virtual CFM::SymbolResolver* ResolveLibrary(const std::string& name);
		virtual ~BridgeLibraryResolver();
	};
}

#endif /* defined(__pefdump__BridgeLibraryResolver__) */
