//
// CXDocument.h
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

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import "CXVirtualMachine.h"
#import "CXDisassembly.h"
#import "CXNavBar.h"
#import "CXStackTrace.h"

@class CXJSAdapter;

@interface CXDocument : NSDocument
{
	CXVirtualMachine* vm;
	CXDisassembly* disassembly;
	CXStackTrace* stackTrace;
	CXJSAdapter* js;
	
	uint32_t sp;
	uint32_t lr;
	
	// UI
	WebView* disassemblyView;
	CXNavBar* navBar;
	NSSegmentedControl* backForward;
	NSOutlineView* outline;
	NSTableView* stackTraceTable;
}

@property (readonly) CXVirtualMachine* vm;
@property (readonly) CXDisassembly* disassembly;

@property (assign) IBOutlet WebView* disassemblyView;
@property (assign) IBOutlet CXNavBar* navBar;
@property (assign) IBOutlet NSSegmentedControl* backForward;
@property (assign) IBOutlet NSOutlineView* outline;
@property (assign) IBOutlet NSTableView* stackTraceTable;

-(IBAction)goBack:(id)sender;
-(IBAction)goForward:(id)sender;
-(IBAction)run:(id)sender;
-(IBAction)stepOver:(id)sender;
-(IBAction)stepInto:(id)sender;

-(IBAction)navigate:(id)sender;
-(IBAction)controlFlow:(id)sender;

@end
