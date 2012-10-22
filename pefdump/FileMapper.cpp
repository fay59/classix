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

#include "FileMapper.h"

FileMapper::FileMapper(const std::string& path)
{
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		throw std::logic_error(strerror(errno));
	
	fileSize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	address = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	
	if (address == MAP_FAILED)
		throw std::logic_error(strerror(errno));
}

void* FileMapper::begin()
{
	return address;
}

void* FileMapper::end()
{
	return static_cast<uint8_t*>(address) + fileSize;
}

FileMapper::~FileMapper()
{
	munmap(address, fileSize);
}
