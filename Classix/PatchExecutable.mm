//
// PatchExecutable.m
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

// This file was made distinct from main.cpp because it uses Objective-C code,
// and this is not something I'm proud of, as I hoped most of Classix would
// run easily on any POSIX system. Sadly, setting the creator code and file type
// of the copied file seems like a very important step to get the executable to
// run on a real Mac OS 9 computer.

#import <Cocoa/Cocoa.h>

#include <string>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

#include "FragmentManager.h"
#include "PEFLibraryResolver.h"
#include "DummyLibraryResolver.h"
#include "NativeAllocator.h"
#include "Instruction.h"

static void CopyCreatorCode(const std::string& path, const std::string& outPath)
{
	@autoreleasepool
	{
		NSString* sourcePath = [NSString stringWithCString:path.c_str() encoding:NSUTF8StringEncoding];
		NSString* targetPath = [NSString stringWithCString:outPath.c_str() encoding:NSUTF8StringEncoding];
		NSFileManager* fs = [NSFileManager defaultManager];
		
		if (NSDictionary* result = [fs attributesOfItemAtPath:sourcePath error:nullptr])
		{
			NSDictionary* attributes = @{
				NSFileHFSCreatorCode: [result objectForKey:NSFileHFSCreatorCode],
				NSFileHFSTypeCode: [result objectForKey:NSFileHFSTypeCode]
			};
			
			[fs setAttributes:attributes ofItemAtPath:targetPath error:nullptr];
		}
	}
}

int patchExecutable(const std::string& path, const std::string& outPath)
{
	Common::NativeAllocator allocator;
	CFM::FragmentManager fragmentManager;
	CFM::PEFLibraryResolver pefResolver(allocator, fragmentManager);
	CFM::DummyLibraryResolver dummyResolver;
	
	fragmentManager.LibraryResolvers.push_back(&pefResolver);
	fragmentManager.LibraryResolvers.push_back(&dummyResolver);
	
	if (!fragmentManager.LoadContainer(path))
	{
		std::cerr << "Couldn't load " << path << std::endl;
		return -4;
	}
	
	CFM::PEFSymbolResolver* resolver = dynamic_cast<CFM::PEFSymbolResolver*>(fragmentManager.GetSymbolResolver(path));
	if (resolver == nullptr)
	{
		std::cerr << "Loaded executable isn't a PowerPC executable" << std::endl;
		return -3;
	}
	
	auto entryPoints = resolver->GetEntryPoints();
	Common::UInt32* transitionVectorAddress = nullptr;
	for (const auto& entryPoint : entryPoints)
	{
		if (entryPoint.Name == CFM::SymbolResolver::MainSymbolName)
		{
			if (entryPoint.Universe != CFM::SymbolUniverse::PowerPC)
			{
				std::cerr << "Entry point isn't a PowerPC entry point" << std::endl;
				return -3;
			}
			
			transitionVectorAddress = allocator.ToPointer<Common::UInt32>(entryPoint.Address);
			break;
		}
	}
	
	if (transitionVectorAddress == nullptr)
	{
		std::cerr << "Couldn't find any main transition vector" << std::endl;
		return -5;
	}
	
	PPCVM::Instruction* instructions = allocator.ToPointer<PPCVM::Instruction>(*transitionVectorAddress);
	PPCVM::Instruction* branch = instructions + 12;
	if (branch->hex == Common::UInt32::FromBigEndian(0x41820010)) // beq 0x10
	{
		// find the actual offset
		int sectionIndex = -1;
		ptrdiff_t offset = 0;
		PEF::Container& container = resolver->GetContainer();
		for (uint32_t i = 0; i < container.size(); i++)
		{
			const PEF::InstantiableSection& section = container.GetSection(i);
			const uint8_t* branchAddress = reinterpret_cast<uint8_t*>(branch);
			if (branchAddress > section.Data && branchAddress < section.Data + section.Size())
			{
				sectionIndex = i;
				offset = branchAddress - section.Data;
				break;
			}
		}
		
		if (sectionIndex == -1)
		{
			std::cerr << "couldn't find the section of the main function" << std::endl;
			return -1;
		}
		
		PPCVM::Instruction nopBuilder = 0;
		nopBuilder.OPCD = 24;
		Common::UInt32 nop = Common::UInt32(nopBuilder.hex);
		
		off_t beqLocation = container.GetSection(sectionIndex).AbsoluteOffset() + offset;
		int fd = open(outPath.c_str(), O_CREAT | O_TRUNC | O_WRONLY);
		if (fd < 0)
		{
			perror("open");
			return -1;
		}
		
		Common::FileMapping mapping(path);
		if (write(fd, mapping.begin(), static_cast<size_t>(mapping.size())) < 0)
		{
			perror("write");
			close(fd);
			return -1;
		}
		
		if (lseek(fd, beqLocation, SEEK_SET) < 0)
		{
			perror("lseek");
			close(fd);
			return -1;
		}
		
		if (write(fd, &nop, sizeof nop) < 0)
		{
			perror("write");
			close(fd);
			return -1;
		}
		
		close(fd);
		
		// also copy the resource fork=
		std::ifstream src(path + "/..namedfork/rsrc", std::ios_base::binary);
		std::ofstream dst(outPath + "/..namedfork/rsrc", std::ios_base::binary);
		dst << src.rdbuf();
		
		// set creator code + file type
		CopyCreatorCode(path, outPath);
		return 0;
	}
	
	std::cerr << "Unexpected instruction at this offset of the main symbol" << std::endl;
	return -2;
}