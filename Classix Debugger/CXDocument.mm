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

#import "CXDocument.h"
#import "CXDocumentController.h"
#import "CXRegister.h"
#import "CXDBRequest.h"
#import "CXJSONEncode.h"
#import "CXJSAdapter.h"
#import "CXCodeLabel.h"

static NSImage* exportImage;
static NSImage* labelImage;
static NSImage* functionImage;

@interface CXDocument (Private)

-(void)buildSymbolMenu;
-(void)showPCAndJump:(BOOL)jumpToPC;
-(NSImage*)fileIcon16x16:(NSString*)path;
-(NSMenu*)exportMenuForResolver:(const CFM::SymbolResolver*)resolver;

@end

@implementation CXDocument

@synthesize vm;
@synthesize disassembly;

@synthesize disassemblyView;
@synthesize navBar;
@synthesize backForward;
@synthesize outline;

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
		vm = [[CXVirtualMachine alloc] init];
		js = [[CXJSAdapter alloc] initWithDocument:self];
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
	
	[self buildSymbolMenu];
	[backForward setEnabled:NO forSegment:0];
	[backForward setEnabled:NO forSegment:1];
	[disassemblyView addObserver:self forKeyPath:@"canGoBack" options:NSKeyValueObservingOptionNew context:nullptr];
	[disassemblyView addObserver:self forKeyPath:@"canGoForward" options:NSKeyValueObservingOptionNew context:nullptr];
	
	outline.delegate = vm;
	outline.dataSource = vm;
	[self showPCAndJump:YES];
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
			*outError = [NSError errorWithDomain:CXErrorDomain code:CXErrorCodeNotLocalURL userInfo:nil];
		return NO;
	}
	
	// FIXME cheap
	NSArray* arguments = @[url.path];
	NSDictionary* environment = [NSDictionary dictionary];
	if ([vm loadClassicExecutable:url.path arguments:arguments environment:environment error:outError])
	{
		disassembly = [[CXDisassembly alloc] initWithVirtualMachine:vm];
		return YES;
	}
	return NO;
}

-(void)webView:(WebView*)sender didFinishLoadForFrame:(WebFrame *)frame
{
	[[sender windowScriptObject] setValue:js forKey:@"cxdb"];
	[self showPCAndJump:NO];
}

-(void)dealloc
{
	[vm release];
	[js release];
	[disassembly release];
	
	[super dealloc];
}

#pragma mark -

-(IBAction)goBack:(id)sender
{
	[disassemblyView goBack];
}

-(IBAction)goForward:(id)sender
{
	[disassemblyView goForward];
}

-(IBAction)run:(id)sender
{
	[vm run:sender];
	[outline reloadData];
	[self showPCAndJump:YES];
}

-(IBAction)stepInto:(id)sender
{
	[vm stepInto:sender];
	[outline reloadData];
	[self showPCAndJump:YES];
}

-(IBAction)stepOver:(id)sender
{
	[vm stepOver:sender];
	[outline reloadData];
	[self showPCAndJump:YES];
}

-(IBAction)controlFlow:(id)sender
{
	NSInteger segment = [sender selectedSegment];
	switch (segment)
	{
		case 0: return [self run:sender];
		case 1: return [self stepOver:sender];
		case 2: return [self stepInto:sender];
	}
}

-(IBAction)navigate:(id)sender
{
	NSInteger segment = [sender selectedSegment];
	if (segment == 0)
		[self goBack:sender];
	else
		[self goForward:sender];
}

#pragma mark -

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if (object == disassemblyView)
	{
		if ([keyPath isEqualToString:@"canGoBack"])
		{
			BOOL enable = [[change objectForKey:NSKeyValueChangeNewKey] boolValue];
			[backForward setEnabled:enable forSegment:0];
		}
		else if ([keyPath isEqualToString:@"canGoForward"])
		{
			BOOL enable = [[change objectForKey:NSKeyValueChangeNewKey] boolValue];
			[backForward setEnabled:enable forSegment:1];
		}
	}
}

-(void)buildSymbolMenu
{
	using namespace ClassixCore;
	using namespace CFM;
	
	// build the menus with the current resolvers
	NSMenu* resolverMenu = [[[NSMenu alloc] initWithTitle:@"Debugger"] autorelease];
	
	CFM::FragmentManager* cfm;
	Common::IAllocator* allocator;
	[[vm fragmentManager] getValue:&cfm];
	[[vm allocator] getValue:&allocator];
	
	for (auto iter = cfm->Begin(); iter != cfm->End(); iter++)
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
			PPCVM::Disassembly::FancyDisassembler disasm(allocator);
			const PEF::Container& container = pef->GetContainer();
			NSMutableDictionary* labelToArray = [NSMutableDictionary dictionary];
			NSMutableDictionary* addressToLabel = [NSMutableDictionary dictionary];
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
					
					NSString* menuTitle = [NSString stringWithCString:section.Name.c_str() encoding:NSUTF8StringEncoding];
					NSMenuItem* sectionItem = [[[NSMenuItem alloc] initWithTitle:menuTitle action:NULL keyEquivalent:@""] autorelease];
					NSMutableArray* currentArray = [NSMutableArray array];
					NSMenu* labelMenu = [[[NSMenu alloc] initWithTitle:menuTitle] autorelease];
					
					for (CXCodeLabel* codeLabel in result)
					{
						NSArray* instructions = codeLabel.instructions;
						if (instructions.count == 0)
							continue;
						
						uint32_t address = codeLabel.address;
						if (codeLabel.isFunction)
							currentArray = [NSMutableArray array];
						
						[currentArray addObject:codeLabel];
						[labelToArray setObject:currentArray forKey:codeLabel.uniqueName];
						[addressToLabel setObject:codeLabel.uniqueName forKey:@(address)];
						
						NSString* uniqueName = codeLabel.uniqueName;
						NSString* title = [disassembly displayNameForUniqueName:uniqueName];
						NSMenuItem* labelMenuItem = [[NSMenuItem alloc] initWithTitle:title action:NULL keyEquivalent:@""];
						labelMenuItem.representedObject = codeLabel;
						
						labelMenuItem.image = codeLabel.isFunction ? functionImage : labelImage;
						[labelMenu addItem:labelMenuItem];
					}
					sectionItem.submenu = labelMenu;
					[submenu addItem:sectionItem];
				}
			}
		}
	}

	navBar.selectionChanged = ^(CXNavBar* bar, NSMenuItem* selection) {
		CXCodeLabel* label = [selection representedObject];
		[self showDisassembly:label.uniqueName];
	};
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

-(void)showPCAndJump:(BOOL)jumpToPC
{
	if (jumpToPC)
	{
		
	}
	
	id<NSObject> scriptArguments = vm.lastError == nil
		? @(vm.pc)
		: [NSString stringWithFormat:@"%u, %@", vm.pc, CXJSONEncodeString(vm.lastError)];

	NSString* script = [NSString stringWithFormat:@"HighlightPC(%@)", scriptArguments];
	[[disassemblyView windowScriptObject] evaluateWebScript:script];
}

-(void)showDisassembly:(NSString *)labelUniqueName
{
	NSUInteger documentId = [[CXDocumentController documentController] idOfDocument:self];
	CXCodeLabel* function = [[disassembly functionDisassemblyForUniqueName:labelUniqueName] objectAtIndex:0];
	NSString* cxdbUrl = [NSString stringWithFormat:@"cxdb://disassembly/%@/%@#%@", @(documentId), function.uniqueName, labelUniqueName];
	NSURL* url = [NSURL URLWithString:cxdbUrl];
	NSURLRequest* request = [NSURLRequest requestWithURL:url];
	[[disassemblyView mainFrame] loadRequest:request];
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
