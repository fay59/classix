//
// CXStackFrame.m
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

#import "CXStackFrame.h"

@implementation CXStackFrame

@synthesize functionLabel;
@synthesize label;
@synthesize labelOffset;

-(unsigned)functionOffset
{
	return labelOffset + label.address - functionLabel.address;
}

-(unsigned)absoluteAddress
{
	return label.address + labelOffset;
}

-(id)initWithFunction:(CXCodeLabel*)func label:(CXCodeLabel*)aLabel offset:(unsigned)offset
{
	if (!(self = [super init]))
		return nil;
	
	functionLabel = [func retain];
	label = [aLabel retain];
	labelOffset = offset;
	
	return self;
}

-(void)dealloc
{
	[functionLabel release];
	[label release];
	[super dealloc];
}

@end
