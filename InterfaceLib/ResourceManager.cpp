//
// ResourceManager.cpp
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
#include <algorithm>
#include <iterator>
#include <cassert>

#include "ResourceManager.h"
#include "BigEndian.h"
#include "FileMapping.h"

using namespace Common;

namespace
{
	inline std::string StringFromFourCharCode(uint32_t fourCC)
	{
		char result[5] = {0};
		result[0] = fourCC >> 24;
		result[1] = (fourCC >> 16) & 0xff;
		result[2] = (fourCC >> 8) & 0xff;
		result[3] = fourCC & 0xff;
		return result;
	}
	
	struct ResourceForkHeader
	{
		UInt32 resourceDataOffset;
		UInt32 resourceMapOffset;
		UInt32 resourceDataLength;
		UInt32 resourceMapLength;
	};
	
	struct ResourceForkBaseData
	{
		ResourceForkHeader header;
		uint8_t systemData[0x70];
		uint8_t applicationData[0x80];
	};
	
	struct ResourceMapHeader
	{
		// ununsed
		ResourceForkHeader headerCopy;
		UInt32 nextResourceMap;
		UInt16 fileReference;
		
		UInt16 attributes;
		UInt16 typeListOffset;
		UInt16 nameListOffset;
	};
	
	struct ResourceType
	{
		UInt32 identifier;
		UInt16 itemCount;
		UInt16 referenceListOffset;
	};
	
	struct ResourceReference
	{
		UInt16 resourceId;
		UInt16 nameOffset;
		uint8_t attributes;
		uint8_t resourceDataOffset[3];
		uint32_t resourceHandle; // unused
		
		inline uint32_t ResourceDataOffset() const
		{
			return (resourceDataOffset[0] << 16) | (resourceDataOffset[1] << 8) | resourceDataOffset[2];
		}
	};
}

namespace InterfaceLib
{
	std::ostream& operator<<(std::ostream& into, const FourCharCode& code)
	{
		into << '\'';
		into << (char)((code.code >> 24) & 0xff);
		into << (char)((code.code >> 16) & 0xff);
		into << (char)((code.code >> 8) & 0xff);
		into << (char)(code.code & 0xff);
		into << '\'';
		return into;
	}
	
	ResourceCatalog::ResourceCatalog(Common::IAllocator& allocator, const std::string& path)
	: allocator(allocator)
	{
		// FIXME check for buffer overflows!
		
		FileMapping mapping(path + "/..namedfork/rsrc");
		const uint8_t* rsrc = reinterpret_cast<const uint8_t*>(mapping.begin());
		const ResourceForkBaseData* base = reinterpret_cast<const ResourceForkBaseData*>(rsrc);
		
		systemData = allocator.Allocate<std::array<uint8_t, 0x70>>(path + " rsrc system data");
		applicationData = allocator.Allocate<std::array<uint8_t, 0x80>>(path + " rsrc application data");
		
		std::copy(std::begin(base->systemData), std::end(base->systemData), systemData->begin());
		std::copy(std::begin(base->applicationData), std::end(base->applicationData), applicationData->begin());
		
		const uint8_t* dataBegin = rsrc + base->header.resourceDataOffset;
		const uint8_t* dataEnd = dataBegin + base->header.resourceDataLength;
		const uint8_t* mapBegin = rsrc + base->header.resourceMapOffset;
		const ResourceMapHeader* mapHeader = reinterpret_cast<const ResourceMapHeader*>(mapBegin);
		
		const uint8_t* typeListBegin = mapBegin + mapHeader->typeListOffset;
		uint16_t typeCount = *reinterpret_cast<const UInt16*>(typeListBegin) + 1;
		resourcesById.reserve(typeCount);
		resourcesByName.reserve(typeCount);
		
		const char* nameListBegin = reinterpret_cast<const char*>(mapBegin + mapHeader->nameListOffset);
		const ResourceType* typeList = reinterpret_cast<const ResourceType*>(typeListBegin + sizeof typeCount);
		const ResourceReference* resourceReference = reinterpret_cast<const ResourceReference*>(typeListBegin + sizeof typeCount + typeCount * sizeof(ResourceType));
		for (size_t i = 0; i < typeCount; i++)
		{
			const ResourceType& type = typeList[i];
			std::unordered_map<uint16_t, ResourceEntry>& idResourceMap = resourcesById[type.identifier];
			std::unordered_map<std::string, ResourceEntry*>& nameResourceMap = resourcesByName[type.identifier];
			uint32_t itemCount = type.itemCount + 1;
			idResourceMap.reserve(itemCount);
			nameResourceMap.reserve(itemCount);
			
			std::string resourceTypeName = StringFromFourCharCode(type.identifier);
			for (size_t j = 0; j < itemCount; j++)
			{
				ResourceEntry entry = {
					.id = resourceReference->resourceId,
					.attributes = resourceReference->attributes
				};
				
				if (resourceReference->nameOffset != 0xffff)
				{
					const char* name = nameListBegin + resourceReference->nameOffset;
					uint8_t length = *name;
					name++;
					entry.name = std::string(name, name + length);
				}
				
				uint32_t offset = resourceReference->ResourceDataOffset();
				const uint8_t* dataLocation = dataBegin + offset;
				assert(dataLocation + 4 <= dataEnd && "Overflowing read for resource data length");
				uint32_t dataLength = *reinterpret_cast<const UInt32*>(dataLocation);
				dataLocation += sizeof dataLength;
				assert(dataLocation + dataLength <= dataEnd && "Resource data overflows");
				
				std::stringstream allocationName;
				allocationName << "'" << resourceTypeName << "' Resource #" << resourceReference->resourceId;
				if (entry.name.length() > 0)
				{
					allocationName << " (\"" << entry.name << "\")";
				}
				
				// allocate room for the handle as well
				dataLength += 4;
				entry._begin = allocator.Allocate(allocationName.str(), dataLength);
				entry._end = entry.begin() + dataLength;
				std::copy(dataLocation, dataLocation + dataLength, entry.begin());
				entry.handle() = allocator.ToIntPtr(entry.begin());
				
				idResourceMap[entry.id] = entry;
				if (entry.name.length() > 0)
				{
					nameResourceMap[entry.name] = &idResourceMap[entry.id];
				}
				
				resourceReference++;
			}
		}
	}
	
	ResourceCatalog::ResourceCatalog(ResourceCatalog&& that)
	: allocator(that.allocator)
	{
		systemData = that.systemData;
		applicationData = that.applicationData;
		resourcesById.swap(that.resourcesById);
		resourcesByName.swap(that.resourcesByName);
		
		that.systemData = nullptr;
		that.applicationData = nullptr;
	}
	
	ResourceEntry* ResourceCatalog::GetRawResource(const FourCharCode& type, uint16_t identifier)
	{
		auto topLevelIter = resourcesById.find(type.code);
		if (topLevelIter != resourcesById.end())
		{
			auto iter = topLevelIter->second.find(identifier);
			if (iter != topLevelIter->second.end())
			{
				return &iter->second;
			}
		}
		return nullptr;
	}
		
	ResourceEntry* ResourceCatalog::GetRawResource(const FourCharCode& type, const std::string& identifier)
	{
		auto topLevelIter = resourcesByName.find(type.code);
		if (topLevelIter != resourcesByName.end())
		{
			auto iter = topLevelIter->second.find(identifier);
			if (iter != topLevelIter->second.end())
			{
				return iter->second;
			}
		}
		return nullptr;
	}
	
	void ResourceCatalog::dump()
	{
		for (const auto& pair : resourcesById)
		{
			std::cout << "Key " << FourCharCode(pair.first) << ":\n";
			for (const auto& innerPair : pair.second)
			{
				const ResourceEntry& entry = innerPair.second;
				std::cout << "\tID #" << entry.id << ": " << entry.name << "\n";
			}
		}
	}
	
	ResourceCatalog::~ResourceCatalog()
	{
		allocator.Deallocate(systemData);
		allocator.Deallocate(applicationData);
	}
	
	ResourceManager::ResourceManager(Common::IAllocator& allocator)
	: allocator(allocator)
	{ }
	
	void ResourceManager::LoadFileResources(const std::string &filePath)
	{
		ResourceCatalog catalog(allocator, filePath);
		catalogs.emplace_back(std::move(catalog));
	}
	
	ResourceEntry* ResourceManager::GetRawResource(const FourCharCode& type, uint16_t identifier)
	{
		for (ResourceCatalog& catalog : catalogs)
		{
			if (ResourceEntry* entry = catalog.GetRawResource(type, identifier))
				return entry;
		}
		return nullptr;
	}
	
	ResourceEntry* ResourceManager::GetRawResource(const FourCharCode& type, const std::string& identifier)
	{
		for (ResourceCatalog& catalog : catalogs)
		{
			if (ResourceEntry* entry = catalog.GetRawResource(type, identifier))
				return entry;
		}
		return nullptr;
	}
}
