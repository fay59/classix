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
#include <unistd.h>

#include "DebugStub.h"
#include "NativeAllocator.h"
#include "BundleLibraryResolver.h"
#include "DlfcnLibraryResolver.h"
#include "PEFLibraryResolver.h"
#include "DummyLibraryResolver.h"
#include "DebugThreadManager.h"
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
	
	uint8_t stopSignals[] = {
		[static_cast<size_t>(StopReason::InterruptTrap)] = SIGTRAP,
		[static_cast<size_t>(StopReason::AccessViolation)] = SIGSEGV,
		[static_cast<size_t>(StopReason::InvalidInstruction)] = SIGILL,
	};
	
	const char hexgits[] = "0123456789abcdef";
}

namespace Classix
{
	DebugContext::DebugContext(const std::string& executable)
	: allocator(new Common::NativeAllocator), threads(*allocator), managers(*allocator, threads)
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
		
		for (auto& ptr : resolvers)
			fragmentManager.LibraryResolvers.push_back(ptr.get());
		
		if (!fragmentManager.LoadContainer(executable))
			throw std::logic_error("Couldn't load executable");
	}
	
	void DebugContext::Start(std::shared_ptr<WaitQueue<std::string>>& sink)
	{
		Common::StackPreparator stackPrep;
		
		// Do a pass to find all init symbols and the main symbol. We need this to be able to create the main thread
		// before the initializer threads.
		std::vector<CFM::ResolvedSymbol> initSymbols;
		CFM::ResolvedSymbol mainSymbol(CFM::SymbolUniverse::LostInTimeAndSpace, "??", 0);
		for (auto& pair : fragmentManager)
		{
			CFM::SymbolResolver* resolver = pair.second;
			auto entryPoints = resolver->GetEntryPoints();
			for (auto& entryPoint : entryPoints)
			{
				if (entryPoint.Name == CFM::SymbolResolver::InitSymbolName && entryPoint.Universe != CFM::SymbolUniverse::LostInTimeAndSpace)
				{
					initSymbols.push_back(entryPoint);
				}
				else if (entryPoint.Name == CFM::SymbolResolver::MainSymbolName)
				{
					assert(mainSymbol.Universe == CFM::SymbolUniverse::LostInTimeAndSpace && "Already assigned a main symbol");
					mainSymbol = entryPoint;
				}
			}
		}
		
		assert(mainSymbol.Universe != CFM::SymbolUniverse::LostInTimeAndSpace);
		
		// Start (but don't run) the main thread. We create it because the DebugThreadManager will stop
		// its run loop if it reaches 0 threads.
		const PEF::TransitionVector* vector = allocator->ToPointer<PEF::TransitionVector>(mainSymbol.Address);
		auto& thread = threads.StartThread(stackPrep, Common::StackPreparator::DefaultStackSize, *vector, false);
		globalTargetThread = thread.GetThreadId();
		
		// Run the initializers
		for (const CFM::ResolvedSymbol& entryPoint : initSymbols)
		{
			const PEF::TransitionVector* transition = allocator->ToPointer<PEF::TransitionVector>(entryPoint.Address);
			threads.StartThread(stackPrep, Common::StackPreparator::DefaultStackSize, *transition, true);
		}
		
		// wait for all initializers to complete; expect 2 qThreadStopInfo commands per initializer
		size_t infoCountRemaining = initSymbols.size() * 2;
		auto threadSink = threads.GetCommandSink();
		while (infoCountRemaining > 0)
		{
			std::string command = threadSink->TakeOne();
			assert(command == "qThreadStopInfo");
			infoCountRemaining--;
		}
		
		// wait for the thread start notification, then replace the sink with the context sink
		std::string command = threadSink->TakeOne();
		assert(command == "qThreadStopInfo");
		threads.SetCommandSink(sink);
	}
	
	const std::unordered_map<std::string, DebugStub::RemoteCommand> DebugStub::commands = {
		std::make_pair("H", &DebugStub::SetOperationTargetThread),
		std::make_pair("?", &DebugStub::GetStopReason),
		std::make_pair("m", &DebugStub::ReadMemory),
		std::make_pair("vCont", &DebugStub::Resume),
		std::make_pair("qC", &DebugStub::QueryCurrentThread),
		std::make_pair("k", &DebugStub::Kill),
		std::make_pair("qfThreadInfo", &DebugStub::QueryThreadList),
		std::make_pair("qsThreadInfo", &DebugStub::QueryThreadList),
		std::make_pair("qOffsets", &DebugStub::QuerySectionOffsets),
		std::make_pair("qHostInfo", &DebugStub::QueryHostInformation),
		std::make_pair("qRegisterInfo", &DebugStub::QueryRegisterInformation),
		std::make_pair("qThreadStopInfo", &DebugStub::GetStopReason),
		std::make_pair("qProcessInfo", &DebugStub::QueryProcessInformation),
	};
	
	DebugStub::DebugStub(const std::string& path)
	: executablePath(path), sink(new WaitQueue<std::string>)
	{ }
	
	uint8_t DebugStub::SetOperationTargetThread(const std::string &commandString, std::string &output)
	{
		char command;
		size_t encodedId;
		
		int assigned = sscanf(commandString.c_str(), "H%c%zx", &command, &encodedId);
		if (assigned != 2)
			return InvalidFormat;
		
		// super evil cast! gotta do what we gotta do: thread IDs need to be serializable to text
		context->operationTargetThreads[command] = reinterpret_cast<pthread_t>(encodedId);
		output = "OK";
		return NoError;
	}
	
	uint8_t DebugStub::GetStopReason(const std::string &commandString, std::string &output)
	{
		if (commandString == "?")
		{
			output.clear();
			context->threads.ForEachThread([&output] (ThreadContext& context)
			{
				size_t reasonIndex = static_cast<size_t>(context.stopReason.load());
				intptr_t serializableHandle = reinterpret_cast<intptr_t>(context.GetThreadId());
				output += StringPrintf("S%02hhxthread:%zx;", stopSignals[reasonIndex], serializableHandle);
			});
			return NoError;
		}
		else
		{
			// just this thread
			intptr_t handle;
			int assigned = sscanf(commandString.c_str(), "qThreadStopInfo%zx", &handle);
			if (assigned != 1)
				return InvalidFormat;
			
			ThreadContext* threadContext;
			if (context->threads.GetThread(reinterpret_cast<std::thread::native_handle_type>(handle), threadContext))
			{
				size_t reasonIndex = static_cast<size_t>(threadContext->stopReason.load());
				output = StringPrintf("S%02hhxthread:%zx;", stopSignals[reasonIndex], handle);
				return NoError;
			}
		}
		return InvalidData;
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
	
	uint8_t DebugStub::Kill(const std::string &commandString, std::string &outputString)
	{
		// Technically, there's a race condition here where a thread created just before Kill is called
		// would survive. In fact, this is very unlikely to be an issue because Mac OS Classic threads are
		// rather rare; but if this seems to happen, remember to check this place.
		
		size_t threadCount = 0;
		std::shared_ptr<WaitQueue<std::string>> killQueue(new WaitQueue<std::string>);
		context->threads.SetCommandSink(killQueue);
		context->threads.ForEachThread([&threadCount] (ThreadContext& context)
		{
			context.Kill();
			threadCount++;
		});
		
		for (size_t i = 0; i < threadCount; i++)
		{
			std::string command = killQueue->TakeOne();
			assert(command == "qThreadStopInfo");
		}
		
		assert(context->threads.ThreadCount() == 0);
		context.reset();
		outputString = "OK";
		return NoError;
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
		output = StringPrintf("QC%zx", reinterpret_cast<intptr_t>(context->globalTargetThread));
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
			output = "m";
			context->threads.ForEachThread([&output] (ThreadContext& context)
			{
				output += StringPrintf("%zx,", reinterpret_cast<intptr_t>(context.GetThreadId()));
			});
			output.resize(output.length() - 1);
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
	
	uint8_t DebugStub::QueryProcessInformation(const std::string &commandString, std::string &output)
	{
		if (uint8_t error = QueryHostInformation(commandString, output))
			return error;
		
		output += StringPrintf(";pid:%x;", getpid());
		return NoError;
	}
	
	void DebugStub::Accept(uint16_t port)
	{
		context.reset(new DebugContext(executablePath));
		stream.reset(new ControlStream(ControlStream::Listen(sink, port)));
	}
	
	void DebugStub::SinkMain()
	{
		try
		{
			stream->ConsumeReadEvents();
		}
		catch (...)
		{
			stream.reset();
			std::string dummy;
			Kill("k", dummy);
		}
	}
	
	void DebugStub::Execute()
	{
		// The thread manager uses a different wait queue at first, so the command stream can read commands
		// and send back acknowledges right away
		std::thread getThreadEvents(&DebugThreadManager::ConsumeThreadEvents, &context->threads);
		std::thread readCommands(&DebugStub::SinkMain, this);
		
		context->Start(sink);
		
		std::string command, output;
		while (context || stream)
		{
			bool gotOne = sink->TakeOne(command, std::chrono::milliseconds(500));
			if (gotOne)
			{
				TODO("StreamMain's dispatch algorithm isn't very beautiful");
				for (const auto& pair : commands)
				{
					if (command.compare(0, pair.first.length(), pair.first) == 0)
					{
						uint8_t commandResult = (this->*pair.second)(command, output);
						if (commandResult == 0)
						{
							stream->WriteAnswer(output);
						}
						else
						{
							stream->WriteAnswer(commandResult);
						}
						
						// this goto could be avoided if C++ had a loop ... else construct like Python :/
						goto nextCommand;
					}
				}
				
				stream->WriteAnswer("");
			}
		nextCommand:;
		}
		
		// if we're stuck on this guy, check for the race condition where a thread is created right before
		// Kill is called
		getThreadEvents.join();
		readCommands.join();
	}
}
