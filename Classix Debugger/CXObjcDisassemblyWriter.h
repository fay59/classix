//
//  CXObjcDisassemblyWriter.h
//  Classix
//
//  Created by Félix on 2012-11-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __Classix__CXObjcDisassemblyWriter__
#define __Classix__CXObjcDisassemblyWriter__

#include "FancyDisassembler.h"
#import <Cocoa/Cocoa.h>

class CXObjCDisassemblyWriter : public PPCVM::Disassembly::DisassemblyWriter
{
	NSMutableArray* result;
	NSMutableArray* currentArray;
	
	uint32_t desiredSection;
	bool inSection;
	
public:
	CXObjCDisassemblyWriter(uint32_t desiredSection);
	
	virtual void EnterSection(const PEF::InstantiableSection& section, uint32_t sectionIndex);
	virtual void EnterLabel(const PPCVM::Disassembly::InstructionRange& label, intptr_t labelAddress);
	virtual void VisitOpcode(const PPCVM::Disassembly::DisassembledOpcode& opcode, intptr_t opcodeAddress, const std::string* metadata);
	
	NSArray* GetDisassembly();
	
	~CXObjCDisassemblyWriter();
};

#endif /* defined(__Classix__CXObjcDisassemblyWriter__) */
