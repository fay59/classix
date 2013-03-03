//
// PEFLibraryResolver.cpp
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

#include "PEFLibraryResolver.h"
#include "PEFSymbolResolver.h"
#include "LibraryResolutionException.h"
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
	PEFLibraryResolver::PEFLibraryResolver(Common::IAllocator& allocator, FragmentManager& manager)
	: cfm(manager)
	, allocator(allocator)
	{ }
	
	SymbolResolver* PEFLibraryResolver::ResolveLibrary(const std::string &name)
	{
		FileDescriptor file(name);
		if (file.fd < 0)
			return nullptr;
		
		Common::FileMapping mapping(file.fd);
		try
		{
			resolvers.emplace_back(allocator, cfm, std::move(mapping));
			return &resolvers.back();
		}
		catch (LibraryResolutionException& ex)
		{
			std::cerr << ex.what() << std::endl;
			return nullptr;
		}
	}
	
	PEFLibraryResolver::~PEFLibraryResolver()
	{ }
}