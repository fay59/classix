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

#include "FragmentManager.h"
#include "PEFLibraryResolver.h"
#include "NativeAllocator.h"
#include "FileMapping.h"
#include "Unmangle.h"
#include "BridgeLibraryResolver.h"
#include "MachineState.h"
#include "Disassembler.h"
#include "Interpreter.h"

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
		const uint32_t* instructions = reinterpret_cast<const uint32_t*>(section.Data);
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

int main(int argc, const char * argv[])
{
	if (argc != 3)
	{
		std::cerr << "usage: pefdump -o file # tries to load fragment" << std::endl;
		std::cerr << "       pefdump -e file # tries to list exports" << std::endl;
		std::cerr << "       pefdump -i file # tries to list imports" << std::endl;
		std::cerr << "       pefdump -d file # tries to disassemble code sections" << std::endl;
		std::cerr << "       pefdump -r file # tries to *gasp* run the file" << std::endl;
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
	}
	catch (std::exception& error)
	{
		std::cerr << "operation failed: " << error.what() << endline;
	}
}

