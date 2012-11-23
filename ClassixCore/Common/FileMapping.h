//
// FileMapping.h
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

#ifndef __pefdump__FileMapper__
#define __pefdump__FileMapper__

#include <string>

namespace Common
{
	class FileMapping
	{
		long long fileSize;
		void* address;
		
	public:
		FileMapping(const std::string& filePath);
		FileMapping(const FileMapping& that) = delete;
		FileMapping(FileMapping&& that);
		FileMapping(int fd);
		
		long long size() const;
		
		void* begin();
		void* end();
		
		const void* begin() const;
		const void* end() const;
		
		~FileMapping();
	};
}

#endif /* defined(__pefdump__FileMapper__) */
