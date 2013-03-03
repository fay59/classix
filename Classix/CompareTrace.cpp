//
// CompareTrace.cpp
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

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "VirtualMachine.h"
#include "NativeAllocator.h"
#include "DlfcnLibraryResolver.h"

// BIG ASSUMPTION:
// this way to proceed will fail if there's more than one executable section
class MacsBugTrace
{
	
private:
	std::string filePath;
	std::ifstream input;
	uint32_t mainAddress = 0;
	
public:
	enum class StepMode
	{
		Unknown, StepInto, StepOver
	};
	
	StepMode mode;
	uint32_t pc;
	uint32_t pcOffset;
	std::string instructionName;
	std::string arguments;
	std::string comment;
	uint32_t opcode;
	uint32_t linesRead;
	
	MacsBugTrace(const std::string& path)
	: filePath(path), input(path, std::ios_base::in | std::ios_base::binary)
	{
		mode = StepMode::Unknown;
		linesRead = 0;
	}
	
	bool ReadNext()
	{
		std::string line;
		std::getline(input, line);
		linesRead++;
		
		if (line.length() == 0 && input.eof())
			return false;
		
		if (line == "Closing log")
			return false;
		
		if (line == "Step (into)")
		{
			mode = StepMode::StepInto;
			return ReadNext();
		}
		
		if (line == "Step (over)")
		{
			mode = StepMode::StepOver;
			return ReadNext();
		}
		
		// skip procedure names
		if (line.substr(0, 2) == "  " && line[2] != ' ')
			return ReadNext();
		
		// parce actual instruction
		
		std::stringstream ss(line, std::ios_base::in);
		ss >> std::hex;
		ss >> pc >> instructionName >> arguments;
		
		if (arguments == "|")
		{
			arguments = "";
		}
		else
		{
			char token;
			ss >> token;
			if (token == ';')
			{
				std::stringstream commentStream;
				std::string commentPart;
				while (commentPart[0] != '|')
				{
					ss >> commentPart;
					commentStream << commentPart << ' ';
				}
			}
			else
			{
				assert(token == '|' && "Expected | delimiter");
			}
		}
		
		ss >> opcode;
		
		if (mainAddress == 0)
			mainAddress = pc;
		
		pcOffset = pc - mainAddress;
		
		return true;
	}
};

int compareTrace(const std::string& path, const std::string& tracePath)
{
	Common::NativeAllocator allocator;
	ClassixCore::DlfcnLibraryResolver dlfcnResolver(&allocator);
	dlfcnResolver.RegisterLibrary("StdCLib");
	
	Classix::VirtualMachine vm(&allocator);
	vm.AddLibraryResolver(dlfcnResolver);
	
	Classix::MainStub stub = vm.LoadMainContainer(path);
	std::string* nullEnvp = nullptr;
	Classix::ProgramControlHandle handle = stub.Instantiate(&path, &path + 1, nullEnvp, nullEnvp);
	
	// BIG ASSUMPTION: `main` is the first function in the instantiable section
	// (the right way to do this would be to find the jump destination from __start)
	uint32_t mainAddress = handle.pc - vm.allocator->GetAllocationOffset(handle.pc);
	handle.RunTo(mainAddress);
	
	uint32_t lastPC = 0;
	uint32_t executedInstructions = 0;
	MacsBugTrace trace(tracePath);
	while (trace.ReadNext())
	{
		std::cout << std::right << std::hex << std::setw(8) << std::setfill('0') << mainAddress + trace.pcOffset << ' ' << trace.opcode << ' ';
		std::cout << std::left << std::setfill(' ') << std::setw(8) << trace.instructionName << trace.arguments << std::endl;
		
		uint32_t offset = handle.pc - mainAddress;
		if (trace.pcOffset != offset)
		{
			std::cerr << "*** Problem after " << std::dec << executedInstructions << " instructions (pc=" << std::hex << lastPC << ", read " << std::dec << trace.linesRead << " lines):" << std::endl;
			std::cerr << "*** Branched to " << std::hex << handle.pc << "; should have gone to " << mainAddress + trace.pcOffset << std::endl;
			return -1;
		}
		
		if (trace.mode == MacsBugTrace::StepMode::StepOver)
			handle.StepOver();
		else if (trace.mode == MacsBugTrace::StepMode::StepInto)
			handle.StepInto();
		else
		{
			std::cerr << "*** undetermined step mode" << std::endl;
			return -2;
		}
		
		lastPC = handle.pc;
		executedInstructions++;
	}
	
	return 0;
}