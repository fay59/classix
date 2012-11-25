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
#import "CXNavBar.h"

struct ClassixCoreVM;

extern NSString* CXErrorDomain;
extern NSString* CXErrorFileURL;

enum CXErrorCode
{
	CXErrorCodeNotLocalURL = 1,
	CXErrorCodeFileNotLoadable = 2
};

@interface CXDocument : NSDocument
{
	struct ClassixCoreVM* vm;
	NSDictionary* disassembly;
	WebView* disassemblyView;
	CXNavBar* navBar;
	NSSegmentedControl* backForward;
}

@property (assign) IBOutlet WebView* disassemblyView;
@property (assign) IBOutlet CXNavBar* navBar;
@property (assign) IBOutlet NSSegmentedControl* backForward;

-(IBAction)navigate:(id)sender;

-(id)executeCommand:(NSString*)command arguments:(NSArray*)arguments;
-(NSArray*)disassemblyForLabel:(NSString*)label;

@end
