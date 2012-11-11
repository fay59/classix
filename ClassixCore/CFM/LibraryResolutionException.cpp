//
//  LibraryResolutionException.cpp
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "LibraryResolutionException.h"

namespace CFM
{
	LibraryResolutionException::LibraryResolutionException(const std::string& libName)
	{
		message = "Cannot load " + libName;
	}
	
	const char* LibraryResolutionException::what() const noexcept
	{
		return message.c_str();
	}
	
	LibraryResolutionException::~LibraryResolutionException()
	{ }
}
