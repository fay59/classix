//
// UIChannel.h
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

#ifndef __Classix__UIChannel__
#define __Classix__UIChannel__

#include <cstdint>
#include <type_traits>
#include <vector>
#include <tuple>
#include <unistd.h>
#include "CommonDefinitions.h"

namespace InterfaceLib
{
	class UIChannel
	{
		union Pipe
		{
			int fd[2];
			struct
			{
				int read;
				int write;
			};
		};
		
		template<size_t... IndexList>
		struct Indices
		{
			typedef Indices<IndexList..., sizeof...(IndexList)> Next;
		};
		
		template<size_t N>
		struct MakeIndices
		{
			typedef typename MakeIndices<N - 1>::Type::Next Type;
		};
		
		// reading to tuples
		// thanks Daniel Frey -- http://stackoverflow.com/q/16248828/variadic-read-tuples
		// the Indices::Next definition is pretty clever.
		template<typename TTupleType>
		struct TupleReader
		{
			int fd;
			TTupleType storage;
			
			template<typename TElement>
			inline size_t ReadOne(size_t index, TElement& into)
			{
				return ::read(fd, &into, sizeof into);
			}
			
			template<size_t... Ns>
			inline void ReadImpl(const Indices<Ns...>&)
			{
				PACK_EXPAND(ReadOne(Ns, std::get<Ns>(storage)));
			}
			
			TupleReader(int fd) : fd(fd)
			{ }
			
			inline void Read()
			{
				ReadImpl(typename MakeIndices<std::tuple_size<TTupleType>::value>::Type());
			}
		};
		
		// utilities
		// WriteToPipe needs to return a value to be usable in a variadic template expansion context
		template<typename T>
		size_t WriteToPipe(const T& argument)
		{
			return ::write(write.write, &argument, sizeof argument);
		}
		
		template<typename T>
		size_t WriteToPipe(const std::vector<T>& argument)
		{
			uint32_t count = static_cast<uint32_t>(argument.size());
			size_t total = ::write(write.write, &count, sizeof count);
			for (const T& item : argument)
				total += WriteToPipe(item);
			return total;
		}
		
		template<typename T>
		T ReturnNonVoid(const uint8_t* buffer)
		{
			return *reinterpret_cast<const T*>(buffer);
		}
		
		// fields
		Pipe read;
		Pipe write;
		pid_t head;
		
	public:
		UIChannel();
		
		template<typename TReturnType, typename... TArgument>
		TReturnType PerformAction(IPCMessage message, TArgument&&... argument)
		{
			static_assert(!std::is_pointer<TReturnType>::value, "Using DoMessage with a pointer type");
			
			char doneReference[4] = {'D', 'O', 'N', 'E'};
			
			::write(write.write, &message, sizeof message);
			PACK_EXPAND(WriteToPipe(argument));
			::write(write.write, doneReference, sizeof doneReference);
			
			uint8_t response[sizeof(TReturnType)];
			if (!std::is_void<TReturnType>::value)
				::read(read.read, response, sizeof response);
			
			char done[4];
			::read(read.read, done, sizeof done);
			if (memcmp(done, doneReference, sizeof doneReference) != 0)
				throw std::logic_error("Wrong return type for action");
			
			return ReturnNonVoid<TReturnType>(response);
		}
		
		template<typename TTupleType, typename... TArgument>
		TTupleType PerformComplexAction(IPCMessage message, TArgument&&... argument)
		{
			char doneReference[4] = {'D', 'O', 'N', 'E'};
			
			::write(write.write, &message, sizeof message);
			PACK_EXPAND(WriteToPipe(argument));
			::write(write.write, doneReference, sizeof doneReference);
			
			TupleReader<TTupleType> reader(read.read);
			reader.Read();
			
			char done[4];
			::read(read.read, done, sizeof done);
			if (memcmp(done, doneReference, sizeof doneReference) != 0)
				throw std::logic_error("Wrong return type for action");
			
			return reader.storage;
		}
		
		~UIChannel();
	};
	
	template<>
	struct UIChannel::MakeIndices<0>
	{
		typedef typename UIChannel::Indices<> Type;
	};
}

#endif /* defined(__Classix__UIChannel__) */
