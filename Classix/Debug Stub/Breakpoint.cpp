//
//  Breakpoint.cpp
//  Classix
//
//  Created by Félix on 2013-09-05.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#include "Breakpoint.h"

using namespace std;
using namespace Common;
using namespace PPCVM;

const uint32_t BreakpointTrap = 0x7FC00008;

Breakpoint::Breakpoint(BreakpointSet& set, UInt32* instruction)
: set(set), location(instruction), instruction(*instruction)
{
	set.SetBreakpoint(instruction);
}

Breakpoint::Breakpoint(Breakpoint&& that)
: set(that.set), instruction(that.instruction), location(that.location)
{
	that.location = nullptr;
}

const UInt32* Breakpoint::GetLocation() const
{
	return location;
}

PPCVM::Instruction Breakpoint::GetInstruction() const
{
	return instruction;
}

Breakpoint::~Breakpoint()
{
	if (location != nullptr)
	{
		set.RemoveBreakpoint(location);
	}
}

void BreakpointSet::SetBreakpoint(Common::UInt32 *location)
{
	lock_guard<mutex> guard(mapMutex);
	auto iter = breakpoints.find(location);
	if (iter == breakpoints.end())
	{
		iter = breakpoints.insert(make_pair(location, make_pair(Instruction(location->Get()), 0))).first;
		*location = BreakpointTrap;
	}
	
	iter->second.second++;
}

bool BreakpointSet::RemoveBreakpoint(Common::UInt32 *location)
{
	lock_guard<mutex> guard(mapMutex);
	auto iter = breakpoints.find(location);
	if (iter == breakpoints.end())
		return false;
	
	iter->second.second--;
	if (iter->second.second == 0)
	{
		*iter->first = iter->second.first.hex;
		breakpoints.erase(iter);
	}
	return true;
}

Breakpoint BreakpointSet::CreateBreakpoint(Common::UInt32 *location)
{
	return Breakpoint(*this, location);
}

BreakpointSet::InhibitedBreakpoints BreakpointSet::InhibitBreakpoints()
{
	unique_lock<mutex> lock(mapMutex);
	return InhibitedBreakpoints(new BreakpointContext(std::move(lock), breakpoints));
}

bool BreakpointSet::GetRealInstruction(Common::UInt32 *location, PPCVM::Instruction &output)
{
	auto iter = breakpoints.find(location);
	if (iter == breakpoints.end())
	{
		output.hex = *location;
		return false;
	}
	else
	{
		output = iter->second.first;
		return true;
	}
}

BreakpointSet::~BreakpointSet()
{
	for (auto iter = breakpoints.begin(); iter != breakpoints.end(); iter++)
	{
		*iter->first = iter->second.first.hex;
	}
}

BreakpointSet::BreakpointContext::BreakpointContext(unique_lock<mutex>&& lock, unordered_map<UInt32*, pair<Instruction, unsigned>>& context)
: lock(std::move(lock)), source(context)
{
	breakpoints.swap(source);
	for (auto iter = breakpoints.begin(); iter != breakpoints.end(); iter++)
	{
		*iter->first = iter->second.first.hex;
	}
}

BreakpointSet::BreakpointContext::~BreakpointContext()
{
	for (auto iter = breakpoints.begin(); iter != breakpoints.end(); iter++)
	{
		*iter->first = BreakpointTrap;
	}
	source.swap(breakpoints);
}
