//
//  FileMapper.h
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__FileMapper__
#define __pefdump__FileMapper__

#include <string>

namespace CFM
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
		
		void* begin();
		void* end();
		
		FileMapping();
	};
}

#endif /* defined(__pefdump__FileMapper__) */
