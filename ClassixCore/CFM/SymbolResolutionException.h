//
//  SymbolResolutionException.h
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__SymbolResolutionException__
#define __pefdump__SymbolResolutionException__

#include <string>
#include <exception>

namespace CFM
{
	class SymbolResolutionException : public virtual std::exception
	{
		std::string message;
		
	public:
		SymbolResolutionException(const std::string& libName, const std::string& symbolName);
		
		virtual const char* what() const noexcept;
		virtual ~SymbolResolutionException();
	};
}

#endif /* defined(__pefdump__SymbolResolutionException__) */
