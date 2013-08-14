//
// DebugStub.cpp
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

#include <thread>
#include <signal.h>
#include <mach/machine.h>

#include "DebugStub.h"
#include "NativeAllocator.h"
#include "BundleLibraryResolver.h"
#include "DlfcnLibraryResolver.h"
#include "PEFLibraryResolver.h"
#include "DummyLibraryResolver.h"
#include "Todo.h"

namespace
{
	enum DebugStubErrorCodes : uint8_t
	{
		NoError,
		NotImplemented,
		InvalidFormat,
		InvalidData,
	};
	
	std::string StringPrintf(const char* format, ...) __attribute__((format(printf, 1, 2)));
	std::string StringPrintf(const char* format, ...)
	{
		std::unique_ptr<char, decltype(&free)> freeResult(nullptr, free);
		
		char* result;
		va_list ap;
		va_start(ap, format);
		vasprintf(&result, format, ap);
		va_end(ap);
		
		freeResult.reset(result);
		std::string stringResult = result;
		return stringResult;
	}
	
	const char hexgits[] = "0123456789abcdef";
}

namespace Classix
{
	DebugContext::DebugContext(const std::string& executable)
	: allocator(new Common::NativeAllocator), managers(*allocator)
	{
		ClassixCore::BundleLibraryResolver* bundleResolver = new ClassixCore::BundleLibraryResolver(*allocator, managers);
		bundleResolver->AllowLibrary("InterfaceLib");
		bundleResolver->AllowLibrary("ControlStripLib");
		resolvers.emplace_back(bundleResolver);
		
		ClassixCore::DlfcnLibraryResolver* dlfcnResolver = new ClassixCore::DlfcnLibraryResolver(*allocator, managers);
		dlfcnResolver->RegisterLibrary("StdCLib");
		dlfcnResolver->RegisterLibrary("MathLib");
		dlfcnResolver->RegisterLibrary("ThreadsLib");
		resolvers.emplace_back(dlfcnResolver);
		
		CFM::PEFLibraryResolver* pefResolver = new CFM::PEFLibraryResolver(*allocator, fragmentManager);
		resolvers.emplace_back(pefResolver);
		
		CFM::DummyLibraryResolver* dummyResolver = new CFM::DummyLibraryResolver(*allocator);
		resolvers.emplace_back(dummyResolver);
	}
	
	const std::unordered_map<std::string, DebugStub::RemoteCommand> DebugStub::commands = {
		std::make_pair("H", &DebugStub::SetOperationTargetThread),
		std::make_pair("?", &DebugStub::GetStopReason),
		std::make_pair("m", &DebugStub::ReadMemory),
		std::make_pair("vCont", &DebugStub::Resume),
		std::make_pair("qC", &DebugStub::QueryCurrentThread),
		std::make_pair("qfThreadInfo", &DebugStub::QueryThreadList),
		std::make_pair("qsThreadInfo", &DebugStub::QueryThreadList),
		std::make_pair("qOffsets", &DebugStub::QuerySectionOffsets),
		std::make_pair("qHostInfo", &DebugStub::QueryHostInformation),
		std::make_pair("qRegisterInfo", &DebugStub::QueryRegisterInformation),
	};
	
	DebugStub::DebugStub(const std::string& path)
	: executablePath(path)
	{ }
	
	void DebugStub::StreamMain(Classix::DebugStub* self)
	{
		std::string output;
		while (true)
		{
			std::string command = self->stream->ReadCommand();
			
			TODO("StreamMain's dispatch algorithm isn't very beautiful");
			for (const auto& pair : commands)
			{
				if (command.compare(0, pair.first.length(), pair.first) == 0)
				{
					uint8_t commandResult = (self->*pair.second)(command, output);
					if (commandResult == 0)
					{
						self->stream->WriteAnswer(output);
					}
					else
					{
						self->stream->WriteAnswer(commandResult);
					}
					
					// this goto could be avoided if C++ had a loop ... else construct like Python :/
					goto nextCommand;
				}
			}
			
			self->stream->WriteAnswer("");
		nextCommand:;
		}
	}
	
	uint8_t DebugStub::SetOperationTargetThread(const std::string &commandString, std::string &output)
	{
		char command;
		int threadId;
		
		int assigned = sscanf(commandString.c_str(), "H%c%i", &command, &threadId);
		if (assigned != 2)
			return InvalidFormat;
		
		operationTargetThreads[command] = threadId;
		output = "OK";
		return NoError;
	}
	
	uint8_t DebugStub::GetStopReason(const std::string &commandString, std::string &output)
	{
		// assume always stopped for a breakpoint
		output = StringPrintf("S%02hhx", static_cast<uint8_t>(SIGTRAP));
		return NoError;
	}
	
	uint8_t DebugStub::ReadMemory(const std::string &commandString, std::string &outputString)
	{
		uint32_t address;
		uint32_t size;
		if (sscanf(commandString.c_str(), "m%x,%x", &address, &size) != 2)
		{
			return InvalidFormat;
		}
		
		size_t read = 0;
		size_t invalid = size;
		const uint8_t* data = nullptr;
		if (auto details = context->allocator->GetDetails(address))
		{
			uint32_t offset = context->allocator->GetAllocationOffset(address);
			read = std::max(details->Size() - offset, static_cast<size_t>(size));
			invalid = size - read;
			data = context->allocator->ToPointer<uint8_t>(address);
		}
		
		outputString.clear();
		outputString.reserve(size * 2 + 1);
		for (size_t i = 0; i < read; i++)
		{
			uint8_t value = data[i];
			outputString += hexgits[value >> 4];
			outputString += hexgits[value & 0xf];
		}
		
		outputString.resize(size * 2, 'e');
		return NoError;
	}
	
	uint8_t DebugStub::Resume(const std::string &commandString, std::string &output)
	{
		if (commandString == "vCont?")
		{
			output = "vCont;c;s;t";
			return NoError;
		}
		
		return NotImplemented;
	}
	
	uint8_t DebugStub::QuerySectionOffsets(const std::string &commandString, std::string &output)
	{
		return NotImplemented;
	}
	
	uint8_t DebugStub::QueryHostInformation(const std::string &commandString, std::string &output)
	{
		output = StringPrintf("cputype:%u;cpusubtype:%u;ostype:%s;vendor:%s;endian:%s;ptrsize:%u",
			CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_750, "classic", "fcloutier", "big", 4);
		
		return NoError;
	}
	
	uint8_t DebugStub::QueryCurrentThread(const std::string &commandString, std::string &output)
	{
		TODO("Threads aren't supported, returning QC1");
		output = "QC1";
		return NoError;
	}
	
	uint8_t DebugStub::QueryThreadList(const std::string &commandString, std::string &output)
	{
		// no support for fragmentation
		if (commandString[1] == 's')
		{
			output = "l";
		}
		else
		{
			TODO("Threads aren't supported, returning 1");
			output = "m1";
		}
		return NoError;
	}
	
	uint8_t DebugStub::QueryRegisterInformation(const std::string &commandString, std::string &output)
	{
		// registers are ordered this way: gpr, fpr, cr
		unsigned long regNumber = strtoul(commandString.data() + 13, nullptr, 16);
		
		int bitSize;
		char regName[5];
		const char* encoding;
		const char* format;
		const char* set;
		size_t offset;
		if (regNumber < 32)
		{
			set = "General Purpose Registers";
			bitSize = 32;
			encoding = "uint";
			format = "decimal";
			offset = regNumber * 4;
			snprintf(regName, sizeof regName, "r%lu", regNumber);
		}
		else if (regNumber < 64)
		{
			set = "Floating-Point Registers";
			bitSize = 64;
			encoding = "ieee754";
			format = "float";
			offset = 32 * 4 + (regNumber - 32) * 8;
			snprintf(regName, sizeof regName, "fr%lu", regNumber - 32);
		}
		else if (regNumber < 72)
		{
			set = "Condition Registers";
			bitSize = 8;
			encoding = "uint";
			format = "binary";
			offset = 32 * 4 + 32 * 8 + (regNumber - 64);
			snprintf(regName, sizeof regName, "cr%lu", regNumber - 64);
		}
		else if (regNumber < 75)
		{
			set = "System Registers";
			bitSize = 32;
			encoding = "uint";
			format = regNumber == 72 ? "binary" : "decimal";
			offset = 32 * 4 + 32 * 8 + 8 + (regNumber - 72) * 4;
			
			switch (regNumber - 72)
			{
				case 0: strncpy(regName, "xer", sizeof regName); break;
				case 1: strncpy(regName, "lr", sizeof regName); break;
				case 2: strncpy(regName, "ctr", sizeof regName); break;
				default: snprintf(regName, sizeof regName, "sr%lu", regNumber - 72); break;
			}
		}
		else
		{
			return InvalidData;
		}
		
		output = StringPrintf("name:%s;bitsize:%u;offset:%lu;encoding:%s;format:%s;set=%s;", regName, bitSize, offset, encoding, format, set);
		if (regNumber == 1)
		{
			output += "alt-name:sp;generic=sp;";
		}
		else if (regNumber == 2)
		{
			output += "alt-name:toc;";
		}
		else if (regNumber >= 3 && regNumber < 11)
		{
			output += "generic:arg";
			output += '1' + static_cast<char>(regNumber - 3);
			output += ';';
		}
		else if (regNumber == 73)
		{
			output += "generic:ra;";
		}
		return NoError;
	}
	
	void DebugStub::Accept(uint16_t port)
	{
		stream.reset(new ControlStream(ControlStream::Listen(port)));
		context.reset(new DebugContext(executablePath));
	}
	
	uint32_t DebugStub::RunToEnd()
	{
		std::thread streamThread(StreamMain, this);
		streamThread.join();
		
		uint32_t result = context->state.r3;
		stream.reset();
		context.reset();
		return result;
	}
}
