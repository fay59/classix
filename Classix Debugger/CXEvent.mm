//
//  CXEventListener.m
//  Classix
//
//  Created by Félix on 2013-02-24.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#import "CXEvent.h"
#include <set>

struct CXEventCXX
{
	struct Callback
	{
		id target;
		SEL selector;
		
		Callback(id target, SEL selector)
		{
			this->target = [target retain];
			this->selector = selector;
		}
		
		Callback(const Callback& that)
		{
			this->target = [that.target retain];
			this->selector = that.selector;
		}
		
		Callback(Callback&& that)
		{
			this->target = that.target;
			that.target = nil;
			this->selector = that.selector;
		}
		
		void operator()(id owner, NSDictionary* data) const
		{
			if (selector == nullptr)
				((CXEventListener)target)(owner, data);
			else
				[target performSelector:selector withObject:owner withObject:data];
		}
		
		bool operator<(const Callback& that) const
		{
			NSUInteger thisHash = [target hash];
			NSUInteger thatHash = [that.target hash];
			if (thisHash < thatHash)
				return true;
			
			if (thisHash == thatHash && target < that.target)
				return true;
			
			return false;
		}
		
		~Callback()
		{
			[target release];
		}
	};
	
	std::set<Callback> listeners;
};

@implementation CXEvent

-(id)initWithOwner:(id)aOwner
{
	if (!(self = [super init]))
		return nil;
	
	owner = aOwner;
	cxx = new CXEventCXX;
	
	return self;
}

-(void)attach:(CXEventListener)listener
{
	[self attachListener:[[listener copy] autorelease] selector:nullptr];
}

-(void)detach:(CXEventListener)listener
{
	[self detachListener:listener];
}

-(void)attachListener:(id)listener selector:(SEL)selector
{
	cxx->listeners.emplace(listener, selector);
}

-(void)detachListener:(id)listener
{
	cxx->listeners.erase(CXEventCXX::Callback(listener, nullptr));
}

-(void)triggerWithData:(NSDictionary *)data
{
	for (const CXEventCXX::Callback& callback : cxx->listeners)
		callback(self, data);
}

-(void)dealloc
{
	delete cxx;
	[super dealloc];
}

@end
