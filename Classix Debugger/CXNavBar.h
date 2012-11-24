//
//  CXNavBar.h
//  Classix
//
//  Created by Félix on 2012-11-23.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class CXNavBar;
typedef void (^CXNavBarSelectionChanged)(CXNavBar* bar, NSMenuItem* newItem);

@interface CXNavBar : NSView
{
	NSMenu* menu;
	NSMenuItem* selectedItem;
	NSDictionary* hitBoxes;
	CXNavBarSelectionChanged selectionChanged;
}

@property (nonatomic, copy) NSMenu* menu;
@property (copy) CXNavBarSelectionChanged selectionChanged;

@end
