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

#include "MachineState.h"
#include "FragmentManager.h"
#include "NativeAllocator.h"
#include "PEFLibraryResolver.h"
#include "Interpreter.h"
#include "DyldLibraryResolver.h"
#include "FancyDisassembler.h"
#include "CXObjcDisassemblyWriter.h"

#import "CXJSDebug.h"
#import "CXDocument.h"
#import "CXDocumentController.h"

// TODO this is ugly and not thread-safe, but a pain to fix in 32-bits assembly
// With 64-bits x86, we have enough registers to save jumpTarget into some random
// register and then access the address throug ctx (which really is a ucontext pointer).
sigjmp_buf jumpTarget;
static void onSigSegv(int signal, struct __siginfo*, void* ctx)
{
	siglongjmp(jumpTarget, 1);
}

NSString* CXErrorDomain = @"Classix Error Domain";
NSString* CXErrorFileURL = @"File URL";

using namespace PEF;

struct ClassixCoreVM
{
	Common::IAllocator* allocator;
	PPCVM::MachineState state;
	CFM::FragmentManager cfm;
	CFM::PEFLibraryResolver pefResolver;
	ClassixCore::DyldLibraryResolver dyldResolver;
	PPCVM::Execution::Interpreter interp;
	PEF::Container* container;
	
	intptr_t nextPC;
	
	ClassixCoreVM(Common::IAllocator* allocator)
	: allocator(allocator)
	, state()
	, cfm()
	, pefResolver(allocator, cfm)
	, dyldResolver(allocator)
	, interp(allocator, &state)
	, container(nullptr)
	, nextPC(0)
	{
		dyldResolver.RegisterLibrary("StdCLib");
		cfm.LibraryResolvers.push_back(&pefResolver);
		cfm.LibraryResolvers.push_back(&dyldResolver);
	}
	
	bool LoadContainer(const std::string& path)
	{
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

@implementation CXDocument

@synthesize disassemblyView;

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
	if (!vm->LoadContainer(path))
	{
		if (outError != nullptr)
			*outError = [NSError errorWithDomain:CXErrorDomain code:CXErrorCodeFileNotLoadable userInfo:@{CXErrorFileURL: url}];
		return NO;
	}
	
	dispatch_async(dispatch_get_main_queue(), ^{
		// great, let's load the XHTML document into the WebView
		NSString* cxdbPath = [NSBundle.mainBundle pathForResource:@"cxdb" ofType:@"xhtml"];
		NSMutableString* xhtml = [NSMutableString stringWithContentsOfFile:cxdbPath encoding:NSUTF8StringEncoding error:NULL];
		if (xhtml == nil)
			xhtml = [NSMutableString stringWithString:@"<!DOCTYPE html><html><body><p>There was an error loading the disassembler user interface.</p></body></html>"];
		
		unsigned docId = (unsigned)[[CXDocumentController documentController] idOfDocument:self];
		NSString* documentId = [NSString stringWithFormat:@"%u", docId];
		NSRange fullRange = NSMakeRange(0, xhtml.length);
		[xhtml replaceOccurrencesOfString:@"##data-document-id##" withString:documentId options:NSLiteralSearch range:fullRange];
		NSData* data = [xhtml dataUsingEncoding:NSUTF8StringEncoding];
		
		NSURL* rootUrl = [NSURL URLWithString:@"cxdb:disassembly"];
		[[disassemblyView mainFrame] loadData:data MIMEType:@"application/xhtml+xml" textEncodingName:@"UTF-8" baseURL:rootUrl];
	});
	
	return YES;
}

-(id)executeCommand:(NSString *)aCommand arguments:(NSArray *)arguments
{
	std::string command = aCommand.UTF8String;
	if (command == "sections")
	{
		if (arguments.count == 0)
		{
			NSUInteger sectionCount = vm->container->Size();
			NSMutableArray* result = [NSMutableArray arrayWithCapacity:sectionCount];
			for (NSUInteger i = 0; i < sectionCount; i++)
			{
				const char* name = vm->container->GetSection(i).Name.c_str();
				NSString* sectionName = [NSString stringWithCString:name encoding:NSUTF8StringEncoding];
				[result addObject:sectionName];
			}
			return result;
		}
		else if (arguments.count == 1)
		{
			NSUInteger section = [[arguments objectAtIndex:0] integerValue];
			CXObjCDisassemblyWriter writer(section);
			PPCVM::Disassembly::FancyDisassembler(vm->allocator).Disassemble(*vm->container, writer);
			return writer.GetDisassembly();
		}
		else
		{
			return nil;
		}
	}
	else if (command == "status")
	{
		if (arguments.count != 0) return nil;
		return @{@"section": @(vm->SectionForPC()), @"pc": @(vm->nextPC)};
	}
	else if (command == "gpr")
	{
		if (arguments.count != 1) return nil;
		int reg = [[arguments objectAtIndex:0] intValue];
		if (reg < 0 || reg > 31) return nil;
		return @(vm->state.gpr[reg]);
	}
	else if (command == "fpr")
	{
		if (arguments.count != 1) return nil;
		int reg = [[arguments objectAtIndex:0] intValue];
		if (reg < 0 || reg > 31) return nil;
		return @(vm->state.fpr[reg]);
	}
	else if (command == "spr")
	{
		if (arguments.count != 1) return nil;
		int reg = [[arguments objectAtIndex:0] intValue];
		switch (reg)
		{
			case 1: return @(vm->state.xer);
			case 8: return @(vm->state.lr);
			case 9: return @(vm->state.ctr);
			default: return nil;
		}
	}
	else if (command == "cr")
	{
		if (arguments.count != 1) return nil;
		int reg = [[arguments objectAtIndex:0] intValue];
		if (reg < 0 || reg > 7) return nil;
		return @(vm->state.cr[reg]);
	}
	else if (command == "memory")
	{
		if (arguments.count != 2) return nil;
		
		// this one is...ugly
		intptr_t begin = [[arguments objectAtIndex:0] integerValue];
		intptr_t count = [[arguments objectAtIndex:1] integerValue];
		uint8_t* bytes = vm->allocator->ToPointer<uint8_t>(begin);
		
		// check that the range is readable
		if (sigsetjmp(jumpTarget, 1) == 0)
		{
			struct sigaction segvAction, oldAction;
			segvAction.sa_flags = SA_RESETHAND;
			segvAction.sa_sigaction = onSigSegv;
			sigemptyset(&segvAction.sa_mask);
			sigaddset(&segvAction.sa_mask, SIGINT);
			sigaddset(&segvAction.sa_mask, SIGALRM);
			sigaddset(&segvAction.sa_mask, SIGTERM);
			sigaction(SIGSEGV, &segvAction, &oldAction);
			
			// (volatile ensures the compiler won't optimize away the reads as trivial)
			volatile uint8_t readTarget;
			for (size_t i = 0; i < count; i++)
				readTarget = bytes[i];
			
			sigaction(SIGSEGV, &oldAction, NULL);
			return [NSData dataWithBytes:bytes length:count];
		}
		return nil;
	}
	return nil;
}

-(void)webView:(WebView*)sender didFinishLoadForFrame:(WebFrame *)frame
{
	CXJSDebug* jsDebug = [[[CXJSDebug alloc] init] autorelease];
	[[disassemblyView windowScriptObject] setValue:jsDebug forKey:@"debug"];
}

-(void)dealloc
{
	[execData release];
	delete vm;
	[super dealloc];
}

@end
