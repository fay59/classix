//
//  LibraryResolutionException.h
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__LibraryResolutionException__
#define __pefdump__LibraryResolutionException__

#include <string>
#include <exception>

namespace CFM
{
	class LibraryResolutionException : public virtual std::exception
	{
		std::string message;
		
	public:
		LibraryResolutionException(const std::string& libName);
		
		virtual const char* what() const noexcept;
		virtual ~LibraryResolutionException();
	};
}

#endif /* defined(__pefdump__LibraryResolutionException__) */
