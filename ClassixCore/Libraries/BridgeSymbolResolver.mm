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
#include "NativeCall.h"
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
		x86.dd(PPCVM::Execution::NativeTag); // native marker
		x86.newFunction(CALL_CONV_DEFAULT, FunctionBuilder1<void, MachineState*>());
		
		GPVar gpSender(x86.newGP(VARIABLE_TYPE_GPD));
		GPVar gpSel(x86.newGP(VARIABLE_TYPE_GPD));
		
		x86.mov(gpSender, imm((intptr_t)sender));
		x86.mov(gpSel, imm((intptr_t)sel));
		
		ECall* call = x86.call(implementation);
		call->setPrototype(CALL_CONV_CDECL, FunctionBuilder3<void, id, SEL, MachineState*>());
		call->setArgument(0, gpSender);
		call->setArgument(1, gpSel);
		call->setArgument(2, GPVar(x86.argGP(0)));
		x86.endFunction();
		
		void* trampoline = x86.make();
		if (trampoline == nullptr)
			throw std::logic_error("could not generate trampoline");
		
		return trampoline;
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
	: allocator(allocator), listNodeAllocator(allocator), transitions(listNodeAllocator)
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
	
	ResolvedSymbol& BridgeSymbolResolver::CacheCodeSymbol(const std::string &name, void *address)
	{
		PEF::TransitionVector transition;
		transition.EntryPoint = reinterpret_cast<intptr_t>(address);
		transition.TableOfContents = 0;
		
		listNodeAllocator.NextName = "Transition vector [" + name + "]";
		auto iter = transitions.insert(transitions.end(), transition);
		return CacheSymbol(name, &*iter);
	}
	
	ResolvedSymbol& BridgeSymbolResolver::CacheDataSymbol(const std::string &name, void *address)
	{
		return CacheSymbol(name, address);
	}
	
	std::string& BridgeSymbolResolver::LibraryName()
	{
		return libraryName;
	}
	
	const std::string& BridgeSymbolResolver::LibraryName() const
	{
		return libraryName;
	}
	
	ResolvedSymbol BridgeSymbolResolver::GetInitAddress()
	{
		return ResolvedSymbol::Invalid;
	}
	
	ResolvedSymbol BridgeSymbolResolver::GetMainAddress()
	{
		return ResolvedSymbol::Invalid;
	}
	
	ResolvedSymbol BridgeSymbolResolver::GetTermAddress()
	{
		return ResolvedSymbol::Invalid;
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
			return CacheDataSymbol(name, address);
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
				return CacheCodeSymbol(name, trampoline);
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
					return CacheCodeSymbol(name, trampoline);
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
