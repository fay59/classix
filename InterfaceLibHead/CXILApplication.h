//
// CXILApplication.h
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
#import "CXILEventHandler.h"

@interface CXILApplication : NSApplication <CXILEventHandler>

// Reacting to stuff
-(void)sendEvent:(NSEvent *)theEvent;
-(CXILEventHandlerActionResult)handleEvent:(NSEvent *)event;
-(CXILEventHandlerActionResult)handleNotification:(NSNotification *)notification;

// IPC messages implementation
-(void)processIPCMessage;

// get a copy of the next EventRecord matching the EventMask without altering the queue
-(void)peekNextEvent;

// discard the next EventRecord matching the EventMask; does nothing if there's no such event
-(void)discardNextEvent;

// tells if the mouse button is down
-(void)tellIsMouseDown;

// makes a beep
-(void)beep;

// creates a window
-(void)createWindow;

// closes and destroys a window
-(void)closeWindow;

// puts an update event on the queue
-(void)requestUpdate;

// returns the first window in which the coordinate exists
-(void)findWindow;

// marks a view rectangle as dirty
-(void)setDirtyRect;

// repaints views marked dirty
-(void)refreshWindows;

// removes menus from the menu bar, leaving only the standard Classix one
-(void)clearMenus;

// adds a menu to the menu bar
-(void)insertMenu;

// checks a menu item
-(void)checkMenuItem;

// adds a menu item to a menu
-(void)insertMenuItem;

// open menus, return when an option has been selected (or menus have been closed)
-(void)menuSelect;

// finds a menu by its key equivalent
-(void)menuKey;

@end
