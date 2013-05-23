//
// ResourceTypes.cpp
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

#include "ResourceTypes.h"

namespace InterfaceLib
{
	const FourCharCode Resources::cfrg::key = "cfrg";
	const FourCharCode Resources::WIND::key = "WIND";
	const FourCharCode Resources::MENU::key = "MENU";
	const FourCharCode Resources::DLOG::key = "DLOG";
	
	const Resources::MENU::Item* Resources::MENU::GetFirstItem() const
	{
		const uint8_t* stringBase = &titleLength;
		const char* itemBase = reinterpret_cast<const char*>(stringBase + 1 + *stringBase);
		return *itemBase == 0 ? nullptr : reinterpret_cast<const Item*>(itemBase);
	}
	
	const Resources::cfrg::Member* Resources::cfrg::GetFirstMember() const
	{
		const uint8_t* dataBase = reinterpret_cast<const uint8_t*>(this);
		dataBase += sizeof *this;
		return reinterpret_cast<const Resources::cfrg::Member*>(dataBase);
	}
	
	std::string Resources::cfrg::Member::GetTitle() const
	{
		const Str255* string = reinterpret_cast<const Str255*>(&titleLength);
		return *string;
	}
	
	std::string Resources::MENU::GetTitle() const
	{
		const Str255* string = reinterpret_cast<const Str255*>(&titleLength);
		return *string;
	}
	
	std::string Resources::MENU::Item::GetTitle() const
	{
		const Str255* string = reinterpret_cast<const Str255*>(this);
		return *string;
	}
	
	uint8_t Resources::MENU::Item::GetIconNumber() const
	{
		const uint8_t* base = reinterpret_cast<const uint8_t*>(this);
		const Str255* string = reinterpret_cast<const Str255*>(base);
		return *(base + 1 + string->length);
	}
	
	uint8_t Resources::MENU::Item::GetKeyEquivalent() const
	{
		const uint8_t* base = reinterpret_cast<const uint8_t*>(this);
		const Str255* string = reinterpret_cast<const Str255*>(base);
		return *(base + 1 + string->length + 1);
	}
	
	uint8_t Resources::MENU::Item::GetCharacterMark() const
	{
		const uint8_t* base = reinterpret_cast<const uint8_t*>(this);
		const Str255* string = reinterpret_cast<const Str255*>(base);
		return *(base + 1 + string->length + 2);
	}
	
	uint8_t Resources::MENU::Item::GetTextStyle() const
	{
		const uint8_t* base = reinterpret_cast<const uint8_t*>(this);
		const Str255* string = reinterpret_cast<const Str255*>(base);
		return *(base + 1 + string->length + 3);
	}
	
	const Resources::MENU::Item* Resources::MENU::Item::GetNextItem() const
	{
		const uint8_t* base = reinterpret_cast<const uint8_t*>(this);
		const Str255* string = reinterpret_cast<const Str255*>(base);
		const uint8_t* nextBase = base + 1 + string->length + 4;
		return *nextBase == 0 ? nullptr : reinterpret_cast<const Item*>(nextBase);
	}
	
	std::string Resources::DLOG::GetTitle() const
	{
		const Str255* string = reinterpret_cast<const Str255*>(&titleLength);
		return *string;
	}
}