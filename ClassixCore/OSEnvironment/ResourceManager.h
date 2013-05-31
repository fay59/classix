//
// ResourceManager.h
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

#ifndef __Classix__ResourceManager__
#define __Classix__ResourceManager__

#include <iostream>
#include <string>
#include <array>
#include <cstdint>
#include <deque>
#include <unordered_map>

#include "IAllocator.h"
#include "BigEndian.h"
#include "FourCharCode.h"

// Information gathered from Inside Macintosh vol. 1 pages 128-130.
// It's funny, that book is older than me.

namespace OSEnvironment
{
	struct ResourceEntry
	{
		uint16_t id;
		std::string name;
		uint8_t attributes;
		uint8_t* _begin;
		uint8_t* _end;
		
		inline Common::UInt32& handle() { return *reinterpret_cast<Common::UInt32*>(_begin); }
		inline uint8_t* begin() { return _begin + sizeof(uint32_t); }
		inline uint8_t* end() { return _end; }
		inline Common::UInt32 handle() const { return *reinterpret_cast<const Common::UInt32*>(_begin); }
		inline const uint8_t* begin() const { return _begin; }
		inline const uint8_t* end() const { return _end; }
	};
	
	class ResourceCatalog
	{
		friend class ResourceManager;
		
		Common::IAllocator& allocator;
		std::array<uint8_t, 0x70>* systemData;
		std::array<uint8_t, 0x80>* applicationData;
		std::unordered_map<uint32_t, std::unordered_map<uint16_t, ResourceEntry>> resourcesById;
		std::unordered_map<uint32_t, std::unordered_map<std::string, ResourceEntry*>> resourcesByName;
		
		explicit ResourceCatalog(Common::IAllocator& allocator, const std::string& filePath);
		
	public:
		ResourceCatalog(const ResourceCatalog&) = delete;
		ResourceCatalog(ResourceCatalog&& that);
		
		ResourceEntry* GetRawResource(const Common::FourCharCode& type, uint16_t identifier);
		ResourceEntry* GetRawResource(const Common::FourCharCode& type, const std::string& identifier);
		
		void dump();
		
		~ResourceCatalog();
	};
	
	class ResourceManager
	{
		Common::IAllocator& allocator;
		std::deque<ResourceCatalog> catalogs;
		
	public:
		ResourceManager(Common::IAllocator& allocator);
		
		void LoadFileResources(const std::string& filePath);
		
		ResourceEntry* GetRawResource(const Common::FourCharCode& type, uint16_t identifier);
		ResourceEntry* GetRawResource(const Common::FourCharCode& type, const std::string& identifier);
		
		template<typename TResourceType>
		TResourceType* GetResource(uint16_t identifier)
		{
			if (ResourceEntry* resource = GetRawResource(TResourceType::key, identifier))
				return reinterpret_cast<TResourceType*>(resource->begin());
			return nullptr;
		}
		
		template<typename TResourceType>
		TResourceType* GetResource(const std::string& identifier)
		{
			if (ResourceEntry* resource = GetRawResource(TResourceType::key, identifier))
				return reinterpret_cast<TResourceType*>(resource->begin());
			return nullptr;
		}
		
		inline void dump() { for (auto& catalog : catalogs) catalog.dump(); }
	};
}

#endif /* defined(__Classix__ResourceManager__) */
