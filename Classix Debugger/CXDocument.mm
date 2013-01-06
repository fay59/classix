//
// CXDocument.mm
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

#import "CXDocument.h"
#include "IAllocator.h"
#include "PEFSymbolResolver.h"



@implementation CXDocument

@synthesize vm;
@synthesize disassembly;
@synthesize arguments;
@synthesize environment;
@synthesize executablePath;
@synthesize debug = debugUIController;

-(NSString *)windowNibName
{
	return @"CXDocument";
}

-(void)windowControllerDidLoadNib:(NSWindowController *)aController
{
	[super windowControllerDidLoadNib:aController];
}

+(BOOL)autosavesInPlace
{
    return NO;
}

-(BOOL)readFromURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError
{
	if (!url.isFileURL)
	{
		if (outError != NULL)
			*outError = [NSError errorWithDomain:CXErrorDomain code:CXErrorCodeNotLocalURL userInfo:nil];
		return NO;
	}
	
	vm = [[CXVirtualMachine alloc] init];
	executablePath = [url.path retain];
	if ([vm loadClassicExecutable:executablePath error:outError])
	{
		disassembly = [[CXDisassembly alloc] initWithVirtualMachine:vm];
		return YES;
	}
	
	return NO;
}

-(uint32_t)addressOfEntryPoint:(int)entryPointIndex
{
	NSParameterAssert(entryPointIndex >= 0 && entryPointIndex < 3);
	
	using namespace CFM;
	using namespace PEF;
	
	IAllocator* allocator;
	FragmentManager* cfm;
	
	[vm.fragmentManager getValue:&cfm];
	[vm.allocator getValue:&allocator];
	
	for (auto iter = cfm->Begin(); iter != cfm->End(); iter++)
	{
		const SymbolResolver* resolver = iter->second;
		if (const std::string* fullPath = resolver->FilePath())
		{
			if (*fullPath == executablePath.UTF8String)
			{
				if (const PEFSymbolResolver* pefResolver = dynamic_cast<const PEFSymbolResolver*>(resolver))
				{
					const Container& container = pefResolver->GetContainer();
					const LoaderHeader::SectionWithOffset& sectionWithOffset = container.LoaderSection()->Header->EntryPoints[entryPointIndex];
					if (sectionWithOffset.Section == -1)
						return -1;
					
					const InstantiableSection& section = container.GetSection(sectionWithOffset.Section);
					return allocator->ToIntPtr(section.Data) + sectionWithOffset.Offset;
				}
			}
		}
	}
	return -1;
}

-(void)dealloc
{
	[debugUIController release];
	[executablePath release];
	[super dealloc];
}

#pragma mark -
#pragma mark UI Methods

-(IBAction)start:(id)sender
{
	if (debugUIController == nil)
	{
		// FIXME ugly
		[vm setArgv:@[executablePath] envp:@{}];
		[vm transitionByAddress:[self addressOfEntryPoint:0]];
		
		debugUIController = [[CXDebugUIController alloc] initWithDocument:self];
		[debugUIController instantiate];
	}
}

@end
