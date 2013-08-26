//
// StackPreparator.cpp
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

#include <sstream>
#include "Allocator.h"
#include "StackPreparator.h"
#include "BigEndian.h"

namespace
{
	struct StackBuilder
	{
		char* begin;
		char* end;
		char* sp;
		
		StackBuilder(char* begin, size_t size)
		{
			this->begin = begin;
			this->end = begin + size;
			this->sp = end;
			
			memset(begin, Common::Allocator::ScribbleStackPattern, size);
		}
		
		ptrdiff_t WriteString(const std::string& string)
		{
			std::string::size_type length = string.length() + 1;
			sp -= length;
			assert(sp >= begin && "Stack overflow");
			
			strncpy(sp, string.c_str(), length);
			return sp - begin;
		}
		
		void Align(size_t boundary)
		{
			assert(__builtin_popcount(boundary) == 1 && "Alignment boundary is not a power of two");
			intptr_t address = reinterpret_cast<intptr_t>(sp);
			address &= ~(boundary - 1);
			sp = reinterpret_cast<char*>(address);
		}
		
		template<typename T>
		ptrdiff_t Write(const T& value)
		{
			sp -= sizeof value;
			assert(sp >= begin && "Stack overflow");
			memcpy(sp, &value, sizeof value);
			return sp - begin;
		}
	};
}

namespace Common
{
	const size_t StackPreparator::DefaultStackSize = 0x10000; // 64KB ought to be enough for anybody
	
	size_t StackPreparator::ArgumentCount() const
	{
		return argv.size();
	}
	
	void StackPreparator::AddArgument(const std::string &arg)
	{
		argv.push_back(arg);
	}
	
	void StackPreparator::AddEnvironmentVariable(const std::string& env)
	{
		envp.push_back(env);
	}
	
	void StackPreparator::AddEnvironmentVariable(const std::string &key, const std::string &value)
	{
		std::stringstream ss;
		ss << key << '=' << value;
		AddEnvironmentVariable(ss.str());
	}
	
	StackPreparator::StackInfo StackPreparator::WriteStack(char *stack, uint32_t virtualAddress, size_t stackSize) const
	{
		//  stack layout:
		// +-------------+
		// | string area |
		// +-------------+
		// | separator 0 |
		// +-------------+
		// |    env[n]   |
		// +-------------+
		//        :
		// +-------------+
		// |    env[0]   | <-- envp
		// +-------------+
		// | separator 0 |
		// +-------------+
		// | arg[argc-1] |
		// +-------------+
		//        :
		// +-------------+
		// |    arg[0]   | <-- argv
		// +-------------+
		// | separator 0 |
		// +-------------+
		// |   0 (toc)   |
		// +-------------+
		// |      0      |
		// +-------------+
		// |      0      |
		// +-------------+
		// |    0 (lr)   |
		// +-------------+
		// |    0 (cr)   |
		// +-------------+
		// |    0 (sp)   | <-- sp
		// +-------------+
		
		StackInfo result;
		StackBuilder builder(stack, stackSize);
		
		std::deque<uint32_t> stringOffsets;
		for (const std::string& arg : argv)
		{
			uint32_t offset = static_cast<uint32_t>(builder.WriteString(arg));
			stringOffsets.push_back(offset);
		}
		
		for (const std::string& env : envp)
		{
			uint32_t offset = static_cast<uint32_t>(builder.WriteString(env));
			stringOffsets.push_back(offset);
		}
		
		builder.Align(4);
		builder.Write<uint32_t>(0);
		
		auto offsetIterator = stringOffsets.rbegin();
		for (size_t i = 0; i < envp.size(); i++)
		{
			UInt32 address(*offsetIterator + virtualAddress);
			builder.Write(address);
			offsetIterator++;
		}
		
		result.envp = reinterpret_cast<char**>(builder.sp);
		builder.Align(4);
		builder.Write<uint32_t>(0);
		
		for (size_t i = 0; i < argv.size(); i++)
		{
			UInt32 address(*offsetIterator + virtualAddress);
			builder.Write(address);
			offsetIterator++;
		}
		assert(offsetIterator == stringOffsets.rend() && "Somehow didn't write an offset to each string");
		
		result.argv = reinterpret_cast<char**>(builder.sp);
		result.argc = argv.size();
		
		// separator 0, r2, reserved, reserved, lr, cr, sp
		for (size_t i = 0; i < 7; i++)
			builder.Write<uint32_t>(0);
		
		result.sp = builder.sp;
		return result;
	}
}
