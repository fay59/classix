//
// GrafPortManager.cpp
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

#include <IOSurface/IOSurface.h>
#include <CoreGraphics/CoreGraphics.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include "GrafPortManager.h"
#include "CFOwningRef.h"

namespace
{
	using namespace InterfaceLib;
	
	template<typename T, size_t N>
	constexpr size_t ArraySize(T (&)[N]) { return N; }
	
	// according to a saved Photoshop Mac OS 9 palette file
	const uint32_t defaultColors[] = {
		0xFFFFFF, 0xFFFFCC, 0xFFFF99, 0xFFFF66, 0xFFFF33, 0xFFFF00, 0xFFCCFF, 0xFFCCCC,
		0xFFCC99, 0xFFCC66, 0xFFCC33, 0xFFCC00, 0xFF99FF, 0xFF99CC, 0xFF9999, 0xFF9966,
		0xFF9933, 0xFF9900, 0xFF66FF, 0xFF66CC, 0xFF6699, 0xFF6666, 0xFF6633, 0xFF6600,
		0xFF33FF, 0xFF33CC, 0xFF3399, 0xFF3366, 0xFF3333, 0xFF3300, 0xFF00FF, 0xFF00CC,
		0xFF0099, 0xFF0066, 0xFF0033, 0xFF0000, 0xCCFFFF, 0xCCFFCC, 0xCCFF99, 0xCCFF66,
		0xCCFF33, 0xCCFF00, 0xCCCCFF, 0xCCCCCC, 0xCCCC99, 0xCCCC66, 0xCCCC33, 0xCCCC00,
		0xCC99FF, 0xCC99CC, 0xCC9999, 0xCC9966, 0xCC9933, 0xCC9900, 0xCC66FF, 0xCC66CC,
		0xCC6699, 0xCC6666, 0xCC6633, 0xCC6600, 0xCC33FF, 0xCC33CC, 0xCC3399, 0xCC3366,
		0xCC3333, 0xCC3300, 0xCC00FF, 0xCC00CC, 0xCC0099, 0xCC0066, 0xCC0033, 0xCC0000,
		0x99FFFF, 0x99FFCC, 0x99FF99, 0x99FF66, 0x99FF33, 0x99FF00, 0x99CCFF, 0x99CCCC,
		0x99CC99, 0x99CC66, 0x99CC33, 0x99CC00, 0x9999FF, 0x9999CC, 0x999999, 0x999966,
		0x999933, 0x999900, 0x9966FF, 0x9966CC, 0x996699, 0x996666, 0x996633, 0x996600,
		0x9933FF, 0x9933CC, 0x993399, 0x993366, 0x993333, 0x993300, 0x9900FF, 0x9900CC,
		0x990099, 0x990066, 0x990033, 0x990000, 0x66FFFF, 0x66FFCC, 0x66FF99, 0x66FF66,
		0x66FF33, 0x66FF00, 0x66CCFF, 0x66CCCC, 0x66CC99, 0x66CC66, 0x66CC33, 0x66CC00,
		0x6699FF, 0x6699CC, 0x669999, 0x669966, 0x669933, 0x669900, 0x6666FF, 0x6666CC,
		0x666699, 0x666666, 0x666633, 0x666600, 0x6633FF, 0x6633CC, 0x663399, 0x663366,
		0x663333, 0x663300, 0x6600FF, 0x6600CC, 0x660099, 0x660066, 0x660033, 0x660000,
		0x33FFFF, 0x33FFCC, 0x33FF99, 0x33FF66, 0x33FF33, 0x33FF00, 0x33CCFF, 0x33CCCC,
		0x33CC99, 0x33CC66, 0x33CC33, 0x33CC00, 0x3399FF, 0x3399CC, 0x339999, 0x339966,
		0x339933, 0x339900, 0x3366FF, 0x3366CC, 0x336699, 0x336666, 0x336633, 0x336600,
		0x3333FF, 0x3333CC, 0x333399, 0x333366, 0x333333, 0x333300, 0x3300FF, 0x3300CC,
		0x330099, 0x330066, 0x330033, 0x330000, 0x00FFFF, 0x00FFCC, 0x00FF99, 0x00FF66,
		0x00FF33, 0x00FF00, 0x00CCFF, 0x00CCCC, 0x00CC99, 0x00CC66, 0x00CC33, 0x00CC00,
		0x0099FF, 0x0099CC, 0x009999, 0x009966, 0x009933, 0x009900, 0x0066FF, 0x0066CC,
		0x006699, 0x006666, 0x006633, 0x006600, 0x0033FF, 0x0033CC, 0x003399, 0x003366,
		0x003333, 0x003300, 0x0000FF, 0x0000CC, 0x000099, 0x000066, 0x000033, 0xEE0000,
		0xDD0000, 0xBB0000, 0xAA0000, 0x880000, 0x770000, 0x550000, 0x440000, 0x220000,
		0x110000, 0x00EE00, 0x00DD00, 0x00BB00, 0x00AA00, 0x008800, 0x007700, 0x005500,
		0x004400, 0x002200, 0x001100, 0x0000EE, 0x0000DD, 0x0000BB, 0x0000AA, 0x000088,
		0x000077, 0x000055, 0x000044, 0x000022, 0x000011, 0xEEEEEE, 0xDDDDDD, 0xBBBBBB,
		0xAAAAAA, 0x888888, 0x777777, 0x555555, 0x444444, 0x222222, 0x111111, 0x000000,
	};
	
	inline InterfaceLib::RGBColor ColorFromHex(uint32_t hex)
	{
		uint16_t r = (hex >> 8) & 0xff00;
		uint16_t g = hex & 0xff00;
		uint16_t b = (hex << 8) & 0xff00;
		RGBColor color = {
			.red = Common::UInt16(r),
			.green = Common::UInt16(g),
			.blue = Common::UInt16(b)
		};
		return color;
	}
	
	UGrafPort* AllocateGrafPort(Common::IAllocator& allocator, uint32_t width, uint32_t height, const std::string& allocationName)
	{
		std::stringstream ss;
		ss << "GrafPort <" << width << "x" << height << ">";
		if (allocationName.size() > 0)
		{
			ss << ": " << allocationName;
		}
		
		return allocator.Allocate<UGrafPort>(ss.str());
	}
	
	// we allocate at once all the memory that we need for all the parts of a CGrafPort
	struct ColorGrafPortEverythingElse
	{
		Common::UInt32 pixMapPointer;
		Common::UInt32 colorTablePointer;
		Common::UInt32 palettePointer;
		InterfaceLib::PixMap pixMap;
		InterfaceLib::ColorTable* colorTable;
		InterfaceLib::Palette* palette;
		
		ColorGrafPortEverythingElse(Common::IAllocator& allocator, const Palette* palette)
		{
			memset(this, 0, sizeof *this);
			
			std::string namePrefix;
			if (auto details = allocator.GetDetails(this))
			{
				std::stringstream colorTableName;
				colorTableName << details->GetAllocationDetails(0) << ": ";
				namePrefix = colorTableName.str();
			}
			else
			{
				namePrefix = "ColorGrafPortEverythingElse: ";
			}
			
			size_t paletteSize = sizeof(Palette) + sizeof(ColorInfo) * palette->pmEntries;
			uint8_t* paletteBytes = allocator.Allocate(namePrefix + "palette", paletteSize);
			memcpy(paletteBytes, palette, paletteSize);
			this->palette = reinterpret_cast<Palette*>(paletteBytes);
			
			size_t colorTableSize = sizeof(ColorTable) + sizeof(ColorSpec) * palette->pmEntries;
			uint8_t* colorTableBytes = allocator.Allocate(namePrefix + "color table", colorTableSize);
			
			colorTable = reinterpret_cast<ColorTable*>(colorTableBytes);
			colorTable->count = palette->pmEntries;
			
			for (int16_t i = 0; i < palette->pmEntries; i++)
			{
				ColorSpec& spec = colorTable->table[i];
				spec.value = i;
				spec.rgb = palette->pmInfo[i].ciRGB;
			}
			
			pixMapPointer = allocator.ToIntPtr(&pixMap);
			colorTablePointer = allocator.ToIntPtr(colorTable);
			palettePointer = allocator.ToIntPtr(palette);
			pixMap.pmTable = allocator.ToIntPtr(&colorTablePointer);
		}
		
		static ColorGrafPortEverythingElse& Allocate(Common::IAllocator& allocator, const Palette* palette, const std::string& allocationName = "Color Graf Port Support")
		{
			ColorGrafPortEverythingElse* support = allocator.Allocate<ColorGrafPortEverythingElse>(allocationName, allocator, palette);
			
			support->colorTable->count = palette->pmEntries;
			for (int16_t i = 0; i < palette->pmEntries; i++)
			{
				ColorSpec& spec = support->colorTable->table[i];
				spec.value = i;
				spec.rgb = palette->pmInfo[i].ciRGB;
			}
			
			return *support;
		}
		
		ColorGrafPortEverythingElse(const ColorGrafPortEverythingElse&) = delete;
		ColorGrafPortEverythingElse(ColorGrafPortEverythingElse&&) = delete;
	};
}

namespace InterfaceLib
{
	struct GrafPortData
	{
		InterfaceLib::UGrafPort* port;
		ColorGrafPortEverythingElse* support;
		IOSurfaceRef surface;
		CGContextRef drawingContext;
		uint32_t dirtyChangeNumber = 0;
		uint32_t cleanChangeNumber = 0;
		
		GrafPortData(InterfaceLib::UGrafPort* port, ColorGrafPortEverythingElse* support)
		: port(port), support(support)
		{
			int32_t width = port->color.portRect.right - port->color.portRect.left;
			int32_t height = port->color.portRect.bottom - port->color.portRect.top;
			int32_t bytesPerElement = 4;
			int32_t bytesPerRow = bytesPerElement * width;
			int32_t allocSize = bytesPerRow * height;
			char pixelFormat[4] = {'A', 'R', 'G', 'B'};
			
			CFOwningRef<CFMutableDictionaryRef> ioSurfaceProperties = CFDictionaryCreateMutable(kCFAllocatorDefault, 9, nullptr, nullptr);
			CFOwningRef<CFNumberRef> cfWidth = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &width);
			CFOwningRef<CFNumberRef> cfHeight = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &height);
			CFOwningRef<CFNumberRef> cfPixelFormat = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pixelFormat);
			CFOwningRef<CFNumberRef> cfBytesPerElement = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &bytesPerElement);
			CFOwningRef<CFNumberRef> cfBytesPerRow = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &bytesPerRow);
			CFOwningRef<CFNumberRef> cfAllocSize = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &allocSize);
			
			CFDictionarySetValue(ioSurfaceProperties, kIOSurfaceIsGlobal, kCFBooleanTrue);
			CFDictionarySetValue(ioSurfaceProperties, kIOSurfaceBytesPerRow, cfBytesPerRow);
			CFDictionarySetValue(ioSurfaceProperties, kIOSurfaceBytesPerElement, cfBytesPerElement);
			CFDictionarySetValue(ioSurfaceProperties, kIOSurfaceWidth, cfWidth);
			CFDictionarySetValue(ioSurfaceProperties, kIOSurfaceHeight, cfHeight);
			CFDictionarySetValue(ioSurfaceProperties, kIOSurfacePixelFormat, cfPixelFormat);
			CFDictionarySetValue(ioSurfaceProperties, kIOSurfaceAllocSize, cfAllocSize);
			
			surface = IOSurfaceCreate(ioSurfaceProperties);
			assert(surface != nullptr && "Null surface");
			
			void* baseAddress = IOSurfaceGetBaseAddress(surface);
			CFOwningRef<CGColorSpaceRef> rgb = CGColorSpaceCreateDeviceRGB();
			
			drawingContext = CGBitmapContextCreate(baseAddress, width, height, 8, bytesPerRow, rgb, kCGImageAlphaNoneSkipFirst);
			
#if !defined(ENABLE_ANTIALIASING) || ENABLE_ANTIALIASING == 0
			// no antialiasing
			CGContextSetShouldAntialias(drawingContext, false);
			CGContextSetShouldSmoothFonts(drawingContext, false);
#endif
			
			// white background
			CGContextSetRGBFillColor(drawingContext, 1, 1, 1, 1);
			CGContextSetRGBStrokeColor(drawingContext, 1, 1, 1, 1);
			CGContextFillRect(drawingContext, CGRectMake(0, 0, width, height));
			
			// text attributes
			CGContextSelectFont(drawingContext, "Geneva", 12, kCGEncodingMacRoman);
		}
		
		GrafPortData(GrafPortData&& that)
		{
			port = that.port;
			surface = that.surface;
			drawingContext = that.drawingContext;
			support = that.support;
			
			that.support = nullptr;
			that.surface = nullptr;
			that.drawingContext = nullptr;
		}
		
		GrafPortData(const GrafPortData&) = delete;
		
		~GrafPortData()
		{
			IOSurfaceDecrementUseCount(surface);
			CGContextRelease(drawingContext);
		}
	};
	
	GrafPortManager::GrafPortManager(Common::IAllocator& allocator)
	: allocator(allocator)
	{
		const int16_t paletteEntryCount = static_cast<int16_t>(ArraySize(defaultColors));
		const size_t paletteSize = sizeof(Palette) + sizeof(ColorInfo) * paletteEntryCount;
		uint8_t* paletteBytes = allocator.Allocate("InterfaceLib Default Palette", paletteSize);
		memset(paletteBytes, 0, paletteSize);
		defaultPalette = reinterpret_cast<Palette*>(paletteBytes);
		
		defaultPalette->pmEntries = paletteEntryCount;
		for (int16_t i = 0; i < paletteEntryCount; i++)
		{
			ColorInfo& info = defaultPalette->pmInfo[i];
			info.ciRGB = ColorFromHex(defaultColors[i]);
		}
	}
	
	InterfaceLib::Palette& GrafPortManager::GetDefaultPalette()
	{
		return *defaultPalette;
	}
	
	InterfaceLib::UGrafPort& GrafPortManager::AllocateGrayGrafPort(const InterfaceLib::Rect& bounds, const std::string& allocationName)
	{
		UGrafPort* port = AllocateGrafPort(allocator, bounds.right - bounds.left, bounds.bottom - bounds.top, allocationName);
		InitializeGrayGrafPort(*port, bounds);
		return *port;
	}
	
	InterfaceLib::UGrafPort& GrafPortManager::AllocateColorGrafPort(const InterfaceLib::Rect& bounds, const InterfaceLib::Palette* palette, const std::string& allocationName)
	{
		UGrafPort* port = AllocateGrafPort(allocator, bounds.right - bounds.left, bounds.bottom - bounds.top, allocationName);
		InitializeColorGrafPort(*port, bounds, palette);
		return *port;
	}
	
	void GrafPortManager::InitializeGrayGrafPort(UGrafPort& uPort, const InterfaceLib::Rect& bounds)
	{
		GrafPort& port = uPort.gray;
		port.portBits.bounds = bounds;
		port.portRect = bounds;
		port.procs = 0;
		// TODO complete initialization
		
		uint32_t address = allocator.ToIntPtr(&port);
		GrafPortData& insertedValue = ports.emplace(std::make_pair(address, GrafPortData(&uPort, nullptr))).first->second;
		insertedValue.port = &uPort;
		
		if (ports.size() == 1)
		{
			currentPort = &insertedValue;
		}
	}
	
	void GrafPortManager::InitializeColorGrafPort(InterfaceLib::UGrafPort &uPort, const InterfaceLib::Rect& bounds, const InterfaceLib::Palette* palette)
	{
		std::stringstream ss;
		ss << "GrafPort Support Fields";
		if (const Common::AllocationDetails* details = allocator.GetDetails(&uPort))
		{
			ss << " for \"" << details->GetAllocationName() << "\" (0x";
			ss << std::hex << std::setw(8) << std::setfill('0') << allocator.ToIntPtr(&uPort) << ")";
		}
		
		const InterfaceLib::Palette* initialPalette = palette == nullptr ? defaultPalette : palette;
		ColorGrafPortEverythingElse& support = ColorGrafPortEverythingElse::Allocate(allocator, initialPalette, ss.str());
		support.pixMap.bounds = bounds;
		support.pixMap.vRes = 72;
		support.pixMap.hRes = 72;
		CGrafPort& port = uPort.color;
		port.portRect = bounds;
		port.portPixMap = allocator.ToIntPtr(&support.pixMapPointer);
		
		uint32_t address = allocator.ToIntPtr(&port);
		GrafPortData& portData = ports.emplace(std::make_pair(address, GrafPortData(&uPort, &support))).first->second;
		portData.port = &uPort;
		
		if (ports.size() == 1)
		{
			currentPort = &portData;
		}
	}
	
	void GrafPortManager::SetCurrentPort(UGrafPort &port)
	{
		uint32_t address = allocator.ToIntPtr(&port);
		auto iter = ports.find(address);
		assert(iter != ports.end() && "Unregistered graphics port");
		currentPort = &iter->second;
	}
	
	UGrafPort& GrafPortManager::GetCurrentPort()
	{
		assert(currentPort != nullptr && "No graf port set");
		return *currentPort->port;
	}
	
	InterfaceLib::Palette* GrafPortManager::PaletteOfGrafPort(InterfaceLib::UGrafPort &port)
	{
		uint32_t address = allocator.ToIntPtr(&port);
		auto iter = ports.find(address);
		if (iter != ports.end())
		{
			return iter->second.support->palette;
		}
		return nullptr;
	}
	
	InterfaceLib::ColorTable* GrafPortManager::ColorTableOfGrafPort(InterfaceLib::UGrafPort &port)
	{
		uint32_t address = allocator.ToIntPtr(&port);
		auto iter = ports.find(address);
		if (iter != ports.end())
		{
			return iter->second.support->colorTable;
		}
		return nullptr;
	}
	
	CGContextRef GrafPortManager::ContextOfGrafPort(InterfaceLib::UGrafPort &port)
	{
		uint32_t address = allocator.ToIntPtr(&port);
		auto iter = ports.find(address);
		if (iter != ports.end())
		{
			GrafPortData& data = iter->second;
			return data.drawingContext;
		}
		
		return nullptr;
	}
	
	IOSurfaceID GrafPortManager::SurfaceOfGrafPort(InterfaceLib::UGrafPort& port)
	{
		uint32_t address = allocator.ToIntPtr(&port);
		auto iter = ports.find(address);
		if (iter != ports.end())
		{
			return IOSurfaceGetID(iter->second.surface);
		}
		
		return 0;
	}
	
	void GrafPortManager::BeginUpdate(InterfaceLib::UGrafPort &port)
	{
		uint32_t key = allocator.ToIntPtr(&port);
		updatedRegions[key] = CGRectNull;
	}
	
	bool GrafPortManager::UpdateRegion(InterfaceLib::UGrafPort &port, CGRect region)
	{
		uint32_t key = allocator.ToIntPtr(&port);
		auto iter = updatedRegions.find(key);
		if (iter == updatedRegions.end())
		{
			return false;
		}
		else
		{
			iter->second = CGRectUnion(iter->second, region);
			return true;
		}
	}
	
	CGRect GrafPortManager::EndUpdate(InterfaceLib::UGrafPort &port)
	{
		uint32_t key = allocator.ToIntPtr(&port);
		auto iter = updatedRegions.find(key);
		if (iter == updatedRegions.end())
		{
			std::string details = allocator.GetDetails(key)->GetAllocationDetails(0);
			std::cerr << "Unbalanced " << __func__ << " call on [" << details << "]" << std::endl;
			return CGRectNull;
		}
		return iter->second;
	}
	
	void GrafPortManager::DestroyGrafPort(UGrafPort& port)
	{
		uint32_t address = allocator.ToIntPtr(&port);
		
		auto iter = ports.find(address);
		if (iter != ports.end())
		{
			if (port.IsColor())
				allocator.Deallocate(iter->second.support);
			
			ports.erase(iter);
		}
	}
	
	GrafPortManager::~GrafPortManager()
	{ }
}
