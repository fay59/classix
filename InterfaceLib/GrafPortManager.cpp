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

#include <sstream>
#include <iomanip>
#include "GrafPortManager.h"
#include "CFOwningRef.h"

namespace
{
	using namespace InterfaceLib;
	
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
		InterfaceLib::PixMap pixMap;
		InterfaceLib::ColorTable colorTable;
		
		ColorGrafPortEverythingElse(Common::IAllocator& allocator, uint16_t colorTableSize)
		{
			memset(this, 0, sizeof *this);
			pixMapPointer = allocator.ToIntPtr(&pixMap);
			colorTablePointer = allocator.ToIntPtr(&colorTable);
			pixMap.pmTable = allocator.ToIntPtr(&colorTablePointer);
			colorTable.count = colorTableSize;
		}
		
		static ColorGrafPortEverythingElse& Allocate(Common::IAllocator& allocator, uint16_t colorTableSize, const std::string& allocationName = "Color Graf Port Support")
		{
			size_t totalSize = sizeof(ColorGrafPortEverythingElse) + sizeof(ColorSpec) * colorTableSize;
			uint8_t* bytes = allocator.Allocate(allocationName, totalSize);
			return *new (bytes) ColorGrafPortEverythingElse(allocator, colorTableSize);
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
		IOSurfaceRef surface;
		CGContextRef drawingContext;
		uint32_t dirtyChangeNumber = 0;
		uint32_t cleanChangeNumber = 0;
		
		GrafPortData(InterfaceLib::UGrafPort* port)
		: port(port)
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
			
			// white background
			CGContextSetRGBFillColor(drawingContext, 1, 1, 1, 1);
			CGContextFillRect(drawingContext, CGRectMake(0, 0, width, height));
			
			// text attributes
			CGContextSelectFont(drawingContext, "Geneva", 12, kCGEncodingMacRoman);
		}
		
		GrafPortData(GrafPortData&& that)
		{
			port = that.port;
			surface = that.surface;
			drawingContext = that.drawingContext;
			
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
	{ }
	
	InterfaceLib::UGrafPort& GrafPortManager::AllocateGrayGrafPort(const InterfaceLib::Rect& bounds, const std::string& allocationName)
	{
		UGrafPort* port = AllocateGrafPort(allocator, bounds.right - bounds.left, bounds.bottom - bounds.top, allocationName);
		InitializeGrayGrafPort(*port, bounds);
		return *port;
	}
	
	InterfaceLib::UGrafPort& GrafPortManager::AllocateColorGrafPort(const InterfaceLib::Rect& bounds, const std::string& allocationName)
	{
		UGrafPort* port = AllocateGrafPort(allocator, bounds.right - bounds.left, bounds.bottom - bounds.top, allocationName);
		InitializeColorGrafPort(*port, bounds);
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
		GrafPortData& portData = ports.emplace(std::make_pair(address, GrafPortData(&uPort))).first->second;
		portData.port = &uPort;
		portData.surface = nullptr; // TODO this should create a new IOSurface
		
		if (ports.size() == 1)
		{
			currentPort = &portData;
		}
	}
	
	void GrafPortManager::InitializeColorGrafPort(InterfaceLib::UGrafPort &uPort, const InterfaceLib::Rect& bounds)
	{
		std::stringstream ss;
		ss << "GrafPort Support Fields";
		if (const Common::AllocationDetails* details = allocator.GetDetails(&uPort))
		{
			ss << " for \"" << details->GetAllocationName() << "\" (0x";
			ss << std::hex << std::setw(8) << std::setfill('0') << allocator.ToIntPtr(&uPort) << ")";
		}
		
		ColorGrafPortEverythingElse& support = ColorGrafPortEverythingElse::Allocate(allocator, 0, ss.str());
		support.pixMap.bounds = bounds;
		support.pixMap.vRes = 72;
		support.pixMap.hRes = 72;
		CGrafPort& port = uPort.color;
		port.portRect = bounds;
		port.portPixMap = allocator.ToIntPtr(&support.pixMapPointer);
		// TODO complete initialization
		
		uint32_t address = allocator.ToIntPtr(&port);
		GrafPortData& portData = ports.emplace(std::make_pair(address, GrafPortData(&uPort))).first->second;
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
	
	void GrafPortManager::DestroyGrafPort(UGrafPort& port)
	{
		uint32_t address = allocator.ToIntPtr(&port);
		if (port.IsColor())
		{
			ColorGrafPortEverythingElse* support = allocator.ToPointer<ColorGrafPortEverythingElse>(port.color.portPixMap);
			allocator.Deallocate(support);
		}
		
		ports.erase(address);
	}
	
	GrafPortManager::~GrafPortManager()
	{ }
}
