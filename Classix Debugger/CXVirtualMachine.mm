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

NSString* CXErrorDomain = @"Classix Error Domain";
NSString* CXErrorFilePath = @"File URL";

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
@synthesize breakpoints;
@synthesize pc;

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

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	vm = new ClassixCoreVM(Common::NativeAllocator::Instance);
	
	NSMutableArray* gpr = [NSMutableArray arrayWithCapacity:32];
	NSMutableArray* fpr = [NSMutableArray arrayWithCapacity:32];
	NSMutableArray* cr = [NSMutableArray arrayWithCapacity:8];
	NSMutableArray* spr = [NSMutableArray array];
	breakpoints = [NSMutableArray array];
	
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

-(BOOL)loadClassicExecutable:(NSString *)executablePath error:(NSError **)error
{
	// TODO check that we haven't already loaded an executable
	std::string path = [executablePath UTF8String];
	
	if (!vm->LoadContainer(path))
	{
		if (error != nullptr)
			*error = [NSError errorWithDomain:CXErrorDomain code:CXErrorCodeFileNotLoadable userInfo:@{CXErrorFilePath: executablePath}];
		return NO;
	}
	return YES;
}

-(NSValue*)fragmentManager
{
	CFM::FragmentManager* cfm = &vm->cfm;
	return [NSValue value:&cfm withObjCType:@encode(typeof cfm)];
}

-(NSValue*)allocator
{
	Common::IAllocator* allocator = vm->allocator;
	return [NSValue value:&allocator withObjCType:@encode(typeof allocator)];
}

-(IBAction)run:(id)sender
{
	std::unordered_set<const void*> cppBreakpoints;
	for (NSNumber* number in breakpoints)
	{
		const void* address = vm->allocator->ToPointer<const void>(number.unsignedIntValue);
		cppBreakpoints.insert(address);
	}
	
	const void* eip = vm->allocator->ToPointer<const void>(pc);
	vm->interp.ExecuteUntil(eip, cppBreakpoints);
}

-(IBAction)stepOver:(id)sender
{
	NSLog(@"*** step over is not currently supported; step into instead");
	[self stepInto:sender];
}

-(IBAction)stepInto:(id)sender
{
	const void* eip = vm->allocator->ToPointer<const void>(pc);
	const void* newEip = vm->interp.ExecuteOne(eip);
	self.pc = vm->allocator->ToIntPtr(newEip);
}

-(void)runTo:(uint32_t)location
{
	std::unordered_set<const void*> until = {vm->allocator->ToPointer<const void>(location)};
	const void* eip = vm->allocator->ToPointer<const void>(pc);
	vm->interp.ExecuteUntil(eip, until);
}

-(void)dealloc
{
	delete vm;
	[registers release];
	[breakpoints release];
	[super dealloc];
}

#pragma mark -
#pragma mark NSOutlineView stuff
-(BOOL)outlineView:(NSOutlineView *)outlineView isGroupItem:(id)item
{
	return [outlineView parentForItem:item] == nil;
}

-(NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (item == nil) return registers.count;
	if ([item respondsToSelector:@selector(count)]) return [item count];
	return 0;
}

-(BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	return [self outlineView:outlineView numberOfChildrenOfItem:item] != 0;
}

-(id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	if (item == nil)
		return [registers objectForKey:@(index)];
	
	return [item objectAtIndex:index];
}

-(id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	if ([outlineView parentForItem:item] == nil)
	{
		if ([tableColumn.identifier isEqualToString:@"Register"])
		{
			static NSString* headers[] = {@"GPR", @"FPR", @"CR", @"SPR"};
			int index = [[[registers allKeysForObject:item] objectAtIndex:0] intValue];
			return headers[index];
		}
		return nil;
	}
	
	NSString* identifier = tableColumn.identifier;
	if ([identifier isEqualToString:@"Register"])
		return [item name];
	else if ([identifier isEqualToString:@"Value"])
		return [item value];
	
	return nil;
}

-(void)outlineView:(NSOutlineView *)outlineView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	if ([item isKindOfClass:CXRegister.class])
	{
		NSString* value = object;
		CXRegister* regObject = item;
		const char* numberString = value.UTF8String;
		char* end;
		
		if (value.length > 2 && [value characterAtIndex:0] == '0')
		{
			long result;
			switch ([value characterAtIndex:1])
			{
				case 'b': result = strtol(numberString + 2, &end, 2); break;
				case 'x': result = strtol(numberString + 2, &end, 16); break;
				default: result = strtol(numberString + 1, &end, 8); break;
			}
			
			if (*end != 0)
			{
				NSLog(@"*** trying to set %@ to invalid string %@", regObject.name, value);
				return;
			}
			regObject.value = @(result);
		}
		else
		{
			NSNumberFormatter* formatter = [[[NSNumberFormatter alloc] init] autorelease];
			formatter.numberStyle = NSNumberFormatterDecimalStyle;
			NSNumber* result = [formatter numberFromString:value];
			if (result == nil)
			{
				NSLog(@"*** trying to set %@ to invalid string %@", regObject.name, value);
				return;
			}
			regObject.value = result;
		}
	}
}

@end
