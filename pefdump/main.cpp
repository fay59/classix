//
//  main.cpp
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <cstdio>
#include <iostream>

#include "FragmentManager.h"
#include "PEFLibraryResolver.h"
#include "NativeAllocator.h"
#include "FileMapping.h"
#include "Unmangle.h"
#include "BridgeLibraryResolver.h"
#include "MachineState.h"

const char endline = '\n';

static void loadTest(const std::string& path)
{
	PPCVM::MachineState state;
	
	CFM::FragmentManager fragmentManager;
	CFM::PEFLibraryResolver pefResolver(Common::NativeAllocator::Instance, fragmentManager);
	ObjCBridge::BridgeLibraryResolver objcResolver(Common::NativeAllocator::Instance, state);
	
	fragmentManager.Resolvers.push_back(&pefResolver);
	fragmentManager.Resolvers.push_back(&objcResolver);
	
	fragmentManager.LoadContainer(path);
	
	// memoryManager.ReserveAdditional(0x2000000); // 32 MB
	std::cout << "Successfully loaded container " << path << endline;
}

static char classChars[] = {
	[PEF::SymbolClasses::CodeSymbol] = 'C',
	[PEF::SymbolClasses::DataSymbol] = 'D',
	[PEF::SymbolClasses::DirectData] = 'I',
	[PEF::SymbolClasses::FunctionPointer] = 'F',
	[PEF::SymbolClasses::GlueSymbol] = 'G'
};

static void listExports(const std::string& path)
{
	Common::FileMapping mapping(path);
	PEF::Container container(Common::NativeAllocator::Instance, mapping.begin(), mapping.end());
	
	const PEF::ExportHashTable& exportTable = container.LoaderSection()->ExportTable;
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

int main(int argc, const char * argv[])
{
	if (argc != 3)
	{
		std::cerr << "usage: pefdump -o file # tries to load fragment" << std::endl;
		std::cerr << "       pefdump -e file # tries to list exports" << std::endl;
		std::cerr << "       pefdump -i file # tries to list imports" << std::endl;
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
	}
	catch (std::exception& error)
	{
		std::cerr << "operation failed: " << error.what() << endline;
	}
}

