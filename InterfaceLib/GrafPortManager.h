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
typedef uint32_t IOSurfaceID;
typedef struct CGContext* CGContextRef;
typedef struct CGRect CGRect;

namespace InterfaceLib
{
	class GrafPortData;
	
	class GrafPortManager
	{
		Common::IAllocator& allocator;
		std::unordered_map<uint32_t, GrafPortData> ports;
		std::unordered_map<uint32_t, CGRect> updatedRegions;
		GrafPortData* currentPort;
		Palette* defaultPalette;
		
	public:
		GrafPortManager(Common::IAllocator& allocator);
		
		InterfaceLib::Palette& GetDefaultPalette();
		
		InterfaceLib::UGrafPort& AllocateGrayGrafPort(const InterfaceLib::Rect& bounds, const std::string& allocationName = "");
		InterfaceLib::UGrafPort& AllocateColorGrafPort(const InterfaceLib::Rect& bounds, const InterfaceLib::Palette* palette = nullptr, const std::string& allocationName = "");
		
		void InitializeGrayGrafPort(InterfaceLib::UGrafPort& port, const InterfaceLib::Rect& bounds);
		void InitializeColorGrafPort(InterfaceLib::UGrafPort& port, const InterfaceLib::Rect& bounds, const InterfaceLib::Palette* palette = nullptr);
		
		void SetCurrentPort(InterfaceLib::UGrafPort& port);
		InterfaceLib::UGrafPort& GetCurrentPort();
		
		InterfaceLib::Palette* PaletteOfGrafPort(InterfaceLib::UGrafPort& port);
		InterfaceLib::ColorTable* ColorTableOfGrafPort(InterfaceLib::UGrafPort& port);
		CGContextRef ContextOfGrafPort(InterfaceLib::UGrafPort& port);
		IOSurfaceID SurfaceOfGrafPort(InterfaceLib::UGrafPort& port);
		
		void BeginUpdate(InterfaceLib::UGrafPort& port);
		bool UpdateRegion(InterfaceLib::UGrafPort& port, CGRect region);
		CGRect EndUpdate(InterfaceLib::UGrafPort& port);
		
		// this does not deallocate 'port', but it gets rid of the IOSurface and the graphics context
		void DestroyGrafPort(InterfaceLib::UGrafPort& port);
		
		~GrafPortManager();
	};
}

#endif /* defined(__Classix__GrafPortManager__) */
