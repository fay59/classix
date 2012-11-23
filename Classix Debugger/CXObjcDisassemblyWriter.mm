//
// CXObjcDisassemblyWriter.mm
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

#include "CXObjcDisassemblyWriter.h"

CXObjCDisassemblyWriter::CXObjCDisassemblyWriter(uint32_t desiredSection)
: desiredSection(desiredSection), inSection(false)
{
	result = [[NSMutableArray alloc] init];
}

void CXObjCDisassemblyWriter::EnterSection(const PEF::InstantiableSection &section, uint32_t sectionIndex)
{
	inSection = sectionIndex == desiredSection;
}

void CXObjCDisassemblyWriter::EnterLabel(const PPCVM::Disassembly::InstructionRange &label, intptr_t labelAddress)
{
	if (!inSection) return;
	
	currentArray = [NSMutableArray array];
	NSDictionary* objcLabel = @{
		@"label": @(label.Name.c_str()),
		@"instructions": currentArray
	};
	[result addObject:objcLabel];
}

void CXObjCDisassemblyWriter::VisitOpcode(const PPCVM::Disassembly::DisassembledOpcode &opcode, intptr_t opcodeAddress, const std::string *metadata)
{
	if (!inSection) return;
	
	NSMutableArray* arguments = [NSMutableArray array];
	for (int i = 0; i < opcode.Arguments.size(); i++)
	{
		NSMutableDictionary* objcOpcode = [NSMutableDictionary dictionary];
		auto& arg = opcode.Arguments[i];
		[objcOpcode setObject:@(static_cast<int>(arg.Format)) forKey:@"type"];
		[objcOpcode setObject:@(arg.Value) forKey:@"value"];
		if (arg.Format == PPCVM::Disassembly::OpcodeArgumentFormat::Offset)
		{
			i++;
			assert(opcode.Arguments[i].Format == PPCVM::Disassembly::OpcodeArgumentFormat::GPR);
			[objcOpcode setObject:@(opcode.Arguments[i].Value) forKey:@"gpr"];
		}
		[arguments addObject:objcOpcode];
	}
	
	id meta = metadata == nullptr ? [NSNull null] : @(metadata->c_str());
	NSDictionary* objcOpcode = @{
		@"location": @(opcodeAddress),
		@"code": @(opcode.Instruction.hex),
		@"opcode": @(opcode.Opcode.c_str()),
		@"target": meta,
		@"arguments": arguments
	};
	
	[currentArray addObject:objcOpcode];
}

NSArray* CXObjCDisassemblyWriter::GetDisassembly()
{
	return [[result retain] autorelease];
}

CXObjCDisassemblyWriter::~CXObjCDisassemblyWriter()
{
	[result release];
}
