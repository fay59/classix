//
//  CXDisassembly.m
//  Classix
//
//  Created by Félix on 2012-12-04.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "CXDisassembly.h"
#include "FragmentManager.h"
#include "PEFSymbolResolver.h"
#include "FancyDisassembler.h"
#include "CXObjcDisassemblyWriter.h"

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
			return displayName;
		return label.label;
	}
	
	return @"<unnamed>";
}

-(void)setDisplayName:(NSString *)displayName forUniqueName:(NSString *)uniqueName
{
	[displayNames setObject:displayName forKey:uniqueName];
}

-(void)dealloc
{
	self.displayNames = nil;
	[addressesToUniqueNames release];
	[disassembly release];
	[orderedAddresses release];
	[vm release];
	[super dealloc];
}

@end
