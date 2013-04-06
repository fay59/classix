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

#include "IAllocator.h"

#include <iostream>
#include <string>
#include <array>
#include <cstdint>
#include <deque>
#include <unordered_map>

// Information gathered from Inside Macintosh vol. 1 pages 128-130.
// It's funny, that book is older than me.

namespace InterfaceLib
{
	struct ResourceEntry
	{
		uint16_t id;
		std::string name;
		uint8_t attributes;
		uint8_t* _begin;
		uint8_t* _end;
		
		inline uint8_t* begin() { return _begin; }
		inline uint8_t* end() { return _end; }
		inline const uint8_t* begin() const { return _begin; }
		inline const uint8_t* end() const { return _end; }
	};
	
	struct FourCharCode
	{
		uint32_t code;
		
		inline FourCharCode(uint32_t code) : code(code)
		{}
		
		inline FourCharCode(const char (&array)[5]) : code(0)
		{
			assert(array[4] == '0' && "Expected a four-char code");
			for (size_t i = 0; i < 4; i++)
			{
				code <<= 8;
				code |= array[i];
			}
		}
	};
	
	std::ostream& operator<<(std::ostream& into, const FourCharCode& code);
	
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
		
		ResourceEntry* GetResource(const FourCharCode& type, uint16_t identifier);
		ResourceEntry* GetResource(const FourCharCode& type, const std::string& identifier);
		
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
		
		ResourceEntry* GetResource(const FourCharCode& type, uint16_t identifier);
		ResourceEntry* GetResource(const FourCharCode& type, const std::string& identifier);
		
		inline void dump() { for (auto& catalog : catalogs) catalog.dump(); }
	};
}

#endif /* defined(__Classix__ResourceManager__) */
