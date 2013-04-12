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

// I don't want to #include IOSurface or CoreGraphics here because it brings
// tons of ambiguous QuickDraw definitions
typedef struct __IOSurface* IOSurfaceRef;
typedef struct CGContext* CGContextRef;

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
		
		InterfaceLib::GrafPort& AllocateGrayGrafPort(const InterfaceLib::Rect& bounds, const std::string& allocationName = "");
		InterfaceLib::CGrafPort& AllocateColorGrafPort(const InterfaceLib::Rect& bounds, const std::string& allocationName = "");
		
		void InitializeGrayGrafPort(InterfaceLib::UGrafPort& port, const InterfaceLib::Rect& bounds);
		void InitializeColorGrafPort(InterfaceLib::UGrafPort& port, const InterfaceLib::Rect& bounds);
		
		void SetCurrentPort(InterfaceLib::UGrafPort& port);
		InterfaceLib::UGrafPort& GetCurrentPort();
		
		CGContextRef ContextOfGrafPort(InterfaceLib::UGrafPort& port);
		IOSurfaceRef SurfaceOfGrafPort(InterfaceLib::UGrafPort& port);
		
		// this does not deallocate 'port', but it gets rid of the IOSurface
		void DestroyGrafPort(InterfaceLib::UGrafPort& port);
		
		~GrafPortManager();
	};
}

#endif /* defined(__Classix__GrafPortManager__) */
