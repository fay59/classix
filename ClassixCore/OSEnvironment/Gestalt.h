//
// Gestalt.h
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

#ifndef __Classix__Gestalt__
#define __Classix__Gestalt__

#include <unordered_map>
#include <memory>
#include <cstdint>

#include "FourCharCode.h"

namespace OSEnvironment
{
	class GestaltCallback
	{
	public:
		virtual int32_t operator()() = 0;
		virtual ~GestaltCallback() = 0;
	};
	
	class Gestalt
	{
		std::unordered_map<uint32_t, int32_t> fixedValues;
		std::unordered_map<uint32_t, GestaltCallback*> callbackValues;
		
	public:
		template<typename TType, typename... TParams>
		void SetCallback(uint32_t key, TParams&&... args)
		{
			fixedValues.erase(key);
			callbackValues[key] = new TType(args...);
		}
		
		template<typename TType, typename... TParams>
		void SetCallback(const Common::FourCharCode& code, TParams&&... args)
		{
			SetCallback<TType>(code.code, args...);
		}
		
		void SetValue(uint32_t key, int32_t value);
		void SetValue(const Common::FourCharCode& code, int32_t value);
		bool GetValue(uint32_t key, int32_t& value);
		bool GetValue(const Common::FourCharCode& code, int32_t& value);
		
		~Gestalt();
	};
}

#endif /* defined(__Classix__Gestalt__) */
