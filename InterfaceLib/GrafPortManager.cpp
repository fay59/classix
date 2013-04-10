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

#include <sstream>
#include "GrafPortManager.h"

namespace InterfaceLib
{
	struct GrafPortData
	{
		InterfaceLib::GrafPort* port;
		IOSurfaceRef surface;
	};
	
	GrafPortManager::GrafPortManager(Common::IAllocator& allocator)
	: allocator(allocator)
	{ }
	
	InterfaceLib::GrafPort& GrafPortManager::AllocateGrafPort(uint32_t width, uint32_t height, const std::string& allocationName)
	{
		std::stringstream ss;
		ss << "GrafPort <" << width << "x" << height << ">";
		if (allocationName.size() > 0)
		{
			ss << ": " << allocationName;
		}
		
		InterfaceLib::GrafPort* port = allocator.Allocate<InterfaceLib::GrafPort>(ss.str());
		
		InitializeGrafPort(*port, width, height);
		return *port;
	}
	
	void GrafPortManager::InitializeGrafPort(InterfaceLib::GrafPort& port, uint32_t width, uint32_t height)
	{
		port.portBits.bounds.left = -width / 2;
		port.portBits.bounds.top = -height / 2;
		port.portBits.bounds.right = width / 2;
		port.portBits.bounds.bottom = height / 2;
		port.portRect = port.portBits.bounds;
		port.procs = 0;
		// TODO complete initialization
		
		uint32_t address = allocator.ToIntPtr(&port);
		GrafPortData& portData = ports[address];
		portData.port = &port;
		portData.surface = nullptr; // TODO this should create a new IOSurface
		
		if (ports.size() == 1)
		{
			currentPort = &portData;
		}
	}
	
	void GrafPortManager::SetCurrentPort(InterfaceLib::GrafPort &port)
	{
		uint32_t address = allocator.ToIntPtr(&port);
		auto iter = ports.find(address);
		assert(iter != ports.end() && "Unregistered graphics port");
		currentPort = &iter->second;
	}
	
	GrafPort& GrafPortManager::GetCurrentPort()
	{
		assert(currentPort != nullptr && "No graf port set");
		return *currentPort->port;
	}
	
	IOSurfaceRef GrafPortManager::SurfaceOfGrafPort(InterfaceLib::GrafPort& port)
	{
		return nullptr;
	}
	
	void GrafPortManager::DestroyGrafPort(GrafPort& port)
	{
		// TODO destroy IOSurface
		uint32_t address = allocator.ToIntPtr(&port);
		ports.erase(address);
	}
	
	GrafPortManager::~GrafPortManager()
	{
		
	}
}
