//
// ResourceTypes.h
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

#ifndef Classix_ResourceTypes_h
#define Classix_ResourceTypes_h

#include <string>
#include <vector>
#include <cassert>
#include <type_traits>
#include "CommonDefinitions.h"
#include "ResourceManager.h"

namespace InterfaceLib
{
namespace Resources
{
	struct __attribute__((packed)) Str255
	{
		uint8_t length;
		char string[0];
		
		inline operator std::string() const
		{
			return std::string(string, length);
		}
	};
	
	struct Unconstructible
	{
		Unconstructible() = delete;
		Unconstructible(const Unconstructible&) = delete;
		Unconstructible(Unconstructible&&) = delete;
		Unconstructible operator=(const Unconstructible&) = delete;
	};
	
	struct __attribute__((packed)) WIND : Unconstructible
	{
		static const FourCharCode key;
		
		Rect windowRect;
		Common::UInt16 procID;
		Common::UInt16 visible;
		Common::UInt16 goAwayFlag;
		Common::UInt32 refCon;
		Str255 title;
	};
	
	struct __attribute__((packed)) MENU : Unconstructible
	{
		static const FourCharCode key;
		
		class Item : Unconstructible
		{
		public:
			std::string GetTitle() const;
			uint8_t GetIconNumber() const;
			uint8_t GetKeyEquivalent() const;
			uint8_t GetCharacterMark() const;
			uint8_t GetTextStyle() const;
			
			const Item* GetNextItem() const;
		};
		
		Common::UInt16 menuId;
		Common::UInt16 width;
		Common::UInt16 height;
		Common::UInt16 definitionProcedure;
		Common::UInt16 zero;
		Common::UInt32 enableFlags;
		uint8_t titleLength;
		
		std::string GetTitle() const;
		const Item* GetFirstItem() const;
	};
}
}

#endif
