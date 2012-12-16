//
// CXStackTrace.m
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

#import "CXStackTrace.h"
#import "CXStackFrame.h"

@interface CXStackTrace (Private)

-(CXStackFrame*)frameForAddress:(uint32_t)address;

@end

@implementation CXStackTrace

@synthesize stackTrace;
@synthesize onFrameSelected;

-(id)initWithDisassembly:(CXDisassembly *)aDisassembly
{
	if (!(self = [super init]))
		return nil;
	
	disassembly = [aDisassembly retain];
	
	return self;
}

-(void)feedNumericTrace:(NSArray *)trace
{
	NSMutableArray* realTrace = [NSMutableArray arrayWithCapacity:trace.count];
	for (NSNumber* number in trace)
	{
		CXStackFrame* frame = [self frameForAddress:number.unsignedIntValue];
		[realTrace addObject:frame];
	}
	
	[stackTrace release];
	stackTrace = [realTrace retain];
}

-(void)setTopAddress:(unsigned int)address
{
	CXStackFrame* frame = [self frameForAddress:address];
	[stackTrace replaceObjectAtIndex:0 withObject:frame];
}

-(CXStackFrame*)frameForAddress:(uint32_t)address
{
	CXCodeLabel* function = [[disassembly functionDisassemblyForAddress:address] objectAtIndex:0];
	CXCodeLabel* label = [disassembly labelDisassemblyForAddress:address];
	unsigned offset = address - label.address;
	return [[[CXStackFrame alloc] initWithFunction:function label:label offset:offset] autorelease];
}

-(NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return stackTrace.count;
}

-(id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	CXStackFrame* frame = [stackTrace objectAtIndex:row];
	NSString* displayLabel = [disassembly displayNameForUniqueName:frame.functionLabel.uniqueName];
	return [NSString stringWithFormat:@"%@ +%u", displayLabel, frame.functionOffset];
}

-(void)tableViewSelectionDidChange:(NSNotification *)notification
{
	NSTableView* sender = notification.object;
	NSInteger selectedItem = sender.selectedRow;
	if (selectedItem != -1 && onFrameSelected)
		onFrameSelected(self, [stackTrace objectAtIndex:selectedItem]);
}

-(void)dealloc
{
	[disassembly release];
	[stackTrace release];
	[super dealloc];
}

@end
