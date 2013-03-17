//
// CXVirtualMachine.h
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
#import "CXEvent.h"

struct ClassixCoreVM;

extern NSNumber* CXVirtualMachineGPRKey;
extern NSNumber* CXVirtualMachineFPRKey;
extern NSNumber* CXVirtualMachineCRKey;
extern NSNumber* CXVirtualMachineSPRKey;

extern NSString* CXErrorDomain;
extern NSString* CXErrorFilePath;

extern NSString* CXVirtualMachineBreakpointChangeTypeKey;
extern NSString* CXVirtualMachineBreakpointAddressKey;
extern NSString* CXVirtualMachineAddedBreakpoint;
extern NSString* CXVirtualMachineRemovedBreakpoint;

enum CXErrorCode
{
	CXErrorCodeInternalError = -1,
	CXErrorCodeNotLocalURL = 1,
	CXErrorCodeFileNotLoadable = 2,
	CXErrorCoreExecutableAlreadyLoaded = 3,
	CXErrorCodeFileNotExecutable = 4,
};

enum CXVirtualMachineSPRIndex
{
	CXVirtualMachineSPRXERIndex = 0,
	CXVirtualMachineSPRLRIndex = 1,
	CXVirtualMachineSPRCTRIndex = 2,
};

@interface CXVirtualMachine : NSObject <NSOutlineViewDataSource, NSOutlineViewDelegate>
{
	struct ClassixCoreVM* vm;
	NSDictionary* registers;
	NSMutableSet* breakpoints;
	NSMutableSet* changedRegisters;
	NSString* lastError;
	CXEvent* breakpointsChanged;
	uint32_t pc;
}

@property (assign) uint32_t pc;
@property (readonly) NSArray* gpr;
@property (readonly) NSArray* fpr;
@property (readonly) NSArray* cr;
@property (readonly) NSArray* spr;
@property (readonly) NSString* lastError;
@property (readonly) NSSet* breakpoints;
@property (readonly) NSDictionary* allRegisters; // split by category
@property (readonly) CXEvent* breakpointsChanged;

-(id)init;

-(BOOL)loadClassicExecutable:(NSString *)executablePath error:(NSError **)error;

-(void)setArgv:(NSArray*)argv envp:(NSDictionary*)envp;
-(void)transitionByAddress:(uint32_t)address;

-(NSValue*)fragmentManager;
-(NSValue*)allocator;

-(NSString*)symbolNameOfAddress:(unsigned)address;
-(NSString*)explainAddress:(unsigned)address;
-(NSString*)stringAtAddress:(unsigned)address;
-(NSNumber*)wordAtAddress:(unsigned)address;
-(NSNumber*)floatAtAddress:(unsigned)address;
-(NSNumber*)doubleAtAddress:(unsigned)address;

-(void)addBreakpoint:(uint32_t)address;
-(void)removeBreakpoint:(uint32_t)address;
-(BOOL)toggleBreakpoint:(uint32_t)address;
-(BOOL)breakpointExists:(uint32_t)address;

-(NSArray*)stackTrace;

-(IBAction)run:(id)sender;
-(IBAction)stepOver:(id)sender;
-(IBAction)stepInto:(id)sender;

-(void)runTo:(uint32_t)location;

@end
