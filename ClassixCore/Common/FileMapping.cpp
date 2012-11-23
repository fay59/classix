//
// FileMapping.cpp
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

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "FileMapping.h"

namespace Common
{
	FileMapping::FileMapping(const std::string& path)
	{
		int fd = open(path.c_str(), O_RDONLY);
		if (fd == -1)
			throw std::logic_error(strerror(errno));
		
		fileSize = lseek(fd, 0, SEEK_END);
		address = mmap(nullptr, static_cast<size_t>(fileSize), PROT_READ, MAP_PRIVATE, fd, 0);
		close(fd);
		
		if (address == MAP_FAILED)
			throw std::logic_error(strerror(errno));
	}
	
	FileMapping::FileMapping(int fd)
	{
		if (fd < 0)
			throw new std::logic_error("file descriptor is not open");
		
		long long currentPosition = lseek(fd, 0, SEEK_CUR);
		fileSize = lseek(fd, 0, SEEK_END);
		lseek(fd, currentPosition, SEEK_SET);
		
		address = mmap(nullptr, static_cast<size_t>(fileSize), PROT_READ, MAP_PRIVATE, fd, 0);
		if (address == MAP_FAILED)
			throw std::logic_error(strerror(errno));
	}
	
	FileMapping::FileMapping(FileMapping&& that)
	{
		address = that.address;
		fileSize = that.fileSize;
		that.address = nullptr;
		that.fileSize = 0;
	}
	
	long long FileMapping::size() const
	{
		return fileSize;
	}

	void* FileMapping::begin()
	{
		return address;
	}

	void* FileMapping::end()
	{
		return static_cast<uint8_t*>(address) + fileSize;
	}
	
	const void* FileMapping::begin() const
	{
		return address;
	}
	
	const void* FileMapping::end() const
	{
		return static_cast<const uint8_t*>(address) + fileSize;
	}

	FileMapping::~FileMapping()
	{
		munmap(address, static_cast<size_t>(fileSize));
	}
}
