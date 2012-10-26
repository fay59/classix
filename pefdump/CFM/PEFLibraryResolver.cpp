//
//  PEFLibraryResolver.cpp
//  pefdump
//
//  Created by Félix on 2012-10-22.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "PEFLibraryResolver.h"
#include "PEFSymbolResolver.h"
#include <unistd.h>
#include <sys/fcntl.h>

namespace
{
	struct FileDescriptor
	{
		int fd;
		
		FileDescriptor(const std::string& file)
		{
			fd = open(file.c_str(), O_RDONLY);
		}
		
		~FileDescriptor()
		{
			close(fd);
		}
	};
}

namespace CFM
{
	PEFLibraryResolver::PEFLibraryResolver(PPCVM::MemoryManager& memMan, FragmentManager& manager)
	: cfm(manager)
	, memoryManager(memMan)
	{ }
	
	SymbolResolver* PEFLibraryResolver::ResolveLibrary(const std::string &name)
	{
		FileDescriptor file(name);
		if (file.fd < 0)
			return nullptr;
		
		Common::FileMapping mapping(file.fd);
		return new PEFSymbolResolver(memoryManager, cfm, std::move(mapping));
	}
	
	PEFLibraryResolver::~PEFLibraryResolver()
	{ }
}