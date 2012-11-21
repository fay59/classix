//
//  CXDocument.m
//  Classix Debugger
//
//  Created by Félix on 2012-11-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "MachineState.h"
#include "FragmentManager.h"
#include "NativeAllocator.h"
#include "PEFLibraryResolver.h"
#include "Interpreter.h"
#include "DyldLibraryResolver.h"

#import "CXDocument.h"

NSString* CXErrorDomain = @"Classix Error Domain";
NSString* CXErrorFileURL = @"File URL";

struct ClassixCoreVM
{
	Common::IAllocator* allocator;
	PPCVM::MachineState state;
	CFM::FragmentManager cfm;
	CFM::PEFLibraryResolver pefResolver;
	ClassixCore::DyldLibraryResolver dyldResolver;
	PPCVM::Execution::Interpreter interp;
	
	ClassixCoreVM(Common::IAllocator* allocator)
	: allocator(allocator)
	, state()
	, cfm()
	, pefResolver(allocator, cfm)
	, dyldResolver(allocator)
	, interp(allocator, &state)
	{
		dyldResolver.RegisterLibrary("StdCLib");
		cfm.LibraryResolvers.push_back(&pefResolver);
		cfm.LibraryResolvers.push_back(&dyldResolver);
	}
};

@implementation CXDocument

-(id)init
{
    self = [super init];
    if (self) {
		vm = new ClassixCoreVM(Common::NativeAllocator::Instance);
    }
    return self;
}

-(NSString *)windowNibName
{
	// Override returning the nib file name of the document
	// If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
	return @"CXDocument";
}

-(void)windowControllerDidLoadNib:(NSWindowController *)aController
{
	[super windowControllerDidLoadNib:aController];
	// Add any code here that needs to be executed once the windowController has loaded the document's window.
}

+(BOOL)autosavesInPlace
{
    return NO;
}

-(BOOL)readFromURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError
{
	if (!url.isFileURL)
	{
		if (outError != nullptr)
			*outError = [NSError errorWithDomain:CXErrorDomain code:CXErrorCodeNotLocalURL userInfo:@{CXErrorFileURL: url}];
		return NO;
	}
	
	const char* path = url.path.UTF8String;
	if (!vm->cfm.LoadContainer(path))
	{
		if (outError != nullptr)
			*outError = [NSError errorWithDomain:CXErrorDomain code:CXErrorCodeFileNotLoadable userInfo:@{CXErrorFileURL: url}];
		return NO;
	}
	
	return YES;
}

-(void)dealloc
{
	[execData release];
	delete vm;
	[super dealloc];
}

@end
