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
#include "MemoryManager.h"
#include "NativeAllocator.h"
#include "FileMapping.h"

const char endline = '\n';

static void loadTest(const std::string& path)
{
	try
	{
		PPCVM::MemoryManager memoryManager;
		
		CFM::FragmentManager fragmentManager;
		CFM::PEFLibraryResolver pefResolver(memoryManager, fragmentManager);
		fragmentManager.Resolvers.push_back(&pefResolver);
		
		fragmentManager.LoadContainer(path);
		
		// memoryManager.ReserveAdditional(0x2000000); // 32 MB
		std::cout << "Successfully loaded container " << path << endline;
	}
	catch (std::logic_error error)
	{
		std::cerr << "operation failed: " << error.what() << endline;
	}
}

static void listExports(const std::string& path)
{
	try
	{
		Common::FileMapping mapping(path);
		PEF::Container container(Common::NativeAllocator::Instance, mapping.begin(), mapping.end());
		
		const auto& exportTable = container.LoaderSection()->ExportTable;
		for (auto iter = exportTable.begin(); iter != exportTable.end(); iter++)
			std::cout << *iter << endline;
	}
	catch (std::logic_error error)
	{
		std::cerr << "operation failed: " << error.what() << endline;
	}
}

int main(int argc, const char * argv[])
{
	if (argc != 3)
	{
		std::cerr << "usage: pefdump -o file" << std::endl;
		std::cerr << "       pefdump -l file" << std::endl;
		return 1;
	}
	
	std::string mode = argv[1];
	std::string path = argv[2];
	
	if (mode == "-o")
		loadTest(path);
	else if (mode == "-l")
		listExports(path);
}

