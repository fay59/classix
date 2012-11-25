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
#include "DlfcnLibraryResolver.h"
#include "FancyDisassembler.h"
#include "CXObjcDisassemblyWriter.h"
#include <unordered_set>

#import "CXJSDebug.h"
#import "CXDocument.h"
#import "CXDocumentController.h"
#import "CXJSONEncode.h"

NSString* CXErrorDomain = @"Classix Error Domain";
NSString* CXErrorFileURL = @"File URL";

static NSImage* exportImage;
static NSImage* labelImage;
static NSImage* functionImage;

using namespace PEF;

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

@interface CXDocument (Private)

-(void)buildSymbolMenu;
-(void)showDisassemblyPage;
-(void)showDisassembly:(NSString*)key;
-(NSImage*)fileIcon16x16:(NSString*)path;
-(NSMenu*)exportMenuForResolver:(const CFM::SymbolResolver*)resolver;

@end

@implementation CXDocument

@synthesize disassemblyView;
@synthesize navBar;

+(void)initialize
{
	NSString* exportPath = [[NSBundle mainBundle] pathForResource:@"export" ofType:@"png"];
	NSString* labelPath = [[NSBundle mainBundle] pathForResource:@"label" ofType:@"png"];
	NSString* functionPath = [[NSBundle mainBundle] pathForResource:@"function" ofType:@"png"];
	exportImage = [[NSImage alloc] initWithContentsOfFile:exportPath];
	labelImage = [[NSImage alloc] initWithContentsOfFile:labelPath];
	functionImage = [[NSImage alloc] initWithContentsOfFile:functionPath];
}

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
	
	return YES;
}

-(void)webView:(WebView*)sender didFinishLoadForFrame:(WebFrame *)frame
{
	CXJSDebug* jsDebug = [[[CXJSDebug alloc] init] autorelease];
	[[disassemblyView windowScriptObject] setValue:jsDebug forKey:@"debug"];
	[self buildSymbolMenu];
}

-(void)awakeFromNib
{
	[self showDisassemblyPage];
}

-(void)dealloc
{
	delete vm;
	[super dealloc];
}

#pragma mark -

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
		
		intptr_t begin = [[arguments objectAtIndex:0] integerValue];
		intptr_t count = [[arguments objectAtIndex:1] integerValue];
		
		// check that the range was allocated
		// this is a cheap, inaccurate check because it only verifies that the two ends of the memory range lie inside
		// any allocated memory range, and not even necessarily the same, but in practice it should be "good enough".
		intptr_t end = begin + count;
		
		if (vm->allocator->IsAllocated(begin) && vm->allocator->IsAllocated(end))
		{
			uint8_t* bytes = vm->allocator->ToPointer<uint8_t>(begin);
			return [NSData dataWithBytes:bytes length:count];
		}
		
		return nil;
	}
	return nil;
}

-(void)buildSymbolMenu
{
	using namespace ClassixCore;
	using namespace CFM;
	
	// build the menus with the current resolvers
	NSMenu* resolverMenu = [[[NSMenu alloc] initWithTitle:@"Debugger"] autorelease];
	
	for (auto iter = vm->cfm.Begin(); iter != vm->cfm.End(); iter++)
	{
		const SymbolResolver* resolver = iter->second;
		std::string name;
		if (const std::string* fullPath = resolver->FilePath())
		{
			std::string::size_type lastSlash = fullPath->find_last_of('/');
			name = fullPath->substr(lastSlash + 1);
		}
		else
		{
			std::string::size_type lastSlash = iter->first.find_last_of('/');
			name = iter->first.substr(lastSlash + 1);
		}
		
		NSString* title = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
		NSMenuItem* resolverItem = [[[NSMenuItem alloc] initWithTitle:title action:NULL keyEquivalent:@""] autorelease];
		
		if (const std::string* path = resolver->FilePath())
		{
			NSString* libraryPath = [NSString stringWithCString:path->c_str() encoding:NSUTF8StringEncoding];
			resolverItem.image = [self fileIcon16x16:libraryPath];
		}
		[resolverMenu addItem:resolverItem];
		
		NSMenu* submenu = [[[NSMenu alloc] initWithTitle:title] autorelease];
		resolverItem.submenu = submenu;
		
		if (NSMenu* exportsMenu = [self exportMenuForResolver:resolver])
		{
			NSMenuItem* exports = [[[NSMenuItem alloc] initWithTitle:@"Exports" action:NULL keyEquivalent:@""] autorelease];
			exports.submenu = exportsMenu;
			[submenu addItem:exports];
		}
		
		// unfortunately we have to do typecasts from this point...
		if (const PEFSymbolResolver* pef = dynamic_cast<const PEFSymbolResolver*>(resolver))
		{
			PPCVM::Disassembly::FancyDisassembler disasm(vm->allocator);
			const PEF::Container& container = pef->GetContainer();
			NSMutableDictionary* labelToArray = [NSMutableDictionary dictionary];
			for (int i = 0; i < container.Size(); i++)
			{
				const PEF::InstantiableSection& section = container.GetSection(i);
				PEF::SectionType type = section.GetSectionType();
				if (type == PEF::SectionType::Code || type == PEF::SectionType::ExecutableData)
				{
					CXObjCDisassemblyWriter writer(i);
					disasm.Disassemble(container, writer);
					NSArray* result = writer.GetDisassembly();
					if (result.count == 0)
						continue;
					
					NSMutableArray* currentArray = [NSMutableArray array];
					for (NSDictionary* dict in result)
					{
						NSString* label = [dict objectForKey:@"label"];
						if ([[label substringToIndex:2] isEqualToString:@"fn"])
							currentArray = [NSMutableArray array];
						[currentArray addObject:dict];
						[labelToArray setObject:currentArray forKey:label];
					}
					
					NSString* menuTitle = [NSString stringWithCString:section.Name.c_str() encoding:NSUTF8StringEncoding];
					NSMenuItem* sectionItem = [[[NSMenuItem alloc] initWithTitle:menuTitle action:NULL keyEquivalent:@""] autorelease];
					
					NSMenu* labelMenu = [[[NSMenu alloc] initWithTitle:menuTitle] autorelease];
					for (NSDictionary* label in result)
					{
						NSString* labelTitle = [label objectForKey:@"label"];
						NSMenuItem* labelMenuItem = [[NSMenuItem alloc] initWithTitle:labelTitle action:NULL keyEquivalent:@""];
						labelMenuItem.image = [[labelTitle substringToIndex:2] isEqualToString:@"lb"] ? labelImage : functionImage;
						[labelMenu addItem:labelMenuItem];
					}
					sectionItem.submenu = labelMenu;
					[submenu addItem:sectionItem];
				}
			}
			disassembly = [labelToArray copy];
		}
	}

	navBar.selectionChanged = ^(CXNavBar* bar, NSMenuItem* selection) { [self showDisassembly:selection.title]; };
	navBar.menu = resolverMenu;
}

-(NSImage*)fileIcon16x16:(NSString *)path
{
	const NSSize targetSize = NSMakeSize(16, 16);
	NSImage* icon = [[NSWorkspace sharedWorkspace] iconForFile:path];
	NSImage* smallIcon = [[NSImage alloc] initWithSize:targetSize];
	
	NSSize actualSize = icon.size;
	[smallIcon lockFocus];
	[[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
	[icon drawInRect:NSMakeRect(0, 0, targetSize.width, targetSize.height) fromRect:NSMakeRect(0, 0, actualSize.width, actualSize.height) operation:NSCompositeCopy fraction:1];
	[smallIcon unlockFocus];
	
	return [smallIcon autorelease];
}

-(void)showDisassemblyPage
{
	NSData* pageData;
	NSString* mimeType;
	NSURL* rootUrl = [NSURL URLWithString:@"cxdb:disassembly"];
	NSString* cxdbPath = [NSBundle.mainBundle pathForResource:@"cxdb" ofType:@"xhtml"];
	NSMutableString* xhtml = [NSMutableString stringWithContentsOfFile:cxdbPath encoding:NSUTF8StringEncoding error:NULL];
	if (xhtml == nil)
	{
		NSString* error = [NSMutableString stringWithString:@"<title>Oops</title><p>There was an error loading the disassembler user interface."];
		pageData = [error dataUsingEncoding:NSUTF8StringEncoding];
		mimeType = @"text/html";
	}
	else
	{
		unsigned docId = (unsigned)[[CXDocumentController documentController] idOfDocument:self];
		NSString* documentId = [NSString stringWithFormat:@"%u", docId];
		NSRange fullRange = NSMakeRange(0, xhtml.length);
		[xhtml replaceOccurrencesOfString:@"##data-document-id##" withString:documentId options:NSLiteralSearch range:fullRange];
		mimeType = @"application/xhtml+xml";
		pageData = [xhtml dataUsingEncoding:NSUTF8StringEncoding];
	}
	[[disassemblyView mainFrame] loadData:pageData MIMEType:@"application/xhtml+xml" textEncodingName:@"UTF-8" baseURL:rootUrl];
}

-(void)showDisassembly:(NSString *)key
{
	if (NSArray* functionDisassembly = [disassembly objectForKey:key])
	{
		NSString* scriptTemplate = @"ShowDisassembly(%@); GoToLabel(\"%@\");";
		NSString* disasmScript = [NSString stringWithFormat:scriptTemplate, CXJSONEncode(functionDisassembly), key];
		auto scriptObject = [disassemblyView windowScriptObject];
		[scriptObject evaluateWebScript:disasmScript];
	}
}

-(NSMenu*)exportMenuForResolver:(const CFM::SymbolResolver *)resolver
{
	std::vector<std::string> symbols = resolver->SymbolList();
	if (symbols.size() == 0)
		return nil;
	
	NSMenu* exportMenu = [[NSMenu alloc] initWithTitle:@"Exports"];
	for (const std::string& symbol : symbols)
	{
		NSString* title = [NSString stringWithCString:symbol.c_str() encoding:NSUTF8StringEncoding];
		NSMenuItem* item = [[[NSMenuItem alloc] initWithTitle:title action:NULL keyEquivalent:@""] autorelease];
		item.image = exportImage;
		[item setEnabled:NO];
		[exportMenu addItem:item];
	}
	
	return [exportMenu autorelease];
}

@end
