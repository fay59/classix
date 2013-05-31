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
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

#include "FragmentManager.h"
#include "PEFLibraryResolver.h"
#include "PEFSymbolResolver.h"
#include "DlfcnLibraryResolver.h"
#include "DummyLibraryResolver.h"
#include "BundleLibraryResolver.h"
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

static int listExports(const std::string& path)
{
	Common::NativeAllocator allocator;
	Common::FileMapping mapping(path);
	PEF::Container container(allocator, mapping.begin(), mapping.end());
	
	const PEF::ExportHashTable& exportTable = container.LoaderSection()->ExportTable;
	std::cout << exportTable.SymbolCount() << " exports" << endline;
	
	struct Export
	{
		char type;
		std::string name;
		const uint8_t* begin;
		const uint8_t* end;
	};
	
	int16_t onlySection = -1;
	std::vector<Export> exports;
	for (auto iter = exportTable.begin(); iter != exportTable.end(); iter++)
	{
		const PEF::ExportedSymbol* symbol = exportTable.Find(*iter);
		Export e = {
			.type = classChars[symbol->Class],
			.name = symbol->SymbolName,
			.begin = nullptr,
			.end = nullptr
		};
		
		if (e.type == 'D')
		{
			ssize_t containerSize = container.size();
			if (symbol->SectionIndex < 0 || symbol->SectionIndex >= containerSize)
				std::cerr << "data symbol " << symbol->SymbolName << " doesn't live in this container" << std::endl;
			else
			{
				const PEF::InstantiableSection& section = container.GetSection(symbol->SectionIndex);
				if (onlySection == -1)
					onlySection = symbol->SectionIndex;
				else if (onlySection != symbol->SectionIndex)
					onlySection = -2;
				e.begin = section.Data + symbol->Offset;
			}
		}
		
		exports.push_back(e);
	}
	
	// we only support dumping globals when there is just one data section
	if (onlySection >= 0)
	{
		std::sort(exports.begin(), exports.end(), [](const Export& a, const Export& b) { return a.begin < b.begin; });
		const PEF::InstantiableSection& section = container.GetSection(onlySection);
		const uint8_t* end = section.Data + section.Size();
		for (auto iter = exports.rbegin(); iter != exports.rend(); iter++)
		{
			if (iter->begin == nullptr)
				break;
			
			iter->end = end;
			end = iter->begin;
		}
	}
	
	for (const auto& e : exports)
	{
		std::cout << '[' << e.type << "] " << e.name;
		if (e.begin != nullptr && e.end != nullptr)
		{
			std::cout << " (" << std::dec << e.end - e.begin << ") =";
			for (auto iter = e.begin; iter != e.end; iter++)
				std::cout << ' ' << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)*iter;
		}
		std::cout << std::endl;
	}
	
	std::cout << endline;
	return 0;
}

static int listImports(const std::string& path)
{
	Common::NativeAllocator allocator;
	Common::FileMapping mapping(path);
	PEF::Container container(allocator, mapping.begin(), mapping.end());
	
	const PEF::LoaderSection* loader = container.LoaderSection();
	for (auto libIter = loader->LibrariesBegin(); libIter != loader->LibrariesEnd(); libIter++)
	{
		std::cout << libIter->Name << ':' << endline;
		for (auto& symbol : libIter->Symbols)
			std::cout << "  [" << classChars[symbol.Class] << "] " << symbol.Name << endline;
		std::cout << endline;
	}
	return 0;
}

int patchExecutable(const std::string& path, const std::string& outPath);

static int disassemble(const std::string& path)
{
	Common::NativeAllocator allocator;
	CFM::FragmentManager fragmentManager;
	CFM::PEFLibraryResolver pefResolver(allocator, fragmentManager);
	CFM::DummyLibraryResolver resolver(allocator);
	
	fragmentManager.LibraryResolvers.push_back(&pefResolver);
	fragmentManager.LibraryResolvers.push_back(&resolver);
	
	if (fragmentManager.LoadContainer(path))
	{
		CFM::SymbolResolver* resolver = fragmentManager.GetSymbolResolver(path);
		if (CFM::PEFSymbolResolver* pefResolver = dynamic_cast<CFM::PEFSymbolResolver*>(resolver))
		{
			PEF::Container& container = pefResolver->GetContainer();
			OStreamDisassemblyWriter writer(allocator, std::cout);
			PPCVM::Disassembly::FancyDisassembler(allocator).Disassemble(container, writer);
		}
		else
		{
			std::cerr << path << " is loadable, but is not a PEF container" << std::endl;
			return -1;
		}
	}
	else
	{
		std::cerr << "Couln't load " << path << std::endl;
		return -2;
	}
	return 0;
}

static int run(const std::string& path, int argc, const char* argv[], const char* envp[])
{
	Common::NativeAllocator allocator;
	OSEnvironment::Managers managers(allocator);
	CFM::DummyLibraryResolver dummyResolver(allocator);
	ClassixCore::DlfcnLibraryResolver dlfcnResolver(allocator, managers);
	ClassixCore::BundleLibraryResolver bundleResolver(allocator, managers);
	
	dlfcnResolver.RegisterLibrary("StdCLib");
	dlfcnResolver.RegisterLibrary("MathLib");
	dlfcnResolver.RegisterLibrary("ThreadsLib");
	bundleResolver.AllowLibrary("InterfaceLib");
	
	char* directory = strdup(path.c_str());
	char* executableName = directory;
	for (char* iter = directory; *iter != 0; iter++)
	{
		if (*iter == '/')
			executableName = iter;
	}
	*executableName = 0;
	executableName++;
	
	chdir(directory);
	std::string executable = executableName;
	free(directory);
	
	Classix::VirtualMachine vm(allocator);
	vm.AddLibraryResolver(dlfcnResolver);
	vm.AddLibraryResolver(bundleResolver);
	vm.AddLibraryResolver(dummyResolver);
	
	auto stub = vm.LoadMainContainer(executable);
	return stub(argc, argv, envp);
}

static int inflateAndDump(const std::string& path, const std::string& targetDir)
{
	Common::NativeAllocator allocator;
	OSEnvironment::Managers managers(allocator);
	CFM::FragmentManager fragmentManager;
	CFM::PEFLibraryResolver pefResolver(allocator, fragmentManager);
	ClassixCore::DlfcnLibraryResolver dlfcnResolver(allocator, managers);
	
	dlfcnResolver.RegisterLibrary("StdCLib");
	
	fragmentManager.LibraryResolvers.push_back(&pefResolver);
	fragmentManager.LibraryResolvers.push_back(&dlfcnResolver);
	
	if (fragmentManager.LoadContainer(path))
	{
		for (auto& pair : fragmentManager)
		{
			if (CFM::PEFSymbolResolver* resolver = dynamic_cast<CFM::PEFSymbolResolver*>(pair.second))
			{
				PEF::Container& container = resolver->GetContainer();
				for (PEF::InstantiableSection& section : container)
				{
					std::stringstream pathSS;
					pathSS << targetDir << '/' << section.Name;
					std::fstream fileOut(pathSS.str(), std::ios_base::out | std::ios_base::binary);
					fileOut.write(reinterpret_cast<char*>(section.Data), section.Size());
				}
			}
		}
	}
	else
	{
		std::cerr << "Couln't load " << path << std::endl;
		return -2;
	}
	return 0;
}

int compareTrace(const std::string& path, const std::string& tracePath);

static int usage()
{
	std::cerr << "usage: Classix -e file # list exports" << std::endl;
	std::cerr << "       Classix -i file # list imports" << std::endl;
	std::cerr << "       Classix -d file # disassemble code sections" << std::endl;
	std::cerr << "       Classix -r file # run the file" << std::endl;
	std::cerr << "       Classix -b file out-file # patch executable to always call _BreakPoint at start" << std::endl;
	std::cerr << "       Classix -z file target # dump sections to target directory" << std::endl;
	std::cerr << "       Classix -c file trace # execute and compare to MacsBug trace" << std::endl;
	return 1;
}

int main(int argc, const char* argv[], const char* envp[])
{
	if (argc < 3) return usage();
	
	std::string mode = argv[1];
	std::string ppcPath = argv[2];
	
	try
	{
		if (mode == "-e")
			return listExports(ppcPath);
		else if (mode == "-i")
			return listImports(ppcPath);
		else if (mode == "-d")
			return disassemble(ppcPath);
		else if (mode == "-r")
			return run(ppcPath, argc - 2, argv + 2, envp);
		else
		{
			if (argc < 4) return usage();
			std::string secondArg = argv[3];
			
			if (mode == "-b")
				return patchExecutable(ppcPath, secondArg);
			else if (mode == "-z")
				return inflateAndDump(ppcPath, secondArg);
			else if (mode == "-c")
				return compareTrace(ppcPath, secondArg);
		}
		
		return usage();
	}
	catch (std::exception& error)
	{
		std::cerr << "operation failed: " << error.what() << endline;
	}
}

