//
//  FileMapper.cpp
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "FileMapping.h"

namespace CFM
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

	void* FileMapping::begin()
	{
		return address;
	}

	void* FileMapping::end()
	{
		return static_cast<uint8_t*>(address) + fileSize;
	}

	FileMapping::~FileMapping()
	{
		munmap(address, static_cast<size_t>(fileSize));
	}
}
