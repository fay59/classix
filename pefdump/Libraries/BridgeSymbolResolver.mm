//
//  BridgeSymbolResolver.cpp
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <objc/runtime.h>
#include "BridgeSymbolResolver.h"
#include "MemoryManagerAllocator.h"
#import "PPCLibrary.h"

#define LIBRARY reinterpret_cast< id<PPCLibrary> >(library)
#define STATE reinterpret_cast<PPCMachineState*>(machineState)

typedef void* (*DataSymbolMethod)(id, SEL);

namespace ObjCBridge
{
	BridgeSymbolResolver::BridgeSymbolResolver(Common::IAllocator* allocator, void* machineStateAsVoid, void* idAsVoid)
	{
		machineState = [reinterpret_cast<PPCMachineState*>(machineStateAsVoid) retain];
		library = [reinterpret_cast< id<PPCLibrary> >(idAsVoid) retain];
		
		this->allocator = allocator;
		
		@autoreleasepool
		{
			libraryName = [[LIBRARY libraryName] UTF8String];
		}
	}
	
	ResolvedSymbol& BridgeSymbolResolver::CacheSymbol(const std::string& name, void *address)
	{
		auto symbol = ResolvedSymbol::IntelSymbol(reinterpret_cast<intptr_t>(address));
		auto pair = symbols.insert(std::pair<std::string, ResolvedSymbol>(name, std::move(symbol)));
		return pair.first->second;
	}
	
	std::string& BridgeSymbolResolver::LibraryName()
	{
		return libraryName;
	}
	
	const std::string& BridgeSymbolResolver::LibraryName() const
	{
		return libraryName;
	}
	
	SymbolResolver::MainSymbol BridgeSymbolResolver::GetMainSymbol()
	{
		// Intel bridged libraries cannot have a main symbol--sorry
		return nullptr;
	}
	
	ResolvedSymbol BridgeSymbolResolver::ResolveSymbol(const std::string &name)
	{
		// do we have a cached version?
		auto iter = symbols.find(name);
		if (iter != symbols.end())
			return iter->second;
		
		NSMutableString* selectorName = [[NSMutableString alloc] initWithFormat:@"%s_%s", libraryName.c_str(), name.c_str()];
		// can we get a data symbol with that name?
		SEL dataSymbolSel = NSSelectorFromString(selectorName);
		if ([LIBRARY respondsToSelector:dataSymbolSel])
		{
			Method method = class_getInstanceMethod([LIBRARY class], dataSymbolSel);
			IMP implementation = method_getImplementation(method);
			DataSymbolMethod symbolGetter = reinterpret_cast<DataSymbolMethod>(implementation);
			void* address = symbolGetter(LIBRARY, dataSymbolSel);
			return CacheSymbol(name, address);
		}
		else
		{
			[selectorName appendString:@":"];
			// then can we get a function symbol with that name?
			// TODO trampoline stuff
			return ResolvedSymbol::IntelSymbol(0);
		}
		[selectorName release];
	}
	
	BridgeSymbolResolver::~BridgeSymbolResolver()
	{
		[LIBRARY release];
		[STATE release];
	}
}
