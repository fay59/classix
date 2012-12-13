//
// CXCodeLabel.h
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

@interface CXCodeLabel : NSObject
{
	NSString* label;
	NSString* uniqueName;
	uint32_t address;
	uint32_t length;
	NSArray* instructions;
	BOOL isFunction;
}

@property (copy) NSString* label;
@property (readonly) NSString* uniqueName;
@property (readonly) uint32_t address;
@property (readonly) uint32_t length;
@property (copy) NSArray* instructions;
@property (assign) BOOL isFunction;

+(id)codeLabelWithAddress:(uint32_t)address length:(uint32_t)length uniqueName:(NSString*)uniqueName;
-(id)initWithAddress:(uint32_t)address length:(uint32_t)length uniqueName:(NSString*)uniqueName;

@end
