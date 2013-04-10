//
// GrafPortManager.h
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

#ifndef __Classix__GrafPortManager__
#define __Classix__GrafPortManager__

#include <unordered_map>
#include "IAllocator.h"
#include "CommonDefinitions.h"

// I don't want to #include <IOSurface/IOSurface.h> here because it brings
// tons of ambiguous QuickDraw definitions
typedef struct __IOSurface* IOSurfaceRef;

namespace InterfaceLib
{
	class GrafPortData;
	
	class GrafPortManager
	{
		Common::IAllocator& allocator;
		std::unordered_map<uint32_t, GrafPortData> ports;
		GrafPortData* currentPort;
		
	public:
		GrafPortManager(Common::IAllocator& allocator);
		
		InterfaceLib::GrafPort& AllocateGrafPort(uint32_t width, uint32_t height, const std::string& allocationName = "");
		void InitializeGrafPort(InterfaceLib::GrafPort& port, uint32_t width, uint32_t height);
		
		void SetCurrentPort(GrafPort& port);
		GrafPort& GetCurrentPort();
		
		IOSurfaceRef SurfaceOfGrafPort(InterfaceLib::GrafPort& port);
		
		// this does not deallocate 'port', but it gets rid of the IOSurface
		void DestroyGrafPort(InterfaceLib::GrafPort& port);
		
		~GrafPortManager();
	};
}

#endif /* defined(__Classix__GrafPortManager__) */
