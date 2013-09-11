//
// NativeLibrary.h
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

#ifndef __Classix__NativeLibrary__
#define __Classix__NativeLibrary__

#include "Allocator.h"
#include "SymbolType.h"
#include "MachineState.h"
#include "Managers.h"

namespace ClassixCore
{
	class NativeLibrary
	{
	public:
		typedef void* (*OnLoadFunction)(Common::Allocator*, OSEnvironment::Managers* managers);
		typedef SymbolType (*LookupFunction)(void*, const char*, void**);
		typedef void (*OnUnloadFunction)(void*);
		
		typedef void (*ExportedFunction)(void*, PPCVM::MachineState*);
		
		static const std::string OnLoadFunctionName;
		static const std::string LookupFunctionName;
		static const std::string OnUnloadFunctionName;
		
		std::string Path;
		std::string Name;
		OnLoadFunction OnLoad;
		LookupFunction Lookup;
		OnUnloadFunction OnUnload;
		const char** CodeSymbols;
		const char** DataSymbols;
		
		NativeLibrary() = default;
		NativeLibrary(const NativeLibrary& that) = delete;
		
		virtual ~NativeLibrary() = 0;
	};
	
	inline NativeLibrary::~NativeLibrary() {}
}

#endif /* defined(__Classix__NativeLibrary__) */
