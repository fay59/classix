//
//  BridgeLibraryResolver.cpp
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <objc/objc-runtime.h>
#include "BridgeLibraryResolver.h"
#include "BridgeSymbolResolver.h"
#import "PPCLibrary.h"
#import "PPCAllocator.h"

namespace ObjCBridge
{
	BridgeLibraryResolver::BridgeLibraryResolver(Common::IAllocator* allocator)
	: allocator(allocator)
	{
		objcAllocator = [[PPCAllocator alloc] initWithAllocator:allocator];
	}
	
	CFM::SymbolResolver* BridgeLibraryResolver::ResolveLibrary(const std::string &name)
	{
		// TODO load from bundles
		// (that would be very cool!)
		
		Class cls = objc_getClass(name.c_str());
		if (cls == nullptr)
			return nullptr;
		
		id<PPCLibrary> library = [[[cls alloc] initWithAllocator:(PPCAllocator*)objcAllocator] autorelease];
		return new BridgeSymbolResolver(allocator, library);
	}
	
	BridgeLibraryResolver::~BridgeLibraryResolver()
	{
		[(NSObject*)objcAllocator release];
	}
}
