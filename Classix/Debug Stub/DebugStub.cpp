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

#include <sstream>
#include <iomanip>
#include <thread>
#include <algorithm>
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

using namespace std;
using namespace Common;

namespace
{
	enum DebugStubErrorCodes : uint8_t
	{
		NoError,
		NotImplemented,
		InvalidFormat,
		InvalidData,
		InvalidTarget,
		TargetKilled,
		
		NoReply = 0xff
	};
	
	bool HasPrefix(const string& longer, const string& prefix)
	{
		return longer.compare(0, prefix.length(), prefix) == 0;
	}
	
	string StringPrintf(const char* format, ...) __attribute__((format(printf, 1, 2)));
	string StringPrintf(const char* format, ...)
	{
		unique_ptr<char, decltype(free)&> freeResult(nullptr, free);
		
		char* result;
		va_list ap;
		va_start(ap, format);
		vasprintf(&result, format, ap);
		va_end(ap);
		
		freeResult.reset(result);
		string stringResult = result;
		return stringResult;
	}
	
	uint8_t stopSignals[] = {
		[static_cast<size_t>(StopReason::InterruptTrap)] = SIGTRAP,
		[static_cast<size_t>(StopReason::AccessViolation)] = SIGSEGV,
		[static_cast<size_t>(StopReason::InvalidInstruction)] = SIGILL,
	};
	
	const char hexgits[] = "0123456789abcdef";
}

DebugContext::DebugContext(const string& executable, uint32_t pid)
: allocator(new NativeAllocator), threads(*allocator), managers(*allocator, threads), pid(pid)
{
	using namespace ClassixCore;
	
	BundleLibraryResolver* bundleResolver = new BundleLibraryResolver(*allocator, managers);
	bundleResolver->AllowLibrary("InterfaceLib");
	bundleResolver->AllowLibrary("ControlStripLib");
	resolvers.emplace_back(bundleResolver);
	
	DlfcnLibraryResolver* dlfcnResolver = new DlfcnLibraryResolver(*allocator, managers);
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
		throw logic_error("Couldn't load executable");
}

void DebugContext::Start(shared_ptr<WaitQueue<string>>& sink)
{
	using namespace CFM;
	
	StackPreparator stackPrep;
	
	// Do a pass to find all init symbols and the main symbol. We need this to be able to create the main thread
	// before the initializer threads.
	vector<ResolvedSymbol> initSymbols;
	ResolvedSymbol mainSymbol(SymbolUniverse::LostInTimeAndSpace, "??", 0);
	for (auto& pair : fragmentManager)
	{
		SymbolResolver* resolver = pair.second;
		auto entryPoints = resolver->GetEntryPoints();
		for (auto& entryPoint : entryPoints)
		{
			if (entryPoint.Name == SymbolResolver::InitSymbolName && entryPoint.Universe != SymbolUniverse::LostInTimeAndSpace)
			{
				initSymbols.push_back(entryPoint);
			}
			else if (entryPoint.Name == SymbolResolver::MainSymbolName)
			{
				assert(mainSymbol.Universe == SymbolUniverse::LostInTimeAndSpace && "Already assigned a main symbol");
				mainSymbol = entryPoint;
			}
		}
	}
	
	assert(mainSymbol.Universe != SymbolUniverse::LostInTimeAndSpace);
	
	// Start (but don't run) the main thread. We create it because once a thread is created, the DebugThreadManager
	// will stop its main loop if it reaches 0 threads again.
	const PEF::TransitionVector* vector = allocator->ToPointer<PEF::TransitionVector>(mainSymbol.Address);
	auto& thread = threads.StartThread(stackPrep, StackPreparator::DefaultStackSize, *vector, false);
	globalTargetThread = thread.GetThreadId();
	
	// Run the initializers
	for (const ResolvedSymbol& entryPoint : initSymbols)
	{
		const PEF::TransitionVector* transition = allocator->ToPointer<PEF::TransitionVector>(entryPoint.Address);
		threads.StartThread(stackPrep, StackPreparator::DefaultStackSize, *transition, true);
	}
	
	// wait for all initializers to complete; expect two $ThreadStatusChanged commands per initializer
	size_t infoCountRemaining = initSymbols.size() * 2;
	auto& threadSink = threads.CommandSink();
	while (infoCountRemaining > 0)
	{
		string command = threadSink->TakeOne();
		assert(HasPrefix(command, "$ThreadStatusChanged"));
		infoCountRemaining--;
	}
	
	// wait for the thread start notification, then replace the thread manager's event sink with our event sink
	string command = threadSink->TakeOne();
	assert(HasPrefix(command, "$ThreadStatusChanged"));
	threads.CommandSink() = sink;
}

const unordered_map<string, DebugStub::RemoteCommand> DebugStub::commands = {
	make_pair("H", &DebugStub::SetOperationTargetThread),
	make_pair("?", &DebugStub::GetStopReason),
	make_pair("m", &DebugStub::ReadMemory),
	make_pair("vCont", &DebugStub::ThreadResume),
	make_pair("c", &DebugStub::Continue),
	make_pair("k", &DebugStub::Kill),
	make_pair("p", &DebugStub::ReadSingleRegister),
	make_pair("Z", &DebugStub::SetBreakpoint),
	make_pair("z", &DebugStub::RemoveBreakpoint),
	
	make_pair("qC", &DebugStub::QueryCurrentThread),
	make_pair("qfThreadInfo", &DebugStub::QueryThreadList),
	make_pair("qsThreadInfo", &DebugStub::QueryThreadList),
	make_pair("qOffsets", &DebugStub::QuerySectionOffsets),
	make_pair("qHostInfo", &DebugStub::QueryHostInformation),
	make_pair("qRegisterInfo", &DebugStub::QueryRegisterInformation),
	make_pair("qThreadStopInfo", &DebugStub::GetStopReason),
	make_pair("qProcessInfo", &DebugStub::QueryProcessInformation),
	make_pair("qMemoryRegionInfo", &DebugStub::QueryMemoryRegionInfo),
	
	make_pair("$StreamClosed", &DebugStub::PrivateStreamClosed),
	make_pair("$ThreadStatusChanged", &DebugStub::GetStopReason),
};

DebugStub::DebugStub(const string& path)
: executablePath(path), sink(new WaitQueue<string>)
{ }

uint8_t DebugStub::SetOperationTargetThread(const string &commandString, string &output)
{
	char command;
	DebugThreadManager::ThreadId threadId;
	
	int assigned = sscanf(commandString.c_str(), "H%c%x", &command, &threadId);
	if (assigned != 2)
		return InvalidFormat;
	
	context->operationTargetThreads[command] = threadId;
	output = "OK";
	return NoError;
}

uint8_t DebugStub::GetStopReason(const string &commandString, string &output)
{
	if (!context) return TargetKilled;
	
	// all threads
	if (commandString == "?")
	{
		output.clear();
		size_t threadCount = 0;
		context->threads.ForEachThread([&output, &threadCount] (ThreadContext& context)
		{
			size_t reasonIndex = static_cast<size_t>(context.GetStopReason());
			output += StringPrintf("S%02hhxthread:%x;", stopSignals[reasonIndex], context.GetThreadId());
			threadCount++;
		});
		
		return NoError;
	}
	
	// just this thread: either qThreadStopInfo or $ThreadStatusChanged
	DebugThreadManager::ThreadId handle;
	if (HasPrefix(commandString, "qThreadStopInfo"))
	{
		int assigned = sscanf(commandString.c_str(), "qThreadStopInfo%x", &handle);
		if (assigned != 1)
			return InvalidFormat;
		
		if (ThreadContextPointer threadContext = context->threads.GetThread(handle))
		{
			size_t reasonIndex = static_cast<size_t>(threadContext->GetStopReason());
			output = StringPrintf("S%02hhxthread:%x;", stopSignals[reasonIndex], handle);
			return NoError;
		}
		else
		{
			output = "W";
			return NoError;
		}
	}
	else if (HasPrefix(commandString, "$ThreadStatusChanged"))
	{
		int assigned = sscanf(commandString.c_str(), "$ThreadStatusChanged;%x", &handle);
		if (assigned != 1)
			return InvalidFormat;
		
		if (ThreadContextPointer threadContext = context->threads.GetThread(handle))
		{
			size_t reasonIndex = static_cast<size_t>(threadContext->GetStopReason());
			output = StringPrintf("S%02hhxthread:%x;", stopSignals[reasonIndex], handle);
			return NoError;
		}
		else if (context->threads.HasCompleted())
		{
			uint32_t exitCode = context->threads.GetLastExitCode();
			output = StringPrintf("W%02hhx;pid=%x;", static_cast<uint8_t>(exitCode), context->pid);
			return NoError;
		}
		return NoReply;
	}
	return InvalidData;
}

uint8_t DebugStub::ThreadResume(const string &commandString, string &output)
{
	if (!context) return TargetKilled;
	
	if (commandString == "vCont?")
	{
		output = "vCont;c;s;t";
		return NoError;
	}
	else
	{
		char action;
		int charCount;
		DebugThreadManager::ThreadId targetThread;
		const char* actions = commandString.c_str() + 5;
		while (sscanf(actions, ";%c:%x%n", &action, &targetThread, &charCount) == 2) // %n doesn't count
		{
			if (ThreadContextPointer threadContext = context->threads.GetThread(targetThread))
			{
				switch (action)
				{
					case 's': threadContext->Perform(RunCommand::StepOver); break;
					case 't': threadContext->Perform(RunCommand::SingleStep); break;
					case 'c': threadContext->Perform(RunCommand::Continue); break;
					default:
						return NotImplemented;
				}
			}
			actions += charCount;
		}
		
		// no immediate reply; reply as we stop again
		return NoReply;
	}
}

uint8_t DebugStub::ReadMemory(const string &commandString, string &outputString)
{
	if (!context) return TargetKilled;
	
	uint32_t address;
	uint32_t size;
	if (sscanf(commandString.c_str(), "m%x,%x", &address, &size) != 2)
	{
		return InvalidFormat;
	}
	
	auto inhibitedBreakpoints = context->threads.BreakpointSet()->InhibitBreakpoints();
	stringstream ss;
	uint32_t written = 0;
	while (written < size)
	{
		if (shared_ptr<const AllocationDetails> details = context->allocator->GetDetails(address))
		{
			uint32_t offset = context->allocator->GetAllocationOffset(address);
			uint32_t intersectingSize = details->Size() - offset;
			const uint8_t* data = context->allocator->ToPointer<uint8_t>(address);
			for (uint32_t i = 0; i < intersectingSize && written < size; i++, written++)
			{
				ss << hexgits[data[i] >> 4];
				ss << hexgits[data[i] & 0xf];
			}
		}
		
		uint32_t nextAddress = context->allocator->GetUpperAllocation(address + written);
		for (uint32_t i = address; i < nextAddress && written < size; i++, written++)
		{
			ss << "EE";
		}
		address = nextAddress;
	}
	
	outputString = ss.str();
	return NoError;
}

uint8_t DebugStub::Kill(const string &commandString, string &outputString)
{
	if (!context) return TargetKilled;
	
	// Technically, there's a race condition here where a thread created just before Kill is called
	// would survive. In fact, this is very unlikely to be an issue because Mac OS Classic threads are
	// rather rare; but if this seems to happen, remember to check this place.
	// (We would probably need to add a field in the thread manager to prevent thread creation once killing is
	// initiated, or something of that effect.)
	
	size_t threadCount = 0;
	shared_ptr<WaitQueue<string>> killQueue(new WaitQueue<string>);
	context->threads.CommandSink() = killQueue;
	context->threads.ForEachThread([&threadCount] (ThreadContext& context)
	{
		context.Kill();
		threadCount++;
	});
	
	for (size_t i = 0; i < threadCount; i++)
	{
		string command = killQueue->TakeOne();
		assert(HasPrefix(command, "$ThreadStatusChanged"));
	}
	
	assert(context->threads.HasCompleted());
	outputString = StringPrintf("W%x;pid=%x;", context->threads.GetLastExitCode(), context->pid);
	context.reset();
	return NoError;
}

uint8_t DebugStub::Continue(const string &commandString, string &outputString)
{
	if (!context) return TargetKilled;
	
	context->threads.ForEachThread([] (ThreadContext& context) { context.Perform(RunCommand::Continue); });
	
	outputString.clear();
	
	return NoReply;
}

uint8_t DebugStub::ReadSingleRegister(const string &commandString, string &outputString)
{
	if (!context) return TargetKilled;
	
	ThreadContextPointer thread = context->threads.GetThread(context->globalTargetThread);
	if (thread == nullptr)
	{
		return InvalidTarget;
	}
	
	size_t registerNumber;
	if (sscanf(commandString.c_str(), "p%zx", &registerNumber) != 1)
	{
		return InvalidFormat;
	}
	
	uint8_t length;
	uint64_t value;
	switch (registerNumber)
	{
		case 0 ... 31: // rN
			value = thread->machineState.gpr[registerNumber];
			length = 8;
			break;
			
		case 32 ... 63: // frN
			value = HostToBig<uint64_t>::Swap(*reinterpret_cast<uint64_t*>(&thread->machineState.fpr[registerNumber - 32]));
			length = 16;
			break;
			
		case 64 ... 71: // crN
			value = thread->machineState.cr[registerNumber - 64];
			length = 2;
			break;
			
		case 72: // xer
			value = thread->machineState.xer;
			length = 8;
			break;
			
		case 73: // lr
			value = thread->machineState.lr;
			length = 8;
			break;
			
		case 74: // ctr
			value = thread->machineState.ctr;
			length = 8;
			break;
			
		case 75: // pc
			assert(thread->GetThreadState() == ThreadState::Stopped && "PC cannot be queried on a running thread");
			value = thread->pc;
			length = 8;
			break;
			
		default:
			assert(false && "Unknown register");
			return InvalidData;
	}
	
	outputString = StringPrintf("%0*llx", length, value);
	return NoError;
}

uint8_t DebugStub::SetBreakpoint(const string &commandString, string &outputString)
{
	if (!context) return TargetKilled;
	
	uint8_t type;
	uint32_t address;
	uint8_t kind;
	if (sscanf(commandString.c_str(), "Z%hhu,%x,%hhu", &type, &address, &kind) == 3)
	{
		// type 0: memory breakpoint; kind 4: 4-byte breakpoint
		if (type == 0 && kind == 4)
		{
			try
			{
				UInt32* breakpointAddress = context->allocator->ToPointer<UInt32>(address);
				context->threads.BreakpointSet()->SetBreakpoint(breakpointAddress);
				outputString = "OK";
				return NoError;
			}
			catch (AccessViolationException& ex)
			{
				return InvalidData;
			}
		}
		else
		{
			return NotImplemented;
		}
	}
	
	return InvalidData;
}

uint8_t DebugStub::RemoveBreakpoint(const string &commandString, string &outputString)
{
	if (!context) return TargetKilled;
	
	uint8_t type;
	uint32_t address;
	uint8_t kind;
	if (sscanf(commandString.c_str(), "z%hhu;%x;%hhu", &type, &address, &kind) == 3)
	{
		// type 0: memory breakpoint; kind 4: 4-byte breakpoint
		if (type == 0 && kind == 4)
		{
			try
			{
				UInt32* breakpointAddress = context->allocator->ToPointer<UInt32>(address);
				context->threads.BreakpointSet()->RemoveBreakpoint(breakpointAddress);
				outputString = "OK";
				return NoError;
			}
			catch (AccessViolationException& ex)
			{
				return InvalidData;
			}
		}
		else
		{
			return NotImplemented;
		}
	}
	
	return InvalidData;
}

uint8_t DebugStub::QuerySectionOffsets(const string &commandString, string &output)
{
	return NotImplemented;
}

uint8_t DebugStub::QueryHostInformation(const string &commandString, string &output)
{
	// for some reason, someone decided that qHostInfo should use base 10
	// but qProcessInfo should use base 16
	output = StringPrintf("cputype:%u;cpusubtype:%u;ostype:%s;vendor:%s;endian:%s;ptrsize:%u;",
		CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_750, "unknown", "unknown", "big", 4);
	
	return NoError;
}

uint8_t DebugStub::QueryCurrentThread(const string &commandString, string &output)
{
	if (!context) return TargetKilled;
	
	output = StringPrintf("QC%x", context->globalTargetThread);
	return NoError;
}

uint8_t DebugStub::QueryThreadList(const string &commandString, string &output)
{
	if (!context) return TargetKilled;
	
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
			output += StringPrintf("%x,", context.GetThreadId());
		});
		output.resize(output.length() - 1);
	}
	return NoError;
}

uint8_t DebugStub::QueryRegisterInformation(const string &commandString, string &output)
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
	else if (regNumber < 76)
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
			case 3: strncpy(regName, "pc", sizeof regName); break;
			default: snprintf(regName, sizeof regName, "sr%lu", regNumber - 72); break;
		}
	}
	else
	{
		return InvalidData;
	}
	
	output = StringPrintf("name:%s;bitsize:%u;offset:%lu;encoding:%s;format:%s;set:%s;", regName, bitSize, offset, encoding, format, set);
	if (regNumber == 1)
	{
		output += "alt-name:sp;generic:fp;";
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
	else if (regNumber == 75)
	{
		output += "generic:pc;";
	}
	return NoError;
}

uint8_t DebugStub::QueryProcessInformation(const string &commandString, string &output)
{
	if (!context) return TargetKilled;
	
	output = StringPrintf("cputype:%x;cpusubtype:%x;ostype:%s;vendor:%s;endian:%s;pid:%x;ptrsize:%u;",
		CPU_TYPE_POWERPC, CPU_SUBTYPE_POWERPC_750, "unknown", "unknown", "big", context->pid, 4);
	
	return NoError;
}

uint8_t DebugStub::QueryMemoryRegionInfo(const std::string &commandString, std::string &output)
{
	if (!context) return TargetKilled;
	
	uint32_t address;
	if (sscanf(commandString.c_str(), "qMemoryRegionInfo:%x", &address) == 1)
	{
		if (auto details = context->allocator->GetDetails(address))
		{
			uint32_t offset = context->allocator->GetAllocationOffset(address);
			uint32_t size = static_cast<uint32_t>(details->Size());
			output = StringPrintf("start:%x;size:%x;permissions:%s;", address - offset, size, "rwx");
			return NoError;
		}
		else
		{
			return InvalidData;
		}
	}
	
	return NotImplemented;
}

uint8_t DebugStub::PrivateStreamClosed(const string&, string& output)
{
	Kill("k", output);
	context.reset();
	stream.reset();
	return NoError;
}

void DebugStub::Accept(uint16_t port)
{
	uint32_t pid = (getpid() * 10) | runCount;
	context.reset(new DebugContext(executablePath, pid));
	stream.reset(new ControlStream(ControlStream::Listen(sink, port)));
	runCount++;
}

void DebugStub::SinkMain()
{
	try
	{
		stream->ConsumeReadEvents();
	}
	catch (...)
	{
		sink->PutOne("$StreamClosed");
	}
}

void DebugStub::Execute()
{
	// The thread manager uses a different wait queue at first, so the command stream can read commands
	// and send back acknowledges right away
	thread getThreadEvents(&DebugThreadManager::ConsumeThreadEvents, &context->threads);
	thread readCommands(&DebugStub::SinkMain, this);
	
	context->Start(sink);
	
	uint8_t commandResult;
	string command, output;
	while (context || stream)
	{
		if (sink->TakeOne(command, chrono::milliseconds(500)))
		{
			auto iter = find_if(commands.begin(), commands.end(), [&command] (const pair<string, RemoteCommand>& pair)
			{
				return HasPrefix(command, pair.first);
			});
			
#if DEBUG
			output = "<no reply>";
#else
			output.clear();
#endif
			if (iter == commands.end())
			{
#if DEBUG
				cerr << "*** $" << command << "$: not implemented" << endl;
				output.clear();
#endif
				commandResult = NoError;
			}
			else
			{
				commandResult = (this->*iter->second)(command, output);
			}
			
			if (stream)
			{
				if (commandResult == NoError)
				{
					stream->WriteAnswer(output);
				}
				else if (commandResult != NoReply)
				{
					stream->WriteAnswer(commandResult);
				}
			}
		}
	}
	
	// if we're stuck on this guy, check for the race condition where a thread is
	// created right before Kill is called, and see the Kill method for ideas on how to fix it
	getThreadEvents.join();
	readCommands.join();
}
