//
//  PPCAllocator.m
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "PPCAllocator.h"
#include "IAllocator.h"

#define ALLOCATOR reinterpret_cast<Common::IAllocator*>(allocator)

@implementation PPCAllocator

-(id)initWithAllocator:(void*)allo
{
	if (!(self = [super init]))
		return nil;
	
	allocator = allo;
	
	return self;
}

-(void*)translate:(intptr_t)offset
{
	return ALLOCATOR->GetBaseAddress() + offset;
}

-(void*)allocate:(NSUInteger)size reason:(NSString*)reason
{
	return ALLOCATOR->Allocate(size, [reason UTF8String]);
}

-(void)deallocate:(void *)ptr
{
	ALLOCATOR->Deallocate(ptr);
}

@end
