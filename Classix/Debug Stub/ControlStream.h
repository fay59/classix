//
// ControlStream.h
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

#ifndef __Classix__ControlStream__
#define __Classix__ControlStream__

#include <cstdint>
#include <string>
#include <memory>
#include <thread>
#include "WaitQueue.h"

class BufferedReader;
// uses the lldb remote protocol
class ControlStream
{
	int fd;
	std::unique_ptr<BufferedReader> reader;
	
	std::shared_ptr<WaitQueue<std::string>> commandQueue;
	
	// control parameters
	bool expectAcks;
	size_t maxPayloadSize;
	
	ControlStream(std::shared_ptr<WaitQueue<std::string>>&, int fd);
	
	bool HandleMetaPacket(const std::string& packet);
	
public:
	ControlStream(const ControlStream& that) = delete;
	ControlStream(ControlStream&& that);
	
	static ControlStream Listen(std::shared_ptr<WaitQueue<std::string>>& waitQueue, uint16_t port);
	
	void WriteAnswer(const std::string& answer);
	void WriteAnswer(uint8_t errorCode);
	
	bool ExpectsAcks() const;
	size_t MaxPayloadSize() const;
	
	void ConsumeReadEvents(); // expected to run on a dedicated thread
	
	~ControlStream();
};

#endif /* defined(__Classix__ControlStream__) */
