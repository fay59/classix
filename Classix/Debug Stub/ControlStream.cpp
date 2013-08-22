//
// ControlStream.cpp
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

#include "ControlStream.h"
#include "FileMapping.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

using namespace Common;

namespace
{
	const int off = 0;
	const int on = 1;
	const char ack = '+';
	const char nack = '-';
	
	uint8_t Byte(const char* stringValue)
	{
		uint8_t result = 0;
		for (int i = 0; i < 2; i++)
		{
			result <<= 4;
			if (stringValue[i] >= '0' && stringValue[i] <= '9')
				result |= stringValue[i] - '0';
			else if (stringValue[i] >= 'a' && stringValue[i] <= 'f')
				result |= stringValue[i] - 'a' + 0xa;
			else if (stringValue[i] >= 'A' && stringValue[i] <= 'F')
				result |= stringValue[i] - 'A' + 0xa;
			else
				throw std::logic_error("Invalid character in byte value");
		}
		return result;
	}
	
	template<typename T, size_t N>
	constexpr size_t ArraySize(const T (&array)[N])
	{
		return N;
	}
}

namespace Classix
{
	class BufferedReader
	{
		char buffer[0x400];
		size_t readIndex;
		size_t maxIndex;
		int fd;
		
		void Refill()
		{
			ssize_t amount = read(fd, buffer, sizeof buffer);
			if (amount < 1)
				throw std::logic_error("Can't read buffer");
			maxIndex = amount;
			readIndex = 0;
		}
		
	public:
		BufferedReader(int fd) : fd(fd)
		{
			readIndex = 0;
			maxIndex = 0;
		}
		
		inline char Read()
		{
			if (readIndex == maxIndex)
			{
				Refill();
			}
			
			return buffer[readIndex++];
		}
		
		template<size_t N>
		inline void Read(char (&c)[N])
		{
			Read(c, N);
		}
		
		inline void Read(char* c, size_t size)
		{
			char* outputTo = c;
			size_t totalRead = 0;
			while (totalRead != size)
			{
				if (readIndex == maxIndex)
				{
					Refill();
				}
				
				size_t readAtOnce = std::min(size, maxIndex - readIndex);
				outputTo = std::copy(buffer + readIndex, buffer + readIndex + readAtOnce, outputTo);
				totalRead += readAtOnce;
				readIndex += readAtOnce;
			}
		}
	};
	
	ControlStream::ControlStream(std::shared_ptr<WaitQueue<std::string>>& queue, int fd)
	: fd(fd), commandQueue(queue), reader(new BufferedReader(fd))
	{
		expectAcks = true;
		maxPayloadSize = 0x4000;
		
		if (reader->Read() != '+')
			throw std::logic_error("Communication should start with an ack");
	}
	
	ControlStream::ControlStream(ControlStream&& that)
	: reader(std::move(reader)), commandQueue(std::move(that.commandQueue))
	{
		fd = that.fd;
		that.fd = -1;
	}
	
	ControlStream ControlStream::Listen(std::shared_ptr<WaitQueue<std::string>>& queue, uint16_t port)
	{
		FileDescriptor sd = socket(PF_INET6, SOCK_STREAM, getprotobyname("TCP")->p_proto);
		setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
		setsockopt(sd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof off);
		
		sockaddr_in6 bindAddress;
		bindAddress.sin6_family = AF_INET6;
		bindAddress.sin6_addr = in6addr_any;
		bindAddress.sin6_port = htons(port);
		
		if (bind(sd, reinterpret_cast<sockaddr*>(&bindAddress), sizeof bindAddress) < 0)
		{
			throw std::logic_error("Couldn't bind control stream");
		}
		
		if (listen(sd, 1) < 0)
		{
			throw std::logic_error("Couldn't listen to socket");
		}
		
		int client = accept(sd, nullptr, nullptr);
		if (client < 0)
		{
			throw std::logic_error("Couldn't accept client");
		}
		
		return ControlStream(queue, client);
	}
	
	void ControlStream::ConsumeReadEvents()
	{
		try
		{
			std::string command;
			command.reserve(32);
			while (true)
			{
				command.clear();
				if (reader->Read() != '$')
				{
					throw std::logic_error("Malformed packet");
				}
				
				uint8_t checksum = 0;
				for (char c = reader->Read(); c != '#'; c = reader->Read())
				{
					command += c;
					checksum += c;
				}
				
				char strChecksum[2];
				reader->Read(strChecksum);
				uint8_t expectedChecksum = Byte(strChecksum);
				if (expectedChecksum != checksum)
				{
					if (expectAcks)
					{
						write(fd, &nack, sizeof nack);
					}
					else if (expectedChecksum != 0)
					{
						// lldb sends packets with a zero cheksum after QStartNoAckMode has been enabled
						throw std::logic_error("Damaged packet");
					}
				}
				
				if (expectAcks)
				{
					write(fd, &ack, sizeof ack);
				}
				std::cerr << "<- " << command << std::endl;
				
				if (!HandleMetaPacket(command))
				{
					commandQueue->PutOne(command);
				}
			}
		}
		catch (std::logic_error&)
		{
			// exit thread
		}
	}
	
	void ControlStream::WriteAnswer(const std::string &answer)
	{
		std::cerr << "-> " << answer << std::endl;
		
		if (answer.length() > maxPayloadSize)
		{
			throw std::logic_error("Answer is larger than max payload");
		}
		
		std::string::size_type length = answer.length();
		std::unique_ptr<char[]> reply(new char[length + 5]);
		reply[0] = '$';
		uint8_t checksum = 0;
		for (std::string::size_type i = 0; i < length; i++)
		{
			reply[i + 1] = answer[i];
			checksum += answer[i];
		}
		
		snprintf(reply.get() + length + 1, 4, "#%02hhx", checksum);
		write(fd, reply.get(), length + 4);
		
		if (expectAcks)
		{
			char ackChar = reader->Read();
			if (ackChar != '+')
			{
				if (ackChar == '-')
					throw std::logic_error("Debug client couldn't understand out answer");
				else
					throw std::logic_error("*** Unexpected answer from debug client: neither ack nor nack");
			}
		}
	}
	
	void ControlStream::WriteAnswer(uint8_t errorCode)
	{
		char errorBuffer[] = "Enn";
		snprintf(errorBuffer, sizeof errorBuffer, "E%02hhx", errorCode);
		WriteAnswer(errorBuffer);
	}
	
	bool ControlStream::ExpectsAcks() const
	{
		return expectAcks;
	}
	
	size_t ControlStream::MaxPayloadSize() const
	{
		return maxPayloadSize;
	}
	
	bool ControlStream::HandleMetaPacket(const std::string &packet)
	{
		static const char qStartNoAckMode[] = "QStartNoAckMode";
		static const char qSetMaxPayloadSize[] = "QSetMaxPayloadSize";
		
		if (packet == qStartNoAckMode)
		{
			WriteAnswer("OK");
			expectAcks = false;
			return true;
		}
		else if (packet.compare(0, ArraySize(qSetMaxPayloadSize) - 1, qSetMaxPayloadSize) == 0)
		{
			maxPayloadSize = strtoul(packet.c_str() + ArraySize(qSetMaxPayloadSize) + 1, nullptr, 16);
			WriteAnswer("OK");
			return true;
		}
		
		return false;
	}
	
	ControlStream::~ControlStream()
	{
		close(fd);
	}
}
