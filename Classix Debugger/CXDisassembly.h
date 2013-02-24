//
// CXDisassembly.h
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

#import <Foundation/Foundation.h>
#import "CXVirtualMachine.h"
#import "CXCodeLabel.h"

struct CXDisassemblyCXX;

@interface CXDisassembly : NSObject
{
	CXVirtualMachine* vm;
	NSMutableDictionary* displayNames;
	NSDictionary* addressesToUniqueNames;
	NSDictionary* disassembly;
	NSArray* orderedAddresses;
	struct CXDisassemblyCXX* cxx;
}

@property (copy) NSMutableDictionary* displayNames;

-(id)initWithVirtualMachine:(CXVirtualMachine*)vm;

-(NSArray*)functionDisassemblyForUniqueName:(NSString*)uniqueName;
-(NSArray*)functionDisassemblyForAddress:(uint32_t)address;

-(CXCodeLabel*)labelDisassemblyForUniqueName:(NSString*)uniqueName;
-(CXCodeLabel*)labelDisassemblyForAddress:(uint32_t)address;

-(NSString*)closestUniqueNameToAddress:(uint32_t)address;

-(NSString*)displayNameForUniqueName:(NSString*)uniqueName;
-(void)setDisplayName:(NSString*)displayName forUniqueName:(NSString*)uniqueName;

-(void)registerNameChangeCallbackObject:(id)target selector:(SEL)selector;
-(void)unregisterNameChangeCallbackObject:(id)target;

-(void)dealloc;

@end
