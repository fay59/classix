//
// Structures.cpp
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

#include "Structures.h"

namespace
{
	template<typename T, size_t N>
	size_t arrayCount(T(&)[N])
	{
		return N;
	}
	
	const char* pefSectionTypes[] = {
		"Code",
		"Unpacked Data",
		"Pattern-Initialized Data",
		"Constant",
		"Loader",
		"Debug",
		"Executable Data",
		"Exception",
		"Traceback",
	};
	
	const char* pefShareTypes[] = {
		[(uint8_t)PEF::ShareType::ProcessShare] = "Process-Share",
		[(uint8_t)PEF::ShareType::GlobalShare] = "Global-Share",
		[(uint8_t)PEF::ShareType::ProtectedShare] = "Protected-Share",
	};
	
	const uint32_t macUNIXTimeOffset = 2082844800u;
	
	inline time_t MacTimeToUNIXTime(Common::UInt32 macDate)
	{
		return macDate - macUNIXTimeOffset;
	}
	
	inline Common::UInt32 UNIXTimeToMacTime(time_t unixDate)
	{
		return Common::UInt32(static_cast<uint32_t>(unixDate + macUNIXTimeOffset));
	}
	
	inline uint32_t MacTime()
	{
		return UNIXTimeToMacTime(time(NULL));
	}
	
	bool ValidateContainerHeader(const PEF::ContainerHeader* header)
	{
		using namespace PEF;
		
		if (header == nullptr)
			return false;
		
		if (header->Tag1 != ContainerHeader::Tag1ExpectedValue)
			return false;
		
		if (header->Tag2 != ContainerHeader::Tag2ExpectedValue)
			return false;
		
		if (header->Architecture != ContainerHeader::Architecture68k && header->Architecture != ContainerHeader::ArchitecturePowerPC)
			return false;
		
		if (header->FormatVersion != 1)
			return false;
		
		if (header->Reserved != 0)
			return false;
		
		return true;
	}
}

namespace PEF
{
	TypeTag::TypeTag()
	{
		numeric = 0;
	}
	
	TypeTag::TypeTag(uint32_t numeric)
	{
		this->numeric = numeric;
	}
	
	TypeTag::TypeTag(const char* ascii)
	{
		for (int i = 0; i < sizeof this->ascii; i++)
			this->ascii[i] = ascii[i];
	}
	
	std::ostream& operator<<(std::ostream& out, const TypeTag& tag)
	{
		out.write(tag.ascii, 4);
		return out;
	}
	
	std::ostream& operator<<(std::ostream& out, const SectionType type)
	{
		if ((size_t)type >= arrayCount(pefSectionTypes))
			throw std::logic_error("unknown section type");
		
		out << pefSectionTypes[(size_t)type];
		return out;
	}
	
	std::ostream& operator<<(std::ostream& out, const ShareType type)
	{
		if ((size_t)type >= arrayCount(pefShareTypes))
			throw std::logic_error("unknown share type");
		
		const char* typeName = pefShareTypes[(size_t)type];
		if (typeName == nullptr)
			throw std::logic_error("unknown share type");
		
		out << typeName;
		return out;
	}
	
#pragma mark -
#pragma mark ContainerHeader
	const TypeTag ContainerHeader::Tag1ExpectedValue = "Joy!";
	const TypeTag ContainerHeader::Tag2ExpectedValue = "peff";
	const TypeTag ContainerHeader::ArchitecturePowerPC = "pwpc";
	const TypeTag ContainerHeader::Architecture68k = "m68k";
	
	ContainerHeader* ContainerHeader::FromPointer(void* ptr)
	{
		ContainerHeader* result = static_cast<ContainerHeader*>(ptr);
		return ValidateContainerHeader(result) ? result : nullptr;
	}
	
	const ContainerHeader* ContainerHeader::FromPointer(const void* ptr)
	{
		const ContainerHeader* result = static_cast<const ContainerHeader*>(ptr);
		return ValidateContainerHeader(result) ? result : nullptr;
	}
	
	ContainerHeader::ContainerHeader()
	{
		Tag1 = Tag1ExpectedValue;
		Tag2 = Tag2ExpectedValue;
		Architecture = Architecture68k;
		FormatVersion = 1;
		DateTimeStamp = MacTime();
		
		OldDefVersion = 0;
		OldImpVersion = 0;
		CurrentVersion = 0;
		
		SectionCount = 0;
		InstSectionCount = 0;
		
		Reserved = 0;
	}
	
	time_t ContainerHeader::GetCreationTime() const
	{
		return MacTimeToUNIXTime(DateTimeStamp);
	}
	
	void ContainerHeader::SetCreationTime(time_t time)
	{
		DateTimeStamp = UNIXTimeToMacTime(time);
	}
	
#pragma mark -
#pragma mark Sectionheader
	SectionHeader* SectionHeader::FromContainer(PEF::ContainerHeader *header, uint16_t sectionIndex)
	{
		if (sectionIndex >= header->SectionCount)
			return nullptr;
		
		uint8_t* asBytes = reinterpret_cast<uint8_t*>(header);
		uint8_t* sectionHeader = asBytes + sizeof(ContainerHeader) + sizeof(SectionHeader) * sectionIndex;
		return reinterpret_cast<SectionHeader*>(sectionHeader);
	}
	
	const SectionHeader* SectionHeader::FromContainer(const PEF::ContainerHeader *header, uint16_t sectionIndex)
	{
		if (sectionIndex >= header->SectionCount)
			return nullptr;
		
		const uint8_t* asBytes = reinterpret_cast<const uint8_t*>(header);
		const uint8_t* sectionHeader = asBytes + sizeof(ContainerHeader) + sizeof(SectionHeader) * sectionIndex;
		return reinterpret_cast<const SectionHeader*>(sectionHeader);
	}
	
	SectionHeader::SectionHeader()
	{
		NameOffset = 0;
		DefaultAddress = 0;
		ExecutionSize = 0;
		UnpackedSize = 0;
		PackedSize = 0;
		ContainerOffset = 0;
		SectionType = SectionType::Code;
		ShareType = ShareType::ProcessShare;
		Alignment = 1;
		Reserved = 0;
	}
	
#pragma mark -
#pragma mark LoaderHeader
	LoaderHeader::SectionWithOffset::SectionWithOffset()
	{
		Section = 0;
		Offset = 0;
	}
	
	LoaderHeader* LoaderHeader::FromSectionHeader(const PEF::SectionHeader *header, void *base)
	{
		if (header->SectionType != SectionType::Loader)
			return nullptr;
		
		uint8_t* asBytes = reinterpret_cast<uint8_t*>(base);
		uint8_t* sectionHeader = asBytes + header->ContainerOffset;
		return reinterpret_cast<LoaderHeader*>(sectionHeader);
	}
	
	const LoaderHeader* LoaderHeader::FromSectionHeader(const PEF::SectionHeader *header, const void *base)
	{
		if (header->SectionType != SectionType::Loader)
			return nullptr;
		
		const uint8_t* asBytes = reinterpret_cast<const uint8_t*>(base);
		const uint8_t* sectionHeader = asBytes + header->ContainerOffset;
		return reinterpret_cast<const LoaderHeader*>(sectionHeader);
	}
}
