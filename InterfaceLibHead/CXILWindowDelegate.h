//
// CXILWindowDelegate.h
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

// This may be kind of a misnomer. It's the delegate class that handles window stuff in lieu of CXILApplication.
@interface CXILWindowDelegate : NSObject

-(void)createWindow:(uint32_t)key withRect:(NSRect)rect surface:(IOSurfaceRef)surface title:(NSString*)title visible:(BOOL)visible behind:(uint32_t)behindKey;
-(void)refreshWindow:(uint32_t)key;
-(void)destroyWindow:(uint32_t)windowID;

-(uint32_t)keyOfWindow:(NSWindow*)window;

@end
