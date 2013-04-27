//
// UIChannel.cpp
// Classix
//
// Copyright (C) 2013 Félix Cloutier
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

#include <CoreFoundation/CoreFoundation.h>
#include <sstream>

#include "UIChannel.h"

namespace
{
	std::string CFStringToStdString(CFStringRef string, CFStringEncoding encoding = kCFStringEncodingMacRoman) noexcept
	{
		if (const char* ptr = CFStringGetCStringPtr(string, encoding))
			return ptr;
		
		size_t length = CFStringGetLength(string) + 1;
		char cString[length];
		if (CFStringGetCString(string, cString, length, encoding))
			return cString;
		
		throw std::logic_error("Could not convert CFStringRef to std::string");
	}
	
	std::string CFURLToStdString(CFURLRef url) noexcept
	{
		CFStringRef path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
		std::string&& result = CFStringToStdString(path);
		CFRelease(path);
		return result;
	}
	
	void LaunchHead(int readPipe, int writePipe) noexcept
	{
		CFBundleRef bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.felixcloutier.InterfaceLib"));
		CFURLRef url = CFBundleCopyResourceURL(bundle, CFSTR("InterfaceLibHead"), CFSTR("app"), nullptr);
		
		std::stringstream processPath;
		std::stringstream readFd;
		std::stringstream writeFd;
		readFd << readPipe;
		writeFd << writePipe;
		processPath << CFURLToStdString(url) << "/Contents/MacOS/InterfaceLibHead";
		execl(processPath.str().c_str(),
			  processPath.str().c_str(), readFd.str().c_str(), writeFd.str().c_str(), nullptr);
	}
}

namespace InterfaceLib
{
	UIChannel::UIChannel()
	{
		if (pipe(read.fd) == -1)
			throw std::runtime_error(strerror(errno));
		
		if (pipe(write.fd) == -1)
		{
			close(read.read);
			close(read.write);
			throw std::runtime_error(strerror(errno));
		}
		
		head = fork();
		if (head == -1)
		{
			close(read.read);
			close(read.write);
			close(write.read);
			close(write.write);
			throw std::runtime_error("Failed to fork the head process");
		}
		else if (head == 0)
		{
			close(write.write);
			close(read.read);
			LaunchHead(write.read, read.write);
		}
		else
		{
			close(write.read);
			close(read.write);
		}
	}
	
	template<>
	void UIChannel::ReturnNonVoid(const uint8_t* buffer) {}
	
	template<>
	char UIChannel::WriteToPipe(const MacRegion& region)
	{
		WriteToPipe(region.rgnSize);
		WriteToPipe(region.rgnBBox);
		const char* bytes = reinterpret_cast<const char*>(&region) + sizeof region;
		::write(write.write, bytes, region.rgnSize - 10);
		return 0;
	}
	
	UIChannel::~UIChannel()
	{
		close(write.write);
		close(read.read);
	}
}
