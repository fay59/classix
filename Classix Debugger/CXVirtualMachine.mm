//
// CXVirtualMachine.mm
// Classix
//
// Copyright (C) 2012 Félix Cloutier
//
// This file is part of Classix.
//
// Classix is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Classix is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Classix. If not, see http://www.gnu.org/licenses/.
//

#import "CXVirtualMachine.h"
#import "CXRegister.h"

#include <dlfcn.h>
#include <unordered_set>

#include "MachineState.h"
#include "FragmentManager.h"
#include "NativeAllocator.h"
#include "PEFLibraryResolver.h"
#include "Interpreter.h"
#include "DlfcnLibraryResolver.h"
#include "FancyDisassembler.h"
#include "CXObjcDisassemblyWriter.h"
#include "NativeCall.h"
#include "CXReverseAllocationDetails.h"
#include "InstructionDecoder.h"

NSNumber* CXVirtualMachineGPRKey = @(CXRegisterGPR);
NSNumber* CXVirtualMachineFPRKey = @(CXRegisterFPR);
NSNumber* CXVirtualMachineSPRKey = @(CXRegisterSPR);
NSNumber* CXVirtualMachineCRKey = @(CXRegisterCR);

NSString* CXErrorDomain = @"Classix Error Domain";
NSString* CXErrorFilePath = @"File URL";

const NSUInteger CXStackSize = 0x100000;

struct ClassixCoreVM
{
	Common::IAllocator* allocator;
	PPCVM::MachineState state;
	CFM::FragmentManager cfm;
	CFM::PEFLibraryResolver pefResolver;
	ClassixCore::DlfcnLibraryResolver dlfcnResolver;
	PPCVM::Execution::Interpreter interp;
	Common::AutoAllocation stack;
	
	std::unordered_set<intptr_t> breakpoints;
	
	ClassixCoreVM(Common::IAllocator* allocator)
	: allocator(allocator)
	, state()
	, cfm()
	, pefResolver(allocator, cfm)
	, dlfcnResolver(allocator)
	, interp(allocator, &state)
	, stack(allocator->AllocateAuto(CXReverseAllocationDetails("Stack", CXStackSize), CXStackSize))
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
			std::vector<CFM::ResolvedSymbol> entryPoints = resolver->GetEntryPoints();
			for (const CFM::ResolvedSymbol& symbol : entryPoints)
			{
				if (symbol.Universe == CFM::SymbolUniverse::PowerPC)
					return true;
			}
		}
		return false;
	}
	
	bool IsCodeAddress(uint32_t address)
	{
		for (auto iter = cfm.Begin(); iter != cfm.End(); iter++)
		{
			if (const CFM::PEFSymbolResolver* resolver = dynamic_cast<const CFM::PEFSymbolResolver*>(iter->second))
			{
				const PEF::Container& container = resolver->GetContainer();
				for (const PEF::InstantiableSection& section : container)
				{
					if (section.IsExecutable())
					{
						uint32_t sectionBase = allocator->ToIntPtr(section.Data);
						uint32_t sectionEnd = sectionBase + section.Size();
						if (address >= sectionBase && address < sectionEnd)
							return true;
					}
				}
			}
		}
		return false;
	}
};

@interface CXVirtualMachine (Private)

-(void)refreshRegisters:(const PPCVM::MachineState*)oldState;

@end

@implementation CXVirtualMachine

@synthesize allRegisters = registers;
@synthesize breakpoints;
@synthesize pc;
@synthesize lastError;

-(void)setLastError:(NSString *)aLastError
{
	NSString* copy = [aLastError copy];
	[self willChangeValueForKey:@"lastError"];
	[lastError release];
	lastError = copy;
	[self didChangeValueForKey:@"lastError"];
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

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	vm = new ClassixCoreVM(Common::NativeAllocator::GetInstance());
	
	NSMutableArray* gpr = [NSMutableArray arrayWithCapacity:32];
	NSMutableArray* fpr = [NSMutableArray arrayWithCapacity:32];
	NSMutableArray* cr = [NSMutableArray arrayWithCapacity:8];
	NSMutableArray* spr = [NSMutableArray array];
	breakpoints = [[NSMutableSet alloc] init];
	changedRegisters = [[NSMutableSet alloc] initWithCapacity:75];
	
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

-(void)setArgv:(NSArray *)args envp:(NSDictionary *)env
{
	// set argv and envp
	std::vector<size_t> argvOffsets;
	std::vector<size_t> envpOffsets;
	std::vector<char> argvStrings;
	std::vector<char> envpStrings;
	
	for (NSString* arg in args)
	{
		const char* begin = arg.UTF8String;
		const char* end = begin + strlen(begin) + 1;
		argvOffsets.push_back(argvStrings.size());
		argvStrings.insert(argvStrings.end(), begin, end);
	}
	
	for (NSString* key in env)
	{
		const char* keyBegin = key.UTF8String;
		const char* keyEnd = keyBegin + strlen(keyBegin);
		envpOffsets.push_back(envpStrings.size());
		envpStrings.insert(envpStrings.end(), keyBegin, keyEnd);
		envpStrings.push_back('=');
		
		NSString* value = [env objectForKey:key];
		const char* valueBegin = value.UTF8String;
		const char* valueEnd = valueBegin + strlen(valueBegin) + 1;
		envpStrings.insert(envpStrings.end(), valueBegin, valueEnd);
	}
	
	Common::AutoAllocation& stack = vm->stack;
	memset(*stack, 0, CXStackSize);
	
	//  stack layout:
	// +-------------+
	// | string area |
	// +-------------+
	// |      0      |
	// +-------------+
	// |    env[n]   |
	// +-------------+
	//        :
	// +-------------+
	// |    env[0]   |
	// +-------------+
	// |      0      |
	// +-------------+
	// | arg[argc-1] |
	// +-------------+
	//        :
	// +-------------+
	// |    arg[0]   |
	// +-------------+
	// |     argc    | <-- sp
	// +-------------+
	
	const uint32_t stackAddress = stack.GetVirtualAddress();
	const NSUInteger stringAreaOffset = CXStackSize - envpStrings.size() - argvStrings.size();
	char* stringArea = static_cast<char*>(*stack) + stringAreaOffset;
	memcpy(stringArea, argvStrings.data(), argvStrings.size());
	memcpy(stringArea + argvStrings.size(), envpStrings.data(), envpStrings.size());
	
	const NSUInteger envOffset = (stringAreaOffset - (envpOffsets.size() + 1) * sizeof(uint32_t)) / 4;
	Common::UInt32* envArea = static_cast<Common::UInt32*>(*stack) + envOffset;
	for (size_t i = 0; i < envpOffsets.size(); i++)
		envArea[i] = stackAddress + stringAreaOffset + envpOffsets[i];
	
	const NSUInteger argvOffset = envOffset - argvOffsets.size() - 1;
	Common::UInt32* argvArea = static_cast<Common::UInt32*>(*stack) + argvOffset;
	for (size_t i = 0; i < argvOffsets.size(); i++)
		argvArea[i] = stackAddress + stringAreaOffset + envpStrings.size() + argvOffsets[i];
	
	Common::UInt32& argc = *(static_cast<Common::UInt32*>(*stack) + argvOffset - 1);
	argc = argvOffsets.size();
	
	// set the stage
	vm->state.r0 = 0;
	vm->state.r1 = vm->allocator->ToIntPtr(&argc);
	vm->state.r3 = args.count;
	vm->state.r4 = vm->allocator->ToIntPtr(argvArea);
	vm->state.r5 = vm->allocator->ToIntPtr(envArea);
	
	NSArray* gpr = self.gpr;
	NSArray* initialRegisters = @[
		[gpr objectAtIndex:1],
		[gpr objectAtIndex:2],
		[gpr objectAtIndex:3],
		[gpr objectAtIndex:4],
		[gpr objectAtIndex:5],
	];
	
	[changedRegisters removeAllObjects];
	[changedRegisters addObjectsFromArray:initialRegisters];
}

-(void)transitionByAddress:(uint32_t)address
{
	const PEF::TransitionVector* transition = vm->allocator->ToPointer<PEF::TransitionVector>(address);
	vm->state.r2 = transition->TableOfContents;
	pc = transition->EntryPoint;
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

-(NSString*)symbolNameOfAddress:(unsigned int)address
{
	using namespace PPCVM::Execution;
	Common::IAllocator* allocator = vm->allocator;
	try
	{
		const NativeCall* pointer = allocator->ToPointer<NativeCall>(address);
		if (pointer->Tag == NativeTag)
		{
			Dl_info info;
			const void* symbolAddress = reinterpret_cast<const void*>(pointer->Callback);
			if (dladdr(symbolAddress, &info))
			{
				return [NSString stringWithCString:info.dli_sname encoding:NSUTF8StringEncoding];
			}
		}
	}
	catch (Common::PPCRuntimeException& ex)
	{ }
	return nil;
}

-(NSString*)explainAddress:(unsigned)address
{
	if (const Common::AllocationDetails* details = vm->allocator->GetDetails(address))
	{
		uint32_t offset = vm->allocator->GetAllocationOffset(address);
		std::string description = details->GetAllocationDetails(offset);
		return [NSString stringWithCString:description.c_str() encoding:NSUTF8StringEncoding];
	}
	return nil;
}

-(NSNumber*)wordAtAddress:(unsigned int)address
{
	try
	{
		const Common::UInt32* atAddress = vm->allocator->ToPointer<const Common::UInt32>(address);
		return @(atAddress->Get());
	}
	catch (Common::AccessViolationException&)
	{
		return nil;
	}
}

-(NSNumber*)floatAtAddress:(unsigned int)address
{
	try
	{
		const Common::Real32* atAddress = vm->allocator->ToPointer<const Common::Real32>(address);
		return @(atAddress->Get());
	}
	catch (Common::AccessViolationException&)
	{
		return nil;
	}
}

-(NSNumber*)doubleAtAddress:(unsigned int)address
{
	try
	{
		const Common::Real64* atAddress = vm->allocator->ToPointer<const Common::Real64>(address);
		return @(atAddress->Get());
	}
	catch (Common::AccessViolationException&)
	{
		return nil;
	}
}

-(NSArray*)stackTrace
{
	using Common::UInt32;
	
	NSMutableArray* stackFrames = [NSMutableArray array];
	// first frame: here
	[stackFrames addObject:@(pc)];
	
	// this relies on the fact that the stack is allocated on a 4-byte boundary
	uint32_t stackWord = vm->state.r1 & ~0b11;
	const UInt32* stackGuard = static_cast<UInt32*>(*vm->stack) + CXStackSize / sizeof (UInt32);
	uint32_t stackEnd = vm->allocator->ToIntPtr(stackGuard);
	uint32_t stackWordCount = (stackEnd - stackWord) / sizeof (UInt32);
	
	const UInt32* stackPointer;
	
	try { stackPointer = vm->allocator->ToArray<const UInt32>(stackWord, stackWordCount); }
	catch (Common::AccessViolationException&) { return nil; }
	
	for (; stackPointer != stackGuard; stackPointer++)
	{
		uint32_t hopefullyBranchAndLinkAddress = *stackPointer - 4;
		if (vm->IsCodeAddress(hopefullyBranchAndLinkAddress))
		{
			const UInt32* hopefullyBranchAndLink = vm->allocator->ToPointer<const UInt32>(hopefullyBranchAndLinkAddress);
			PPCVM::Instruction inst = hopefullyBranchAndLink->Get();
			int opcd = inst.OPCD;
			int subop = inst.SUBOP10;
			bool isBranch = opcd == 16 || opcd == 18 || (opcd == 19 && (subop == 16 || subop == 528));
			bool isLink = inst.LK;
			if (isBranch && isLink)
			{
				// alright, that makes "enough sense" to be added
				uint32_t address = stackPointer->Get();
				[stackFrames addObject:@(address)];
			}
		}
	}
	
	return stackFrames;
}

-(IBAction)run:(id)sender
{
	std::unordered_set<const void*> cppBreakpoints;
	for (NSNumber* number in breakpoints)
	{
		const void* address = vm->allocator->ToPointer<const void>(number.unsignedIntValue);
		cppBreakpoints.insert(address);
	}
	
	const void* eip = vm->allocator->ToPointer<void>(pc);
	PPCVM::MachineState oldState = vm->state;
	
	try
	{
		eip = vm->interp.ExecuteUntil(eip, cppBreakpoints);
		self.pc = vm->allocator->ToIntPtr(const_cast<void*>(eip));
		self.lastError = nil;
	}
	catch (PPCVM::Execution::InterpreterException& ex)
	{
		self.pc = ex.GetPC();
		self.lastError = [NSString stringWithCString:ex.what() encoding:NSUTF8StringEncoding];
	}
	
	[self refreshRegisters:&oldState];
}

-(IBAction)stepOver:(id)sender
{
	NSLog(@"*** step over is not currently supported; stepping into instead");
	[self stepInto:sender];
}

-(IBAction)stepInto:(id)sender
{
	using namespace PPCVM::Execution;
	
	PPCVM::MachineState oldState = vm->state;
	const void* eip = vm->allocator->ToPointer<const void>(pc);
	try
	{
		eip = vm->interp.ExecuteOne(eip);
		self.pc = vm->allocator->ToIntPtr(const_cast<void*>(eip));
		self.lastError = nil;
	}
	catch (PPCVM::Execution::InterpreterException& ex)
	{
		self.pc = ex.GetPC();
		self.lastError = [NSString stringWithCString:ex.what() encoding:NSUTF8StringEncoding];
	}
	
	[self refreshRegisters:&oldState];
	
	// don't stop inside a native call
	const NativeCall* nativeCall = reinterpret_cast<const NativeCall*>(eip);
	if (nativeCall->Tag == NativeTag)
		[self stepOver:sender];
}

-(void)runTo:(uint32_t)location
{
	std::unordered_set<const void*> until = {vm->allocator->ToPointer<const void>(location)};
	const void* eip = vm->allocator->ToPointer<const void>(pc);
	PPCVM::MachineState oldState = vm->state;
	try
	{
		eip = vm->interp.ExecuteUntil(eip, until);
		self.pc = vm->allocator->ToIntPtr(const_cast<void*>(eip));
		self.lastError = nil;
	}
	catch (PPCVM::Execution::InterpreterException& ex)
	{
		self.pc = ex.GetPC();
		self.lastError = [NSString stringWithCString:ex.what() encoding:NSUTF8StringEncoding];
	}
	
	[self refreshRegisters:&oldState];
}

-(void)dealloc
{
	delete vm;
	[registers release];
	[breakpoints release];
	[changedRegisters release];
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

-(void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	if ([changedRegisters containsObject:item])
	{
		[cell setTextColor:NSColor.redColor];
	}
	else
	{
		[cell setTextColor:NSColor.blackColor];
	}
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

#pragma mark -
#pragma mark Private
-(void)refreshRegisters:(const PPCVM::MachineState *)oldState
{
	[changedRegisters removeAllObjects];
	// notify observers for value changes
	for (int i = 0; i < 8; i++)
	{
		if (oldState->cr[i] != vm->state.cr[i])
		{
			CXRegister* cr = [self.cr objectAtIndex:i];
			cr.value = @(vm->state.cr[i]);
			[changedRegisters addObject:cr];
		}
	}
	
	for (int i = 0; i < 32; i++)
	{
		if (oldState->gpr[i] != vm->state.gpr[i])
		{
			CXRegister* gpr = [self.gpr objectAtIndex:i];
			gpr.value = @(vm->state.gpr[i]);
			[changedRegisters addObject:gpr];
		}
		
		if (oldState->fpr[i] != vm->state.fpr[i])
		{
			CXRegister* fpr = [self.fpr objectAtIndex:i];
			fpr.value = @(vm->state.fpr[i]);
			[changedRegisters addObject:fpr];
		}
	}
	
	if (oldState->xer != vm->state.xer)
	{
		CXRegister* xer = [self.spr objectAtIndex:CXVirtualMachineSPRXERIndex];
		xer.value = @(vm->state.xer);
		[changedRegisters addObject:xer];
	}
	
	if (oldState->ctr != vm->state.ctr)
	{
		CXRegister* ctr = [self.spr objectAtIndex:CXVirtualMachineSPRCTRIndex];
		ctr.value = @(vm->state.ctr);
		[changedRegisters addObject:ctr];
	}
	
	if (oldState->lr != vm->state.lr)
	{
		CXRegister* lr = [self.spr objectAtIndex:CXVirtualMachineSPRLRIndex];
		lr.value = @(vm->state.lr);
		[changedRegisters addObject:lr];
	}
}

@end
