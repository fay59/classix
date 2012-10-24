//
//  PEFContaierHeader.h
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__PEFContaierHeader__
#define __pefdump__PEFContaierHeader__

#include <cstdint>
#include <ctime>
#include <iostream>
#include "BigEndian.h"

namespace PEF
{
	using namespace BigEndian;
	
	union TypeTag
	{
		uint32_t numeric;
		char ascii[4];
		
		TypeTag();
		TypeTag(uint32_t numeric);
		TypeTag(const char* ascii);
		
		inline bool operator==(const TypeTag& that) const
		{
			return this->numeric == that.numeric;
		}
		
		inline bool operator!=(const TypeTag& that) const
		{
			return !(*this == that);
		}
	};
	
	enum class SectionType : uint8_t
	{
		Code					= 0, // read-only executable
		UnpackedData			= 1, // uncompressed read-write data
		PatternInitializedData	= 2, // read-write data initialized by the code in the section
		Constant				= 3, // uncompressed read-only data
		Loader					= 4, // loader directives
		Debug					= 5, // reserved
		ExecutableData			= 6, // read-write executable
		Exception				= 7, // reserved
		Traceback				= 8, // reserved
	};
	
	enum class ShareType : uint8_t
	{
		ProcessShare			= 1, // private
		GlobalShare				= 4, // public
		ProtectedShare			= 5, // read-only for all processes except for privileged code
	};
		
	std::ostream& operator<<(std::ostream& out, const TypeTag& tag);
	std::ostream& operator<<(std::ostream& out, SectionType type);
	std::ostream& operator<<(std::ostream& out, ShareType type);
	
	struct ContainerHeader
	{
		static const TypeTag Tag1ExpectedValue;
		static const TypeTag Tag2ExpectedValue;
		static const TypeTag ArchitecturePowerPC;
		static const TypeTag Architecture68k;
		
		// methods
		static ContainerHeader* FromPointer(void* pointer);
		static const ContainerHeader* FromPointer(const void* pointer);
		
		ContainerHeader();
		
		time_t GetCreationTime() const;
		void SetCreationTime(time_t time);
		
		TypeTag Tag1; // 'Joy!'
		TypeTag Tag2; // 'peff'
		TypeTag Architecture; // 'pwpc' or 'm68k'
		
		UInt32 FormatVersion; // 1
		
		UInt32 DateTimeStamp; // seconds since Jan 1, 1904; use (Get|Set)CreationTime to have UNIX time stamps
		
		// fragment manager stuff
		UInt32 OldDefVersion;
		UInt32 OldImpVersion;
		UInt32 CurrentVersion;
		
		UInt16 SectionCount;
		UInt16 InstSectionCount;
		
		UInt32 Reserved; // always 0
	} __attribute__((packed));
	
	struct SectionHeader
	{
		// methods
		static SectionHeader* FromContainer(ContainerHeader* header, uint16_t sectionIndex);
		static const SectionHeader* FromContainer(const ContainerHeader* file, uint16_t sectionIndex);
		
		SectionHeader();
		
		SInt32 NameOffset; // offset to name from name table, or -1 if no name
		UInt32 DefaultAddress; // preferred address
		UInt32 ExecutionSize; // the total size required by the section
		UInt32 UnpackedSize; // size of explicitly initialized data
		UInt32 PackedSize; // size in this file
		UInt32 ContainerOffset; // offset from beginning of file to contents
		
		SectionType SectionType;
		ShareType ShareType;
		uint8_t Alignment; // as a power to two
		uint8_t Reserved; // always 0
	} __attribute__((packed));
	
	struct LoaderHeader
	{
		struct SectionWithOffset
		{
			SInt32 Section;
			UInt32 Offset;
			
			SectionWithOffset();
			SectionWithOffset(int32_t section, uint32_t offset);
		};
		
		static LoaderHeader* FromSectionHeader(const SectionHeader* header, void* base);
		static const LoaderHeader* FromSectionHeader(const SectionHeader* header, const void* base);
		
		LoaderHeader();
		
		SectionWithOffset Main;
		SectionWithOffset Init;
		SectionWithOffset Term;
		
		UInt32 ImportedLibraryCount;
		UInt32 ImportedSymbolCount;
		UInt32 RelocSectionCount;
		UInt32 RelocInstructionOffset;
		UInt32 LoaderStringsOffset;
		UInt32 ExportHashOffset;
		UInt32 ExportHashTablePower;
		UInt32 ExportedSymbolCount;
	} __attribute__((packed));
		
	struct ImportedLibraryHeader
	{
		UInt32 NameOffset;
		UInt32 OldImpVersion;
		UInt32 CurrentVersion;
		UInt32 ImportedSymbolCount;
		UInt32 FirstImportedSymbol;
		uint8_t Options;
		uint8_t reservedA;
		UInt16 reservedB;
	} __attribute__((packed));
		
	struct SymbolFlags
	{
		enum Enum
		{
			Weak = 8,
		};
	};
	
	struct SymbolClasses
	{
		enum Enum
		{
			CodeSymbol		= 0,
			DataSymbol		= 1,
			FunctionPointer	= 2,
			DirectData		= 3,
			GlueSymbol		= 4
		};
	};
	
	union SymbolClass
	{
		uint8_t Value;
		struct
		{
			unsigned Class : 4;
			unsigned Flags : 4;
		};
		
		inline bool HasFlag(SymbolFlags::Enum flag) const
		{
			return (Flags & flag) == flag;
		}
	};
	
	struct ImportedSymbolHeader
	{
		UInt32 Entry;
		
		inline SymbolClass GetClass() const
		{
			SymbolClass symClass;
			symClass.Value = Entry.AsBigEndian & 0xff;
			return symClass;
		}
		
		inline uint32_t GetNameOffset() const
		{
			return UInt32(Entry.AsBigEndian & 0xffffff00);
		}
	} __attribute__((packed));
		
	struct RelocationHeader
	{
		UInt16 SectionIndex;
		UInt16 Reserved;
		UInt32 RelocationCount;
		UInt32 FirstRelocationOffset;
	} __attribute__((packed));
}

#endif /* defined(__pefdump__PEFContaierHeader__) */
