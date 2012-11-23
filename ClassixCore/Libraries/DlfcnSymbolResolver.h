//
// DlfcnSymbolResolver.h
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

#ifndef __Classix__DlfcnSymbolResolver__
#define __Classix__DlfcnSymbolResolver__

#include <string>
#include <unordered_map>
#include <list>
#include <deque>
#include "IAllocator.h"
#include "STAllocator.h"
#include "Structures.h"
#include "SymbolResolver.h"
#include "NativeCall.h"

namespace ClassixCore
{
	using namespace CFM;
	using namespace PPCVM::Execution;
	
	class DlfcnLibrary;
	
	class DlfcnSymbolResolver : public SymbolResolver
	{
		const DlfcnLibrary& library;
		void* globals;
		
		// symbol cache
		Common::IAllocator* allocator;
		Common::STAllocator<uint8_t> stlAllocator;
		std::list<PEF::TransitionVector, Common::STAllocator<PEF::TransitionVector>> transitions;
		std::deque<NativeCall, Common::STAllocator<NativeCall>> nativeCalls;
		std::unordered_map<std::string, ResolvedSymbol> symbols;
		
		ResolvedSymbol& CacheSymbol(const std::string& name, void* address);
		PEF::TransitionVector& MakeTransitionVector(const std::string& symbolName, void* address);
		
	public:
		DlfcnSymbolResolver(Common::IAllocator* allocator, const DlfcnLibrary& library);
		
		virtual ResolvedSymbol GetInitAddress();
		virtual ResolvedSymbol GetMainAddress();
		virtual ResolvedSymbol GetTermAddress();
		
		virtual ResolvedSymbol ResolveSymbol(const std::string& name);
		virtual ~DlfcnSymbolResolver();
	};
}

#endif /* defined(__Classix__DlfcnSymbolResolver__) */
