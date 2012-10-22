//
//  main.cpp
//  pefdump
//
//  Created by Félix on 2012-10-20.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include <cstdio>
#include <iostream>

#include "Container.h"
#include "FileMapper.h"

const char endline = '\n';

int main(int argc, const char * argv[])
{
	if (argc < 2)
	{
		std::cerr << "usage: pefdump pef-file" << std::endl;
		return 1;
	}
	
	try
	{
		FileMapper file(argv[1]);
		PEF::Container container(file.begin(), file.end());
		
		std::cout << "Preferred Executable Format file '" << argv[1] << "' has " << container.Size() << " instantiable sections" << endline;
		for (auto iter = container.SectionsBegin(); iter != container.SectionsEnd(); iter++)
		{
			std::cout << "Section '" << iter->Name << "' "
				<< "(" << iter->GetSectionType() << ", "
				<< iter->GetShareType() << ")"
				<< endline;
		}
		
		const PEF::LoaderSection& loader = *container.LoaderSection();
		std::cout << "Loader section: " << loader.Header->ImportedLibraryCount << " imported libraries "
			<< "for a total of " << loader.Header->ImportedSymbolCount << " imported symbols"
			<< endline << endline;
		for (auto libIter = loader.LibrariesBegin(); libIter != loader.LibrariesEnd(); libIter++)
		{
			std::cout << "Library: " << libIter->Name << endline;
			for (auto iter = libIter->symbols.begin(); iter != libIter->symbols.end(); iter++)
			{
				std::cout << "\t[" << (iter->IsStronglyLinked ? 'S' : 'w') << "] " << iter->Name << endline;
			}
			std::cout << std::endl;
		}
		
		std::cout << "Relocations required for " << loader.Header->RelocSectionCount << " sections" << endline;
		for (auto relocIter = loader.RelocationsBegin(); relocIter != loader.RelocationsEnd(); relocIter++)
		{
			ptrdiff_t relocInstructionCount = relocIter->end() - relocIter->begin();
			std::cout << "Section #" << relocIter->GetSectionIndex() << " will execute " << relocInstructionCount << " relocation instructions" << endline;
		}
		
		const auto& exports = loader.ExportTable;
		std::cout << "There are " << exports.SymbolCount() << " exported symbols:" << endline;
		for (auto nameIter = exports.begin(); nameIter != exports.end(); nameIter++)
		{
			auto symbol = exports.Find(*nameIter);
			std::cout << "\t" << *nameIter << " (section " << symbol->SectionIndex << ", offset " << symbol->Offset << ")" << endline;
		}
	}
	catch (std::logic_error error)
	{
		std::cerr << "operation failed: " << error.what() << endline;
	}
}

