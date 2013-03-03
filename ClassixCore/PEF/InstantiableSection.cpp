//
// InstantiableSection.cpp
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

#include "InstantiableSection.h"

#include <cstdlib>
#include <iostream>
#include <cassert>

namespace
{
	struct PatternInitOpcode
	{
		// : 8 tricks the debugger into thinking that they're not chars
		unsigned opcode : 8;
		unsigned arg : 8;
		
		PatternInitOpcode(uint8_t byte)
		{
			opcode = (byte >> 5) & 0b111;
			arg = byte & 0b11111;
		}
	};
	
	uint32_t ReadVariableLengthInteger(const uint8_t* input, uint32_t& output)
	{
		output = 0;
		bool readNextByte = true;
		const uint8_t* data = input;
		while (readNextByte)
		{
			output <<= 7;
			output |= *data & 0x7f;
			readNextByte = *data >> 7;
			data++;
		}
		uint32_t consumedLength = static_cast<uint32_t>(data - input);
		assert(consumedLength <= 5 && "should never read more than 5 bytes");
		return consumedLength;
	}
	
	void ExecutePattern(const uint8_t* pattern, uint32_t packedSize, uint8_t* output, uint32_t outputSize)
	{
		const uint8_t* input = pattern;
		const uint8_t* inputEnd = input + packedSize;
		uint8_t* outputEnd = output + outputSize;
		while (input < inputEnd && output < outputEnd)
		{
			PatternInitOpcode operation = *input;
			input++;
			
			uint8_t opcode = operation.opcode;
			uint32_t argument;
			if (operation.arg == 0)
				input += ReadVariableLengthInteger(input, argument);
			else
				argument = operation.arg;
			
			const uint8_t* initialInputPosition = input;
			const uint8_t* initialOutputPosition = output;
			switch (opcode)
			{
				case 0b000: // zero
				{
					//std::cerr << "zeroing " << argument << " bytes" << std::endl;
					memset(output, 0, argument);
					output += argument;
					
					assert(input == initialInputPosition && "pattern was not correctly interpreted");
					assert(output == initialOutputPosition + argument && "output was not correctly written");
					break;
				}
					
				case 0b001: // block copy
				{
					//std::cerr << "copying " << argument << " bytes" << std::endl;
					memcpy(output, input, argument);
					output += argument;
					input += argument;
					
					assert(input == initialInputPosition + argument && "pattern was not correctly interpreted");
					assert(output == initialOutputPosition + argument && "output was not correctly written");
					break;
				}
					
				case 0b010: // repeated block
				{
					uint32_t repeatCount;
					uint32_t consumedLength = ReadVariableLengthInteger(input, repeatCount);
					input += consumedLength;
					
					//std::cerr << "repeating " << argument << " bytes " << repeatCount << " times" << std::endl;
					for (uint32_t i = 0; i < repeatCount + 1; i++)
					{
						memcpy(output, input, argument);
						output += argument;
					}
					input += argument;
					
					assert(input == initialInputPosition + argument && "pattern was not correctly interpreted");
					assert(output == initialOutputPosition + argument * (repeatCount + 1) && "output was not correctly written");
					break;
				}
					
				case 0b011: // interleave repeat block with block copy
				{
					uint32_t commonSize = argument;
					uint32_t customSize;
					uint32_t repeatCount;
					uint32_t consumedLength = 0;
					consumedLength += ReadVariableLengthInteger(input, customSize);
					consumedLength += ReadVariableLengthInteger(input + consumedLength, repeatCount);
					input += consumedLength;
					
					const uint8_t* commonData = input;
					input += commonSize;
					//std::cerr << "interleaving " << customSize << " custom bytes with " << argument << " common bytes, " << repeatCount << " times" << std::endl;
					
					for (uint32_t i = 0; i < repeatCount; i++)
					{
						memcpy(output, commonData, commonSize);
						output += commonSize;
						memcpy(output, input, customSize);
						output += customSize;
						input += customSize;
					}
					memcpy(output, commonData, commonSize);
					output += commonSize;
					
					const uint8_t* inputFinal = initialInputPosition + consumedLength + commonSize + (customSize * repeatCount);
					const uint8_t* outputFinal = initialOutputPosition + (commonSize + customSize) * repeatCount + commonSize;
					assert(input == inputFinal && "pattern was not correctly interpreted");
					assert(output == outputFinal && "output was not correctly written");
					break;
				}
					
				case 0b100: // interleave repeat block with zero
				{
					uint32_t zeroSize = argument;
					uint32_t customSize;
					uint32_t repeatCount;
					uint32_t consumedLength = 0;
					consumedLength += ReadVariableLengthInteger(input, customSize);
					consumedLength += ReadVariableLengthInteger(input + consumedLength, repeatCount);
					input += consumedLength;
					
					//std::cerr << "interleaving " << customSize << " custom bytes with " << argument << " zeroed bytes, " << repeatCount << " times" << std::endl;
					for (uint32_t i = 0; i < repeatCount; i++)
					{
						memset(output, 0, zeroSize);
						output += zeroSize;
						memcpy(output, input, customSize);
						output += customSize;
						input += customSize;
					}
					memset(output, 0, zeroSize);
					output += zeroSize;
					
					assert(input == initialInputPosition + consumedLength + customSize * repeatCount && "pattern was not correctly interpreted");
					assert(output == initialOutputPosition + (zeroSize + customSize) * repeatCount + zeroSize && "output was not correctly written");
					break;
				}
					
				default:
					throw std::logic_error("unknown opcode in ExecutePattern");
			}
		}
		
		assert(output == outputEnd && "Pattern did not fill whole section");
		
		if (input != inputEnd)
			throw std::logic_error("pattern should execute exactly to the pattern boundaries");
	}
}

namespace PEF
{
	InstantiableSection::InstantiableSection(Common::IAllocator& allocator, const SectionHeader* header, const std::string& name, const uint8_t* base, const uint8_t* end)
	: allocator(allocator)
	{
		uint32_t packedSize = header->PackedSize;
		uint32_t unpackedSize = header->UnpackedSize;
		uint32_t totalSize = header->ExecutionSize;
		const uint8_t* sectionContent = base + header->ContainerOffset;
		
		if (sectionContent + packedSize > end)
			throw std::logic_error("section offset goes past to end of file");
		
		this->header = header;
		Name = name;
		Data = allocator.Allocate(name, totalSize);
		
		switch (GetSectionType())
		{
			case SectionType::Code:
			case SectionType::Constant:
			case SectionType::UnpackedData:
			case SectionType::ExecutableData:
			{
				//assert((2 << header->Alignment) % 16 == 0 && "Content should be aligned on a minimum 16 bytes boundary");
				memcpy(Data, sectionContent, totalSize);
				break;
			}
				
			case SectionType::PatternInitializedData:
			{
				//assert((2 << header->Alignment) % 4 == 0 && "Content should be aligned on a minimum 16 bytes boundary");
				ExecutePattern(sectionContent, packedSize, Data, unpackedSize);
				memset(Data + unpackedSize, 0, totalSize - unpackedSize);
				break;
			}
				
			default:
			{
				free(Data);
				throw std::logic_error("Unknown or uninstantiable section type");
			}
		}
	}
	
	InstantiableSection::InstantiableSection(InstantiableSection&& that)
	: Name(std::move(that.Name)), allocator(that.allocator)
	{
		header = that.header;
		Data = that.Data;
		that.Data = nullptr;
	}
	
	bool InstantiableSection::IsExecutable() const
	{
		switch (GetSectionType())
		{
			case SectionType::Code:
			case SectionType::ExecutableData:
				return true;
				
			default:
				return false;
		}
	}
	
	SectionType InstantiableSection::GetSectionType() const
	{
		return this->header->SectionType;
	}
	
	ShareType InstantiableSection::GetShareType() const
	{
		return this->header->ShareType;
	}
	
	size_t InstantiableSection::Size() const
	{
		return header->ExecutionSize;
	}
	
	size_t InstantiableSection::AbsoluteOffset() const
	{
		return header->ContainerOffset;
	}
	
	uint32_t InstantiableSection::GetDataLocation() const
	{
		return allocator.ToIntPtr(Data);
	}
	
	InstantiableSection::~InstantiableSection()
	{
		allocator.Deallocate(Data);
	}
}
