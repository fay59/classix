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

-(void*)allocate:(NSUInteger)size;
-(void)deallocate:(void*)ptr;

@end
