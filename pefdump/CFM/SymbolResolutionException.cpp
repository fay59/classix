//
//  SymbolResolutionException.cpp
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "SymbolResolutionException.h"

namespace CFM
{
	SymbolResolutionException::SymbolResolutionException(const std::string& libName, const std::string& symbolName)
	{
		message = "Cannot find symbol " + symbolName + " in " + libName;
	}
	
	const char* SymbolResolutionException::what() const noexcept
	{
		return message.c_str();
	}
	
	SymbolResolutionException::~SymbolResolutionException()
	{ }
}
