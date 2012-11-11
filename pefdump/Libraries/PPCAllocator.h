//
//  PPCAllocator.h
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PPCAllocator : NSObject
{
	void* allocator;
}

-(id)initWithAllocator:(void*)allocator;

-(void*)translate:(intptr_t)offset;

-(void*)allocate:(NSUInteger)size reason:(NSString*)reason;
-(void)deallocate:(void*)ptr;

@end
