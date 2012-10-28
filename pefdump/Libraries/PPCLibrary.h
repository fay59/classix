//
//  PPCLibrary.h
//  pefdump
//
//  Created by Félix on 2012-10-26.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PPCMachineState.h"
#import "PPCAllocator.h"

// Signature of a bridged Intel-to-PowerPC call. This is what the -resolve: call is expected to return.
// Here, id will be the PPCLibrary object; SEL, the name of the function; and PPCMachineState*, the machine state
// object.
typedef void (*PPCLibraryFunction)(id, SEL, PPCMachineState*);

// The PPCLibrary interface proposes two methods: -libraryName and -resolve:. For method resolution, though,
// the preferred way is to implement a method called [LibraryName]_[FunctionName]: that accepts, as a single
// argument, a PPCMachineState*. The existence of such a method is checked first. If it does not exist, then the
// -resolve: method is called.

// For non-code symbols ONLY, the method is expected to accept no argument, and return a void*. It is imperative
// that the returned pointer never changes. There is also NO WAY to retrieve data symbols other than through this
// scheme.

// All memory should be allocated using the PPCAllocator* object passed in the init method.
@protocol PPCLibrary <NSObject>

-(id)initWithAllocator:(PPCAllocator*)allocator;

-(NSString*)libraryName;
-(PPCLibraryFunction)resolve:(NSString*)functionName;

@end
