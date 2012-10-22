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

class FileMapper
{
	long fileSize;
	void* address;
	
public:
	FileMapper(const std::string& filePath);
	
	void* begin();
	void* end();
	
	~FileMapper();
};

#endif /* defined(__pefdump__FileMapper__) */
