//
//  Breakpoint.h
//  Classix
//
//  Created by Félix on 2013-09-05.
//  Copyright (c) 2013 Félix. All rights reserved.
//

#ifndef __Classix__Breakpoint__
#define __Classix__Breakpoint__

#include "BigEndian.h"
#include "Instruction.h"

#include <mutex>
#include <unordered_map>

class BreakpointSet;

class Breakpoint
{
	friend class BreakpointSet;
	BreakpointSet& set;
	Common::UInt32* location;
	PPCVM::Instruction instruction;
	
	Breakpoint(BreakpointSet& set, Common::UInt32* location);
	
public:
	Breakpoint(Breakpoint&& that);
	Breakpoint(const Breakpoint& that) = delete;
	
	const Common::UInt32* GetLocation() const;
	PPCVM::Instruction GetInstruction() const;
	~Breakpoint();
};

class BreakpointSet
{
	std::mutex mapMutex;
	std::unordered_map<Common::UInt32*, std::pair<PPCVM::Instruction, unsigned>> breakpoints;
	
public:
	class BreakpointContext
	{
		friend class BreakpointSet;
		std::unique_lock<std::mutex> lock;
		std::unordered_map<Common::UInt32*, std::pair<PPCVM::Instruction, unsigned>> breakpoints;
		std::unordered_map<Common::UInt32*, std::pair<PPCVM::Instruction, unsigned>>& source;
		
		BreakpointContext(std::unique_lock<std::mutex>&& lock, std::unordered_map<Common::UInt32*, std::pair<PPCVM::Instruction, unsigned>>& context);
	public:
		~BreakpointContext();
	};
	
	typedef std::unique_ptr<BreakpointContext> InhibitedBreakpoints;
	
	void SetBreakpoint(Common::UInt32* location);
	bool RemoveBreakpoint(Common::UInt32* location);
	Breakpoint CreateBreakpoint(Common::UInt32* location);
	
	InhibitedBreakpoints InhibitBreakpoints();
	bool GetRealInstruction(Common::UInt32* location, PPCVM::Instruction& output);
	
	~BreakpointSet();
};

#endif /* defined(__Classix__Breakpoint__) */
