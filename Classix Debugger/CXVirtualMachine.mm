//
//  CXVirtualMachine.m
//  Classix
//
//  Created by Félix on 2012-11-25.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "CXVirtualMachine.h"
#import "CXRegister.h"

#include "MachineState.h"
#include "FragmentManager.h"
#include "NativeAllocator.h"
#include "PEFLibraryResolver.h"
#include "Interpreter.h"
#include "DlfcnLibraryResolver.h"
#include "FancyDisassembler.h"
#include "CXObjcDisassemblyWriter.h"
#include <unordered_set>

NSNumber* CXVirtualMachineGPRKey = @(CXRegisterGPR);
NSNumber* CXVirtualMachineFPRKey = @(CXRegisterFPR);
NSNumber* CXVirtualMachineSPRKey = @(CXRegisterSPR);
NSNumber* CXVirtualMachineCRKey = @(CXRegisterCR);

struct ClassixCoreVM
{
	Common::IAllocator* allocator;
	PPCVM::MachineState state;
	CFM::FragmentManager cfm;
	CFM::PEFLibraryResolver pefResolver;
	ClassixCore::DlfcnLibraryResolver dlfcnResolver;
	PPCVM::Execution::Interpreter interp;
	PEF::Container* container;
	
	std::unordered_set<intptr_t> breakpoints;
	intptr_t nextPC;
	
	ClassixCoreVM(Common::IAllocator* allocator)
	: allocator(allocator)
	, state()
	, cfm()
	, pefResolver(allocator, cfm)
	, dlfcnResolver(allocator)
	, interp(allocator, &state)
	, container(nullptr)
	, nextPC(0)
	{
		dlfcnResolver.RegisterLibrary("StdCLib");
		cfm.LibraryResolvers.push_back(&pefResolver);
		cfm.LibraryResolvers.push_back(&dlfcnResolver);
	}
	
	bool LoadContainer(const std::string& path)
	{
		using namespace PEF;
		
		if (cfm.LoadContainer(path))
		{
			CFM::SymbolResolver* resolver = cfm.GetSymbolResolver(path);
			if (CFM::PEFSymbolResolver* pefResolver = dynamic_cast<CFM::PEFSymbolResolver*>(resolver))
			{
				container = &pefResolver->GetContainer();
				auto main = pefResolver->GetMainAddress();
				if (main.Universe == CFM::SymbolUniverse::PowerPC)
				{
					nextPC = main.Address;
					
					auto& section = container->GetSection(SectionForPC());
					if (section.GetSectionType() != SectionType::Code && section.GetSectionType() != SectionType::ExecutableData)
					{
						// then it's a transition vector
						const TransitionVector* vector = allocator->ToPointer<TransitionVector>(nextPC);
						nextPC = vector->EntryPoint;
					}
					return true;
				}
			}
		}
		return false;
	}
	
	int SectionForPC() const
	{
		void* ptr = allocator->ToPointer<void>(nextPC);
		for (int i = 0; i < container->Size(); i++)
		{
			auto& section = container->GetSection(i);
			if (section.Data <= ptr && section.Data + section.Size() > ptr)
				return i;
		}
		return -1;
	}
};

@implementation CXVirtualMachine

@synthesize allRegisters = registers;

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	vm = new ClassixCoreVM(Common::NativeAllocator::Instance);
	
	NSMutableArray* gpr = [NSMutableArray arrayWithCapacity:32];
	NSMutableArray* fpr = [NSMutableArray arrayWithCapacity:32];
	NSMutableArray* cr = [NSMutableArray arrayWithCapacity:8];
	NSMutableArray* spr = [NSMutableArray array];
	
	for (int i = 0; i < 32; i++)
	{
		CXRegister* r = [CXRegister GPRNumber:i location:&vm->state.gpr[i]];
		CXRegister* fr = [CXRegister FPRNumber:i location:&vm->state.fpr[i]];
		[gpr addObject:r];
		[fpr addObject:fr];
	}
	
	for (int i = 0; i < 8; i++)
	{
		CXRegister* reg = [CXRegister CRNumber:i location:&vm->state.cr[i]];
		[cr addObject:reg];
	}
	
	[spr addObject:[CXRegister SPRName:@"xer" location:&vm->state.xer]];
	[spr addObject:[CXRegister SPRName:@"lr" location:&vm->state.lr]];
	[spr addObject:[CXRegister SPRName:@"ctr" location:&vm->state.ctr]];
	
	registers = [@{
		CXVirtualMachineGPRKey: gpr,
		CXVirtualMachineFPRKey: fpr,
		CXVirtualMachineSPRKey: spr,
		CXVirtualMachineCRKey: cr
	} retain];
	
	return self;
}

-(NSArray*)gpr
{
	return [registers objectForKey:CXVirtualMachineGPRKey];
}

-(NSArray*)fpr
{
	return [registers objectForKey:CXVirtualMachineFPRKey];
}

-(NSArray*)spr
{
	return [registers objectForKey:CXVirtualMachineSPRKey];
}

-(NSArray*)cr
{
	return [registers objectForKey:CXVirtualMachineCRKey];
}

-(void)dealloc
{
	delete vm;
	[registers release];
	[super dealloc];
}

@end
