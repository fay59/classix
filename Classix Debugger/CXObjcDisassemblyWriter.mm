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
#include <CommonCrypto/CommonCrypto.h>

CXObjCDisassemblyWriter::CXObjCDisassemblyWriter(uint32_t desiredSection)
: desiredSection(desiredSection), inSection(false), currentLabel(nil)
{
	result = [[NSMutableArray alloc] init];
}

void CXObjCDisassemblyWriter::GetSectionMD5(const PEF::InstantiableSection &section, char *output)
{
	static const char hexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	unsigned char md5[16];
	CC_MD5(section.Data, section.Size(), md5);
	for (int i = 0; i < sizeof(md5); i++)
	{
		output[i * 2] = hexChars[md5[i] >> 4];
		output[i * 2 + 1] = hexChars[md5[i] & 0xf];
	}
}

void CXObjCDisassemblyWriter::EnterSection(const PEF::InstantiableSection &section, uint32_t sectionIndex)
{
	inSection = sectionIndex == desiredSection;
	if (inSection)
	{
		GetSectionMD5(section, sectionMD5);
		sectionBase = section.GetDataLocation();
	}
}

void CXObjCDisassemblyWriter::EnterLabel(const PPCVM::Disassembly::InstructionRange &label, uint32_t labelAddress)
{
	if (!inSection) return;
	
	currentLabel.instructions = currentArray;
	NSString* uniqueName = [NSString stringWithFormat:@"%.32s%08x", sectionMD5, labelAddress - sectionBase];
	uint32_t length = (label.End - label.Begin) * sizeof(Common::UInt32);
	currentArray = [NSMutableArray array];
	currentLabel = [CXCodeLabel codeLabelWithAddress:labelAddress length:length uniqueName:uniqueName];
	currentLabel.isFunction = label.IsFunction;
	[result addObject:currentLabel];
}

void CXObjCDisassemblyWriter::VisitOpcode(const PPCVM::Disassembly::DisassembledOpcode &opcode, uint32_t opcodeAddress, const PPCVM::Disassembly::SectionDisassembler::MetadataMap::mapped_type* metadata)
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
	
	NSNumber* meta = nil;
	if (metadata != nullptr)
		meta = @(*metadata);
	
	NSDictionary* objcOpcode = @{
		@"location": @(opcodeAddress),
		@"code": @(opcode.Instruction.hex),
		@"opcode": @(opcode.Opcode.c_str()),
		@"target": meta ? meta : NSNull.null,
		@"arguments": arguments
	};
	
	[currentArray addObject:objcOpcode];
}

NSArray* CXObjCDisassemblyWriter::GetDisassembly()
{
	if (currentLabel != nil)
	{
		[result addObject:currentLabel];
		currentLabel = nil;
	}
	
	// this can be necessary, as the CXObjCDisassemblyWriter will release `result` as it goes
	// out of scope
	return [[result retain] autorelease];
}

CXObjCDisassemblyWriter::~CXObjCDisassemblyWriter()
{
	[result release];
}
