//
// CXILApplication.m
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

#include <list>
#include <dispatch/dispatch.h>
#include "CommonDefinitions.h"

#import "CXILApplication.h"

using namespace Common;
using namespace InterfaceLib;

static inline BOOL isFDValid(int fd)
{
	return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

@interface CXILApplication (GoryDetails)

-(BOOL)suggestEventRecord:(const EventRecord&)record;

@end

@implementation CXILApplication
{
	int writeHandle;
	int readHandle;
	dispatch_source_t ipcSource;
	
	std::list<EventRecord> eventQueue;
	uint16_t mouseButtonState;
	EventMask currentlyWaitingOn;
}

#define IPC_INDEX(x) [(unsigned)IPCMessage::x]

SEL ipcSelectors[] = {
	IPC_INDEX(PeekNextEvent) = @selector(peekNextEvent),
	IPC_INDEX(DequeueNextEvent) = @selector(discardNextEvent),
};

const size_t ipcSelectorCount = sizeof ipcSelectors / sizeof(SEL);

#pragma mark -
#pragma mark Overrides
-(id)init
{
	if (!(self = [super init]))
		return nil;
	
	NSArray* arguments = NSProcessInfo.processInfo.arguments;
	if (arguments.count != 3)
	{
		NSLog(@"InterfaceLibHead is not meant to be run directly. Please let InterfaceLib launch it.");
		NSLog(@"Bad arguments passed to main().");
		return nil;
	}
	
	readHandle = [arguments[1] intValue];
	writeHandle = [arguments[2] intValue];
	
	if (readHandle == 0 || writeHandle == 0 || !isFDValid(readHandle) || !isFDValid(writeHandle))
	{
		NSLog(@"InterfaceLibHead is not meant to be run directly. Please let InterfaceLib launch it.");
		NSLog(@"Either the read or write pipe is invalid.");
		return nil;
	}
	
	ipcSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ, readHandle, 0, dispatch_get_main_queue());
	dispatch_source_set_event_handler(ipcSource, ^{ [self processIPCMessage]; });
	dispatch_resume(ipcSource);
	
	return self;
}

-(void)sendEvent:(NSEvent *)theEvent
{
	NSPoint globalCoordinates = theEvent.window != nil
		? [theEvent.window convertBaseToScreen:theEvent.locationInWindow]
		: theEvent.locationInWindow;
	
	EventRecord eventRecord = {
		.when = Common::UInt32(theEvent.timestamp * 60),
		.where = {
			.h = Common::SInt16(globalCoordinates.x),
			.v = Common::SInt16(globalCoordinates.y)
		},
	};
	
	uint16_t modifiers = mouseButtonState;
	
	if ((theEvent.modifierFlags & NSCommandKeyMask) == NSCommandKeyMask)
		modifiers |= static_cast<uint16_t>(EventModifierFlags::cmdKey);
	
	if ((theEvent.modifierFlags & NSShiftKeyMask) == NSShiftKeyMask)
		modifiers |= static_cast<uint16_t>(EventModifierFlags::shiftKey);
	
	if ((theEvent.modifierFlags & NSAlphaShiftKeyMask) == NSAlphaShiftKeyMask)
		modifiers |= static_cast<uint16_t>(EventModifierFlags::alphaLock);
	
	if ((theEvent.modifierFlags & NSAlternateKeyMask) == NSAlternateKeyMask)
		modifiers |= static_cast<uint16_t>(EventModifierFlags::optionKey);
	
	if ((theEvent.modifierFlags & NSControlKeyMask) == NSControlKeyMask)
		modifiers |= static_cast<uint16_t>(EventModifierFlags::controlKey);
	
	// TODO active state?
	// TODO right shift, command, control?
	
	switch (theEvent.type)
	{
		case NSRightMouseDown:
			modifiers |= static_cast<uint16_t>(EventModifierFlags::controlKey);
		case NSLeftMouseDown:
			eventRecord.what = Common::UInt16(static_cast<uint16_t>(EventCode::mouseDown));
			eventRecord.message = 0;
			mouseButtonState = static_cast<uint16_t>(EventModifierFlags::mouseButtonState);
			break;
			
		case NSRightMouseUp:
			modifiers |= static_cast<uint16_t>(EventModifierFlags::controlKey);
		case NSLeftMouseUp:
			eventRecord.what = Common::UInt16(static_cast<uint16_t>(EventCode::mouseUp));
			eventRecord.message = 0;
			mouseButtonState = 0;
			break;
			
			uint16_t keyMessage;
		case NSKeyDown:
			keyMessage = ((theEvent.keyCode & 0xff) << 8);
			keyMessage |= [theEvent.characters cStringUsingEncoding:NSMacOSRomanStringEncoding][0];
			
			eventRecord.what =
				Common::UInt16(static_cast<uint16_t>(theEvent.isARepeat ? EventCode::autoKey : EventCode::keyDown));
			eventRecord.message = keyMessage;
			break;
			
		case NSKeyUp:
			keyMessage = ((theEvent.keyCode & 0xff) << 8);
			keyMessage |= [theEvent.characters cStringUsingEncoding:NSMacOSRomanStringEncoding][0];
			
			eventRecord.what = Common::UInt16(static_cast<uint16_t>(EventCode::keyUp));
			eventRecord.message = keyMessage;
			break;
			
		// TODO updateEvent, diskEvent, activateEvent, osEvent, highLevelEvent
	}
	
	eventRecord.modifiers = Common::UInt16(modifiers);
	
	if (![self suggestEventRecord:eventRecord])
	{
		eventQueue.push_back(eventRecord);
	}
}

-(void)dealloc
{
	if (ipcSource != nullptr)
		dispatch_suspend(ipcSource);
}

#pragma mark -
#pragma mark RPCs
-(void)readInto:(void *)into size:(size_t)size
{
	size_t count = read(readHandle, into, size);
	if (count < size)
	{
		// broken pipe: the parent process probably quit
		[self terminate:self];
	}
}

-(void)writeFrom:(const void *)from size:(size_t)size
{
	size_t count = write(writeHandle, from, size);
	if (count < size)
	{
		// broken pipe: the parent process probably quit
		[self terminate:self];
	}
}

-(void)sendDone
{
	[self writeFrom:"DONE" size:4];
}

-(void)expectDone
{
	char done[4];
	[self readInto:done size:sizeof done];
	if (memcmp(done, "DONE", sizeof done) != 0)
	{
		NSLog(@"*** Expected a DONE, got %.4s", done);
		abort();
	}
}

-(void)processIPCMessage
{
	unsigned messageType;
	[self readInto:&messageType size:sizeof messageType];
	
	NSAssert(messageType < ipcSelectorCount, @"Message type %u is undefined", messageType);
	
	// this operation is safe, because the selector accepts no argument and returns no object
	// since no leak is possible, we shut up the compiler
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
	[self performSelector:ipcSelectors[messageType]];
#pragma clang diagnostic pop
}

-(void)peekNextEvent
{
	uint16_t desiredEvent;
	[self readInto:&desiredEvent size:sizeof desiredEvent];
	[self expectDone];
	
	currentlyWaitingOn = static_cast<EventMask>(desiredEvent);
	
	// if we have one such event, send it to the client
	for (const EventRecord& event : eventQueue)
	{
		if ([self suggestEventRecord:event])
		{
			return;
		}
	}
	
	// otherwise, wait for one such event
}

-(void)discardNextEvent
{
	uint16_t desiredEvent;
	[self readInto:&desiredEvent size:sizeof desiredEvent];
	[self expectDone];
	
	for (auto iter = eventQueue.begin(); iter != eventQueue.end(); iter++)
	{
		if ((iter->what & desiredEvent) != 0)
		{
			eventQueue.erase(iter);
			break;
		}
	}
	
	[self sendDone];
}

#pragma mark -
#pragma mark Gory details

-(BOOL)suggestEventRecord:(const EventRecord &)record
{
	if (((1 << record.what) & static_cast<uint16_t>(currentlyWaitingOn)) != 0)
	{
		[self writeFrom:&record size:sizeof record];
		[self sendDone];
		currentlyWaitingOn = EventMask::noEvent;
		return YES;
	}
	return NO;
}

@end
