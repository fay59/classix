//
//  BridgeSymbolResolver.cpp
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <objc/runtime.h>
#include "BridgeSymbolResolver.h"
#include "AsmJit.h"
#import "PPCLibrary.h"

#define LIBRARY reinterpret_cast< id<PPCLibrary> >(library)

typedef void* (*DataSymbolMethod)(id, SEL);

namespace
{
	class Autoreleaser
	{
		id<NSObject> obj;
	public:
		Autoreleaser(id<NSObject> obj) : obj(obj) { }
		~Autoreleaser() { [obj release]; }
	};
	
	void* MakeTrampoline(void* sender, SEL sel, void* implementation)
	{
		using namespace AsmJit;
		Compiler x86;
		x86.dd(0xffffffff); // native marker
		x86.newFunction(CALL_CONV_DEFAULT, FunctionBuilder1<void, MachineState*>());
		ECall* call = x86.call(implementation);
		call->setPrototype(CALL_CONV_CDECL, FunctionBuilder3<void, id, SEL, MachineState*>());
		call->setArgument(0, imm((intptr_t)sender));
		call->setArgument(1, imm((intptr_t)sel));
		call->setArgument(2, GPVar(x86.argGP(1)));
		return x86.make();
	}
	
	void* MakeTrampoline(void* sender, Class cls, SEL sel)
	{
		Method method = class_getInstanceMethod(cls, sel);
		if (method == nullptr)
			throw std::logic_error("Class does not respond to selector");
		
		IMP imp = method_getImplementation(method);
		return MakeTrampoline(sender, sel, reinterpret_cast<void*>(imp));
	}
}

namespace ObjCBridge
{
	BridgeSymbolResolver::BridgeSymbolResolver(Common::IAllocator* allocator, void* idAsVoid)
	{
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
		
		Class cls = [LIBRARY class];
		NSMutableString* selectorName = [[NSMutableString alloc] initWithFormat:@"%s_%s", libraryName.c_str(), name.c_str()];
		Autoreleaser selectorNameRelease(selectorName);
		
		// can we get a data symbol with that name?
		SEL dataSymbolSel = NSSelectorFromString(selectorName);
		if ([LIBRARY respondsToSelector:dataSymbolSel])
		{
			Method method = class_getInstanceMethod(cls, dataSymbolSel);
			IMP implementation = method_getImplementation(method);
			DataSymbolMethod symbolGetter = reinterpret_cast<DataSymbolMethod>(implementation);
			void* address = symbolGetter(LIBRARY, dataSymbolSel);
			return CacheSymbol(name, address);
		}
		else
		{
			[selectorName appendString:@":"];
			SEL codeSymbolSel = NSSelectorFromString(selectorName);
			
			// then can we get a function symbol with that name?
			if ([LIBRARY respondsToSelector:codeSymbolSel])
			{
				void* trampoline = MakeTrampoline(library, cls, codeSymbolSel);
				trampolines.push_back(trampoline);
				return CacheSymbol(name, trampoline);
			}
			else
			{
				// well then, try the -resolve method
				NSString* objcName = [[NSString alloc] initWithCString:name.c_str() encoding:NSUTF8StringEncoding];
				Autoreleaser objcNameRelease(objcName);
				PPCLibraryFunction func = [LIBRARY resolve:objcName];
				if (func != nullptr)
				{
					void* trampoline = MakeTrampoline(library, codeSymbolSel, (void*)func);
					trampolines.push_back(trampoline);
					return CacheSymbol(name, trampoline);
				}
			}
		}
		return ResolvedSymbol::Invalid;
	}
	
	BridgeSymbolResolver::~BridgeSymbolResolver()
	{
		auto manager = AsmJit::MemoryManager::getGlobal();
		for (void* trampoline : trampolines)
			manager->free(trampoline);
		
		[LIBRARY release];
	}
}
