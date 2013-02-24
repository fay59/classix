//
// CXDisassembly.m
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

#import "CXDisassembly.h"
#include <set>
#include "FragmentManager.h"
#include "PEFSymbolResolver.h"
#include "FancyDisassembler.h"
#include "CXObjcDisassemblyWriter.h"

struct CXDisassemblyCXX
{
	struct Callback
	{
		id target;
		SEL selector;
		
		Callback(id target, SEL selector)
		{
			this->target = [target retain];
			this->selector = selector;
		}
		
		Callback(const Callback& that)
		{
			this->target = [that.target retain];
			this->selector = that.selector;
		}
		
		Callback(Callback&& that)
		{
			this->target = that.target;
			that.target = nil;
			this->selector = that.selector;
		}
		
		void operator()(CXDisassembly* disasm, NSString* uniqueName) const
		{
			[target performSelector:selector withObject:disasm withObject:uniqueName];
		}
		
		bool operator<(const Callback& that) const
		{
			return target < that.target;
		}
		
		~Callback()
		{
			[target release];
		}
	};
	
	std::set<Callback> callbacks;
};

static NSUInteger CXFindNextSmaller(NSArray* sortedArray, NSNumber* number, NSUInteger partitionBegin, NSUInteger size)
{
	if (size < 2)
		return partitionBegin;
	
	NSUInteger halfSize = size / 2;
	NSUInteger middle = partitionBegin + halfSize;
	NSNumber* middleNumber = [sortedArray objectAtIndex:middle];
	
	NSComparisonResult compare = [number compare:middleNumber];
	
	if (compare == NSOrderedAscending)
		return CXFindNextSmaller(sortedArray, number, partitionBegin, halfSize);
	
	if (compare == NSOrderedDescending)
		return CXFindNextSmaller(sortedArray, number, middle, size - halfSize);
	
	return middle;
}

static NSUInteger CXFindNextSmaller(NSArray* sortedArray, NSNumber* number)
{
	return CXFindNextSmaller(sortedArray, number, 0, sortedArray.count);
}

@interface CXDisassembly (Private)

-(NSString*)findBetterNameForLabel:(CXCodeLabel*)label;

@end

@implementation CXDisassembly

-(id)initWithVirtualMachine:(CXVirtualMachine*)aVm
{
	if (!(self = [super init]))
		return nil;
	
	vm = [aVm retain];
	NSMutableDictionary* uniqueNames = [NSMutableDictionary dictionary];
	NSMutableDictionary* disassemblyDictionary = [NSMutableDictionary dictionary];
	
	Common::IAllocator* allocator;
	CFM::FragmentManager* cfm;
	[vm.allocator getValue:&allocator];
	[vm.fragmentManager getValue:&cfm];
	
	for (auto iter = cfm->Begin(); iter != cfm->End(); iter++)
	{
		if (const CFM::PEFSymbolResolver* pef = dynamic_cast<const CFM::PEFSymbolResolver*>(iter->second))
		{
			PPCVM::Disassembly::FancyDisassembler disasm(allocator);
			const PEF::Container& container = pef->GetContainer();
			for (int i = 0; i < container.size(); i++)
			{
				const PEF::InstantiableSection& section = container.GetSection(i);
				PEF::SectionType type = section.GetSectionType();
				if (type != PEF::SectionType::Code && type != PEF::SectionType::ExecutableData)
					continue;
				
				CXObjCDisassemblyWriter writer(i);
				disasm.Disassemble(container, writer);
				NSArray* result = writer.GetDisassembly();
				if (result.count == 0)
					continue;
				
				for (CXCodeLabel* label in result)
				{
					[uniqueNames setObject:label.uniqueName forKey:@(label.address)];
					[disassemblyDictionary setObject:label forKey:label.uniqueName];
				}
			}
		}
	}
	
	orderedAddresses = [[[uniqueNames allKeys] sortedArrayUsingSelector:@selector(compare:)] retain];
	addressesToUniqueNames = [uniqueNames copy];
	disassembly = [disassemblyDictionary copy];
	displayNames = [[NSMutableDictionary alloc] init];
	cxx = new CXDisassemblyCXX;
	
	return self;
}

-(NSMutableDictionary*)displayNames
{
	return [[displayNames copy] autorelease];
}

-(void)setDisplayNames:(NSDictionary *)aDisplayNames
{
	NSMutableDictionary* newDisplayNames = [aDisplayNames mutableCopy];
	[displayNames release];
	displayNames = newDisplayNames;
}

-(NSArray*)functionDisassemblyForUniqueName:(NSString *)uniqueName
{
	NSNumber* firstAddress = @([self labelDisassemblyForUniqueName:uniqueName].address);
	return [self functionDisassemblyForAddress:firstAddress.unsignedIntValue];
}

-(NSArray*)functionDisassemblyForAddress:(uint32_t)address
{
	NSInteger startIndex = CXFindNextSmaller(orderedAddresses, @(address));
	while (startIndex > 0)
	{
		NSString* labelUniqueName = [addressesToUniqueNames objectForKey:[orderedAddresses objectAtIndex:startIndex]];
		CXCodeLabel* label = [self labelDisassemblyForUniqueName:labelUniqueName];
		if (label.isFunction)
			break;
		startIndex--;
	}
	
	NSUInteger lookupIndex = startIndex;
	NSMutableArray* result = [NSMutableArray array];
	NSString* labelUniqueName = [addressesToUniqueNames objectForKey:[orderedAddresses objectAtIndex:lookupIndex]];
	CXCodeLabel* label = [self labelDisassemblyForUniqueName:labelUniqueName];
	do
	{
		[result addObject:label];
		lookupIndex++;
		if (lookupIndex >= addressesToUniqueNames.count)
			break;
		
		labelUniqueName = [addressesToUniqueNames objectForKey:[orderedAddresses objectAtIndex:lookupIndex]];
		label = [self labelDisassemblyForUniqueName:labelUniqueName];
	} while (!label.isFunction);
	
	return result;
}

-(CXCodeLabel*)labelDisassemblyForUniqueName:(NSString *)uniqueName
{
	return [disassembly objectForKey:uniqueName];
}

-(CXCodeLabel*)labelDisassemblyForAddress:(uint32_t)address
{
	NSUInteger labelNumber = CXFindNextSmaller(orderedAddresses, @(address));
	NSNumber* labelAddress = [orderedAddresses objectAtIndex:labelNumber];
	NSString* uniqueName = [addressesToUniqueNames objectForKey:labelAddress];
	CXCodeLabel* label = [self labelDisassemblyForUniqueName:uniqueName];
	if (label.address <= address && label.address + label.length > address)
		return label;
	
	return nil;
}

-(NSString*)closestUniqueNameToAddress:(uint32_t)address
{
	NSUInteger index = CXFindNextSmaller(orderedAddresses, @(address));
	NSNumber* closestAddress = [orderedAddresses objectAtIndex:index];
	return [addressesToUniqueNames objectForKey:closestAddress];
}

-(NSString*)displayNameForUniqueName:(NSString *)uniqueName
{
	if (NSString* displayName = [displayNames objectForKey:uniqueName])
		return displayName;
	
	if (CXCodeLabel* label = [self labelDisassemblyForUniqueName:uniqueName])
	{
		if (NSString* displayName = [vm symbolNameOfAddress:label.address])
		{
			if ([displayName hasPrefix:@"Instantiable section"])
			{
				// that's not a very good name... can we come up with anything better?
				if (NSString* betterName = [self findBetterNameForLabel:label])
				{
					return betterName;
				}
			}
			
			return displayName;
		}
		return label.label;
	}
	
	return @"<unnamed>";
}

-(NSString*)findBetterNameForLabel:(CXCodeLabel *)label
{
	if (label.isFunction)
	{
		// is this function a stub for an import?
		// use a dirty cheap approximative to find out: stubs are usually 6 instructions and end with a bctr that
		// has metadata
		if (label.length == 6 * sizeof(PPCVM::Instruction))
		{
			NSDictionary* lastInstruction = [label.instructions objectAtIndex:5];
			id description = [lastInstruction objectForKey:@"target"];
			if (description != nil && description != NSNull.null)
			{
				uint32_t address = [description unsignedIntValue];
				if (NSString* displayName = [vm symbolNameOfAddress:address])
				{
					return [NSString stringWithFormat:@"stub for %@", displayName];
				}
			}
		}
	}
	else
	{
		// is this label an offset of a named function?
		NSInteger labelIndex = [orderedAddresses indexOfObject:@(label.address)];
		labelIndex--;
		while (labelIndex > -1)
		{
			uint32_t address = [[orderedAddresses objectAtIndex:labelIndex] unsignedIntValue];
			CXCodeLabel* previousLabel = [self labelDisassemblyForAddress:address];
			if (previousLabel.isFunction)
			{
				NSString* functionName = [self displayNameForUniqueName:previousLabel.uniqueName];
				if (![functionName hasPrefix:@"Instantiable section"])
				{
					uint32_t offset = label.address - previousLabel.address;
					return [NSString stringWithFormat:@"%@ +%u", functionName, offset];
				}
				break;
			}
			labelIndex--;
		}
	}
	
	return nil;
}

-(void)setDisplayName:(NSString *)displayName forUniqueName:(NSString *)uniqueName
{
	[displayNames setObject:displayName forKey:uniqueName];
	for (const CXDisassemblyCXX::Callback& callback : cxx->callbacks)
		callback(self, uniqueName);
}

-(void)registerNameChangeCallbackObject:(id)target selector:(SEL)selector
{
	cxx->callbacks.emplace(target, selector);
}

-(void)unregisterNameChangeCallbackObject:(id)target
{
	cxx->callbacks.erase(CXDisassemblyCXX::Callback(target, NULL));
}

-(void)dealloc
{
	self.displayNames = nil;
	[addressesToUniqueNames release];
	[disassembly release];
	[orderedAddresses release];
	[vm release];
	delete cxx;
	
	[super dealloc];
}

@end
