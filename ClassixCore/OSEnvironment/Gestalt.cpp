//
// Gestalt.cpp
// Classix
//
// Copyright (C) 2012 Félix Cloutier
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

#include "Gestalt.h"

namespace OSEnvironment
{
	Gestalt::Gestalt()
	{
		SetValue("sysv", 0x0922);
	}
	
	void Gestalt::SetValue(uint32_t key, int32_t value)
	{
		auto iter = callbackValues.find(key);
		if (iter != callbackValues.end())
		{
			delete iter->second;
			callbackValues.erase(iter);
		}
		
		fixedValues[key] = value;
	}
	
	void Gestalt::SetValue(const Common::FourCharCode &code, int32_t value)
	{
		SetValue(code.code, value);
	}

	bool Gestalt::GetValue(uint32_t key, int32_t& value)
	{
		auto fixedValueIter = fixedValues.find(key);
		if (fixedValueIter != fixedValues.end())
		{
			value = fixedValueIter->second;
			return true;
		}
		
		auto callbackValueIter = callbackValues.find(key);
		if (callbackValueIter != callbackValues.end())
		{
			value = (*callbackValueIter->second)();
			return true;
		}
		
		return false;
	}
	
	bool Gestalt::GetValue(const Common::FourCharCode& code, int32_t &value)
	{
		return GetValue(code.code, value);
	}
	
	int32_t Gestalt::GetValue(uint32_t key)
	{
		int32_t result;
		if (!GetValue(key, result))
			throw std::logic_error("Could not find gestalt key");
		return result;
	}
	
	int32_t Gestalt::GetValue(const Common::FourCharCode& code)
	{
		return GetValue(code.code);
	}
	
	Gestalt::~Gestalt()
	{
		for (auto& pair : callbackValues)
		{
			delete pair.second;
		}
	}
}
