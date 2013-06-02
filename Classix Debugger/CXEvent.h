//
// CXEvent.h
// Classix
//
// Copyright (C) 2013 Félix Cloutier
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

struct CXEventCXX;

typedef void (^CXEventListener)(id owner, NSDictionary* eventData);

@interface CXEvent : NSObject
{
	struct CXEventCXX* cxx;
	__weak id owner;
}

-(id)initWithOwner:(id)owner;

-(void)attach:(CXEventListener)listener;
-(void)detach:(CXEventListener)listener;
-(void)attachListener:(id)listener selector:(SEL)selector;
-(void)detachListener:(id)listener;

-(void)triggerWithData:(NSDictionary*)data;

@end
