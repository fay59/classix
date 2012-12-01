//
//  CXVirtualMachine.h
//  Classix
//
//  Created by Félix on 2012-11-25.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

struct ClassixCoreVM;

extern NSNumber* CXVirtualMachineGPRKey;
extern NSNumber* CXVirtualMachineFPRKey;
extern NSNumber* CXVirtualMachineCRKey;
extern NSNumber* CXVirtualMachineSPRKey;

extern NSString* CXErrorDomain;
extern NSString* CXErrorFilePath;

enum CXErrorCode
{
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
	uint32_t pc;
}

@property (assign) uint32_t pc;
@property (readonly) NSArray* gpr;
@property (readonly) NSArray* fpr;
@property (readonly) NSArray* cr;
@property (readonly) NSArray* spr;
@property (readonly) NSString* lastError;
@property (readonly) NSMutableSet* breakpoints;
@property (readonly) NSDictionary* allRegisters; // split by category

-(id)init;

-(BOOL)loadClassicExecutable:(NSString *)executablePath arguments:(NSArray*)args environment:(NSDictionary*)env error:(NSError **)error;

-(NSValue*)fragmentManager;
-(NSValue*)allocator;

-(IBAction)run:(id)sender;
-(IBAction)stepOver:(id)sender;
-(IBAction)stepInto:(id)sender;

-(void)runTo:(uint32_t)location;

@end
