//
//  main.cpp
//  Classix
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
#include "DyldLibraryResolver.h"
#include "VirtualMachine.h"
#include "NativeAllocator.h"
#include "FileMapping.h"
#include "Unmangle.h"
#include "Disassembler.h"

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
	ClassixCore::DyldLibraryResolver dyldResolver(Common::NativeAllocator::Instance);
	
	dyldResolver.RegisterLibrary("StdCLib");
	
	fragmentManager.LibraryResolvers.push_back(&pefResolver);
	fragmentManager.LibraryResolvers.push_back(&dyldResolver);
	
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
	throw std::logic_error("direct main invocation is not supported");
}

static void runMPW(const std::string& path, int argc, const char* argv[], const char* envp[])
{
	ClassixCore::DyldLibraryResolver dyldResolver(Common::NativeAllocator::Instance);
	dyldResolver.RegisterLibrary("StdCLib");
	
	Classix::VirtualMachine vm(Common::NativeAllocator::Instance);
	vm.AddLibraryResolver(dyldResolver);
	
	auto stub = vm.LoadMainContainer(path);
	stub(argc, argv, envp);
}

int main(int argc, const char* argv[], const char* envp[])
{
	if (argc != 3)
	{
		std::cerr << "usage: Classix -o file # tries to load fragment" << std::endl;
		std::cerr << "       Classix -e file # tries to list exports" << std::endl;
		std::cerr << "       Classix -i file # tries to list imports" << std::endl;
		std::cerr << "       Classix -d file # tries to disassemble code sections" << std::endl;
		std::cerr << "       Classix -r file # tries to *gasp* run the file" << std::endl;
		std::cerr << "       Classix -mpw file # tries to run the file as a MPW executable" << std::endl;
		return 1;
	}
	
	// keeping this reference around to help printing memory areas
	Common::NativeAllocator* allocator = Common::NativeAllocator::Instance;
	(void)allocator;
	
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

