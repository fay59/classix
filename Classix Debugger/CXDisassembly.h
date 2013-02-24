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
#import "CXEvent.h"

extern NSString* CXDisassemblyUniqueNameKey;
extern NSString* CXDisassemblyDisplayNameKey;

struct CXDisassemblyCXX;

@interface CXDisassembly : NSObject
{
	CXVirtualMachine* vm;
	NSMutableDictionary* displayNames;
	NSDictionary* addressesToUniqueNames;
	NSDictionary* disassembly;
	NSArray* orderedAddresses;
	CXEvent* nameChanged;
	struct CXDisassemblyCXX* cxx;
}

@property (readonly) CXEvent* nameChanged;
@property (copy) NSMutableDictionary* displayNames;

-(id)initWithVirtualMachine:(CXVirtualMachine*)vm;

-(NSArray*)functionDisassemblyForUniqueName:(NSString*)uniqueName;
-(NSArray*)functionDisassemblyForAddress:(uint32_t)address;

-(CXCodeLabel*)labelDisassemblyForUniqueName:(NSString*)uniqueName;
-(CXCodeLabel*)labelDisassemblyForAddress:(uint32_t)address;

-(NSString*)closestLabelUniqueNameToAddress:(uint32_t)address;
-(NSString*)uniqueNameForAddress:(uint32_t)address;
-(uint32_t)addressForUniqueName:(NSString*)uniqueName;

-(NSString*)displayNameForUniqueName:(NSString*)uniqueName;
-(void)setDisplayName:(NSString*)displayName forUniqueName:(NSString*)uniqueName;

-(void)dealloc;

@end
