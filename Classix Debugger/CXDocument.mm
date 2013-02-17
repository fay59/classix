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

static NSString* CXExecutableUTI = @"public.executable";
static NSString* CXDebugDocumentUTI = @"com.felixcloutier.classix.document";

@synthesize vm;
@synthesize disassembly;
@synthesize arguments;
@synthesize environment;
@synthesize debug = debugUIController;
@synthesize entryPoints;
@synthesize executableURL;

-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	arguments = [[NSMutableArray alloc] init];
	environment = [[NSMutableDictionary alloc] init];
	
	return self;
}

-(NSString *)windowNibName
{
	return @"CXDocument";
}

-(void)windowControllerDidLoadNib:(NSWindowController *)aController
{
	[super windowControllerDidLoadNib:aController];
	[entryPoints removeAllItems];
	
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
			if (*fullPath == executableURL.path.UTF8String)
			{
				std::vector<ResolvedSymbol> executableEntryPoints = resolver->GetEntryPoints();
				for (const ResolvedSymbol& entryPoint : executableEntryPoints)
				{
					NSString* itemName = [NSString stringWithCString:entryPoint.Name.c_str() encoding:NSUTF8StringEncoding];
					NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:itemName action:NULL keyEquivalent:@""];
					item.tag = entryPoint.Address;
					[[entryPoints menu] addItem:item];
				}
			}
		}
	}
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
	
	if (UTTypeConformsTo((CFStringRef)typeName, (CFStringRef)CXDebugDocumentUTI))
	{
		return [self readDebugDocumentFromURL:url error:outError];
	}
	else if (UTTypeConformsTo((CFStringRef)typeName, (CFStringRef)CXExecutableUTI))
	{
		return [self readExecutableFromURL:url error:outError];
	}
	
	return NO;
}

-(BOOL)writeToURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError
{
	NSData* bookmark = [executableURL bookmarkDataWithOptions:NSURLBookmarkCreationPreferFileIDResolution includingResourceValuesForKeys:nil relativeToURL:nil error:outError];
	if (bookmark == nil)
	{
		return NO;
	}
	
	NSMutableArray* saveArguments = [arguments mutableCopy];
	[saveArguments removeObjectAtIndex:0];
	
	NSDictionary* saveInfo = @{
		@"executable": bookmark,
		@"argv": saveArguments,
		@"envp": environment,
		@"labels": disassembly.displayNames
	};
	
	return [saveInfo writeToURL:url atomically:YES];
}

-(void)dealloc
{
	[vm release];
	[disassembly release];
	
	[debugUIController release];
	[executableURL release];
	[arguments release];
	[environment release];
	
	[super dealloc];
}

#pragma mark -
#pragma mark Non-overrides

-(BOOL)readDebugDocumentFromURL:(NSURL *)url error:(NSError **)error
{
	NSDictionary* savedSettings = [[[NSDictionary alloc] initWithContentsOfURL:url] autorelease];
	if (savedSettings != nil)
	{
		NSData* bookmark = [savedSettings objectForKey:@"executable"];
		NSURL* savedURL = [NSURL URLByResolvingBookmarkData:bookmark options:NSURLBookmarkResolutionWithoutUI relativeToURL:nil bookmarkDataIsStale:NULL error:error];
		if (savedURL != nil)
		{
			if ([self useExecutableURL:savedURL error:error])
			{
				[environment release];
				environment = [[savedSettings objectForKey:@"envp"] mutableCopy];
				
				[arguments removeAllObjects];
				[arguments addObject:executableURL.path];
				[arguments addObjectsFromArray:[savedSettings objectForKey:@"argv"]];
				
				disassembly.displayNames = [savedSettings objectForKey:@"labels"];
				return YES;
			}
		}
	}
	
	return NO;
}

-(BOOL)readExecutableFromURL:(NSURL *)url error:(NSError **)error
{
	if ([self useExecutableURL:url error:error])
	{
		self.fileURL = nil;
		self.displayName = url.lastPathComponent;
		self.fileType = CXDebugDocumentUTI;
		[self updateChangeCount:1];
		return YES;
	}
	
	return NO;
}

-(BOOL)useExecutableURL:(NSURL *)path error:(NSError **)error
{
	executableURL = [path copy];
	vm = [[CXVirtualMachine alloc] init];
	if ([vm loadClassicExecutable:executableURL.path error:error])
	{
		disassembly = [[CXDisassembly alloc] initWithVirtualMachine:vm];
		arguments = [[NSMutableArray alloc] initWithObjects:executableURL.path, nil];
		return YES;
	}
	return NO;
}

-(IBAction)start:(id)sender
{
	if (debugUIController == nil)
	{
		// FIXME ugly
		[vm setArgv:arguments envp:environment];
		
		uint32_t transitionAddress = [entryPoints selectedItem].tag;
		[vm transitionByAddress:transitionAddress];
		
		debugUIController = [[CXDebugUIController alloc] initWithDocument:self];
		[debugUIController instantiate];
		debugUIController.windowController.document = self;
	}
	else
	{
		[debugUIController orderFront];
	}
}

@end
