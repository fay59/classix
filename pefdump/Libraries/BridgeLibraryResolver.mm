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
#import "PPCMachineState.h"
#import "PPCAllocator.h"

#define STATE reinterpret_cast<PPCMachineState*>(state)

namespace ObjCBridge
{
	BridgeLibraryResolver::BridgeLibraryResolver(Common::IAllocator* allocator, PPCVM::MachineState& state)
	: allocator(allocator)
	{
		objcAllocator = [[PPCAllocator alloc] initWithAllocator:allocator];
		this->state = [[PPCMachineState alloc] initWithMachineState:&state];
	}
	
	CFM::SymbolResolver* BridgeLibraryResolver::ResolveLibrary(const std::string &name)
	{
		// TODO load from bundles
		// (that would be very cool!)
		
		Class cls = objc_getClass(name.c_str());
		if (cls == nullptr)
			return nullptr;
		
		id<PPCLibrary> library = [[[cls alloc] initWithAllocator:(PPCAllocator*)objcAllocator] autorelease];
		return new BridgeSymbolResolver(allocator, state, library);
	}
	
	BridgeLibraryResolver::~BridgeLibraryResolver()
	{
		[(NSObject*)objcAllocator release];
		[STATE release];
	}
}
