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
	PEFLibraryResolver::PEFLibraryResolver(FragmentManager& manager)
	: cfm(manager)
	{ }
	
	SymbolResolver* PEFLibraryResolver::ResolveLibrary(const std::string &name)
	{
		FileDescriptor file(name);
		return file.fd < 0 ? nullptr : new PEFSymbolResolver(cfm, FileMapping(file.fd));
	}
	
	PEFLibraryResolver::~PEFLibraryResolver()
	{ }
}