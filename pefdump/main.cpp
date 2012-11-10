//
//  main.cpp
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <memory>

#include "FragmentManager.h"
#include "PEFLibraryResolver.h"
#include "NativeAllocator.h"
#include "FileMapping.h"
#include "Unmangle.h"
#include "BridgeLibraryResolver.h"
#include "MachineState.h"
#include "Disassembler.h"
#include "Interpreter.h"

// be super-generous: apps on Mac OS 9, by default, have a 32 KB stack
// but we give them 1 MB since messing with ApplLimit has no effect
const uint32_t defaultStackSize = 0x100000;
const char endline = '\n';

static char classChars[] = {
	[PEF::SymbolClasses::CodeSymbol] = 'C',
	[PEF::SymbolClasses::DataSymbol] = 'D',
	[PEF::SymbolClasses::DirectData] = 'I',
	[PEF::SymbolClasses::FunctionPointer] = 'F',
	[PEF::SymbolClasses::GlueSymbol] = 'G'
};

static void loadTest(const std::string& path)
{
	MachineState state;
	MachineStateInit(&state);
	
	CFM::FragmentManager fragmentManager;
	CFM::PEFLibraryResolver pefResolver(Common::NativeAllocator::Instance, fragmentManager);
	ObjCBridge::BridgeLibraryResolver objcResolver(Common::NativeAllocator::Instance);
	
	fragmentManager.LibraryResolvers.push_back(&pefResolver);
	fragmentManager.LibraryResolvers.push_back(&objcResolver);
	
	fragmentManager.LoadContainer(path);
	
}

static void listExports(const std::string& path)
{
	Common::FileMapping mapping(path);
	PEF::Container container(Common::NativeAllocator::Instance, mapping.begin(), mapping.end());
	
	const PEF::ExportHashTable& exportTable = container.LoaderSection()->ExportTable;
	std::cout << exportTable.SymbolCount() << " exports" << endline;
	for (auto iter = exportTable.begin(); iter != exportTable.end(); iter++)
	{
		const PEF::ExportedSymbol* symbol = exportTable.Find(*iter);
		std::cout << '[' << classChars[symbol->Class] << "] " << Common::Unmangle(symbol->SymbolName) << endline;
	}
	std::cout << endline;
}

static void listImports(const std::string& path)
{
	Common::FileMapping mapping(path);
	PEF::Container container(Common::NativeAllocator::Instance, mapping.begin(), mapping.end());
	
	const PEF::LoaderSection* loader = container.LoaderSection();
	for (auto libIter = loader->LibrariesBegin(); libIter != loader->LibrariesEnd(); libIter++)
	{
		std::cout << libIter->Name << ':' << endline;
		for (auto& symbol : libIter->Symbols)
			std::cout << "  [" << classChars[symbol.Class] << "] " << Common::Unmangle(symbol.Name) << endline;
		std::cout << endline;
	}
}

static void disassemble(const std::string& path)
{
	Common::FileMapping mapping(path);
	PEF::Container container(Common::NativeAllocator::Instance, mapping.begin(), mapping.end());
	
	for (uint32_t i = 0; i < container.Size(); i++)
	{
		const PEF::InstantiableSection& section = container.GetSection(i);
		if (section.GetSectionType() != PEF::SectionType::Code && section.GetSectionType() != PEF::SectionType::ExecutableData)
			continue;
		
		std::cout << "Section " << i;
		if (section.Name.length() != 0)
			std::cout << " (" << section.Name << ")";
		std::cout << ": " << endline;
		
		const uint32_t totalInstructions = section.Size() / 4;
		const Common::UInt32* instructions = reinterpret_cast<const Common::UInt32*>(section.Data);
		PPCVM::Disassembler::DisassembledInstruction instruction;
		for (uint32_t i = 0; i < totalInstructions; i++)
		{
			PPCVM::Disassembler::Disassemble(instructions[i], instruction);
			std::cout << std::left << std::setw(10) << instruction.Opcode << ' ' << instruction.Arguments << endline;
		}
	}
}

static void run(const std::string& path)
{
	MachineState state;
	MachineStateInit(&state);
	
	CFM::FragmentManager fragmentManager;
	CFM::PEFLibraryResolver pefResolver(Common::NativeAllocator::Instance, fragmentManager);
	ObjCBridge::BridgeLibraryResolver objcResolver(Common::NativeAllocator::Instance);
	
	fragmentManager.LibraryResolvers.push_back(&pefResolver);
	fragmentManager.LibraryResolvers.push_back(&objcResolver);
	
	if (!fragmentManager.LoadContainer(path))
	{
		std::cerr << "could not load " << path << std::endl;
		return;
	}
	
	PPCVM::Execution::Interpreter interpreter(&state);
	
	auto resolver = fragmentManager.GetSymbolResolver(path);
	auto main = resolver->GetMainAddress();
	if (main.Universe != CFM::SymbolUniverse::PowerPC)
	{
		std::cerr << path << " successfully loaded, but main symbol is not a PPC symbol" << std::endl;
		return;
	}
	
	const void* mainAddress = reinterpret_cast<const void*>(main.Address);
	interpreter.Execute(mainAddress);
}

static void runMPW(const std::string& path, int argc, const char* argv[], const char* envp[])
{
	CFM::FragmentManager fragmentManager;
	CFM::PEFLibraryResolver pefResolver(Common::NativeAllocator::Instance, fragmentManager);
	ObjCBridge::BridgeLibraryResolver objcResolver(Common::NativeAllocator::Instance);
	
	fragmentManager.LibraryResolvers.push_back(&pefResolver);
	fragmentManager.LibraryResolvers.push_back(&objcResolver);
	
	if (!fragmentManager.LoadContainer(path))
	{
		std::cerr << "could not load " << path << std::endl;
		return;
	}
	
	auto resolver = fragmentManager.GetSymbolResolver(path);
	auto main = resolver->GetMainAddress();
	if (main.Universe != CFM::SymbolUniverse::PowerPC)
	{
		std::cerr << path << " successfully loaded, but main symbol is not a PPC symbol" << std::endl;
		return;
	}
	
	const PEF::TransitionVector* mainVector = reinterpret_cast<const PEF::TransitionVector*>(main.Address);
	uint32_t startAddress = mainVector->EntryPoint;
	
	Common::AutoAllocation stackRef = Common::NativeAllocator::Instance->AllocateAuto(defaultStackSize);
	uint8_t* stack = static_cast<uint8_t*>(*stackRef);
	
	MachineState state;
	MachineStateInit(&state);
	
	state.r1 = reinterpret_cast<intptr_t>(stack + defaultStackSize - 12);
	state.r2 = mainVector->TableOfContents;
	
	// TODO argv and envp need to be moved to a safe place of the address space
	state.r29 = argc;
	state.r30 = reinterpret_cast<intptr_t>(argv);
	state.r31 = reinterpret_cast<intptr_t>(envp);
	
	PPCVM::Execution::Interpreter interpreter(&state);
	
	interpreter.Execute(reinterpret_cast<const void*>(startAddress));
	std::cout << "Execution returned " << state.r3 << std::endl;
}

int main(int argc, const char* argv[], const char* envp[])
{
	if (argc != 3)
	{
		std::cerr << "usage: pefdump -o file # tries to load fragment" << std::endl;
		std::cerr << "       pefdump -e file # tries to list exports" << std::endl;
		std::cerr << "       pefdump -i file # tries to list imports" << std::endl;
		std::cerr << "       pefdump -d file # tries to disassemble code sections" << std::endl;
		std::cerr << "       pefdump -r file # tries to *gasp* run the file" << std::endl;
		std::cerr << "       pefdump -mpw file # tries to run the file as a MPW executable" << std::endl;
		return 1;
	}
	
	std::string mode = argv[1];
	std::string path = argv[2];
	
	try
	{
		if (mode == "-o")
			loadTest(path);
		else if (mode == "-e")
			listExports(path);
		else if (mode == "-i")
			listImports(path);
		else if (mode == "-d")
			disassemble(path);
		else if (mode == "-r")
			run(path);
		else if (mode == "-mpw")
			runMPW(path, argc - 3, argv + 3, envp);
	}
	catch (std::exception& error)
	{
		std::cerr << "operation failed: " << error.what() << endline;
	}
}

