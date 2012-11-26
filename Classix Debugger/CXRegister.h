//
//  CXRegister.h
//  Classix
//
//  Created by Félix on 2012-11-25.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum
{
	CXRegisterGPR,
	CXRegisterFPR,
	CXRegisterCR,
	CXRegisterSPR
} CXRegisterType;

@interface CXRegister : NSObject
{
	NSString* name;
	CXRegisterType type;
	void* address;
}

@property (readonly) NSString* name;
@property (readonly) CXRegisterType type;
@property (copy) NSNumber* value;

+(id)GPRNumber:(int)number location:(uint32_t*)location;
+(id)FPRNumber:(int)number location:(double*)location;
+(id)CRNumber:(int)number location:(uint8_t*)location;
+(id)SPRName:(NSString*)name location:(uint32_t*)location;

-(id)initWithName:(NSString*)name address:(void*)address type:(CXRegisterType)size;

-(NSString*)description;

@end
