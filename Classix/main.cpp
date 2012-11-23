//
// main.cpp
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

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <dlfcn.h>

#include "FragmentManager.h"
#include "PEFLibraryResolver.h"
#include "PEFSymbolResolver.h"
#include "DyldLibraryResolver.h"
#include "VirtualMachine.h"
#include "NativeAllocator.h"
#include "FileMapping.h"
#include "Disassembler.h"
#include "NativeCall.h"
#include "FancyDisassembler.h"
#include "OStreamDisassemblyWriter.h"

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
		std::cout << '[' << classChars[symbol->Class] << "] " << symbol->SymbolName << endline;
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
			std::cout << "  [" << classChars[symbol.Class] << "] " << symbol.Name << endline;
		std::cout << endline;
	}
}

static void disassemble(const std::string& path)
{
	Common::IAllocator* allocator = Common::NativeAllocator::Instance;
	CFM::FragmentManager fragmentManager;
	CFM::PEFLibraryResolver pefResolver(Common::NativeAllocator::Instance, fragmentManager);
	ClassixCore::DyldLibraryResolver dyldResolver(allocator);
	
	dyldResolver.RegisterLibrary("StdCLib");
	
	fragmentManager.LibraryResolvers.push_back(&pefResolver);
	fragmentManager.LibraryResolvers.push_back(&dyldResolver);
	
	if (fragmentManager.LoadContainer(path))
	{
		CFM::SymbolResolver* resolver = fragmentManager.GetSymbolResolver(path);
		if (CFM::PEFSymbolResolver* pefResolver = dynamic_cast<CFM::PEFSymbolResolver*>(resolver))
		{
			PEF::Container& container = pefResolver->GetContainer();
			OStreamDisassemblyWriter writer(std::cout);
			PPCVM::Disassembly::FancyDisassembler(allocator).Disassemble(container, writer);
		}
		else
		{
			std::cerr << path << " is loadable, but is not a PEF container" << std::endl;
		}
	}
	else
	{
		std::cerr << "Couln't load " << path << std::endl;
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
	if (argc < 3)
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
			runMPW(path, argc - 2, argv + 2, envp);
	}
	catch (std::exception& error)
	{
		std::cerr << "operation failed: " << error.what() << endline;
	}
}

