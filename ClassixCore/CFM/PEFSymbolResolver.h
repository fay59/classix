//
// PEFSymbolResolver.h
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

#ifndef __pefdump__PEFSymbolResolver__
#define __pefdump__PEFSymbolResolver__

#include "SymbolResolver.h"
#include "Container.h"
#include "FileMapping.h"
#include "FragmentManager.h"

namespace CFM
{
	class PEFSymbolResolver : public SymbolResolver
	{
		Common::IAllocator& allocator;
		FragmentManager& cfm;
		
		Common::FileMapping mapping;
		PEF::Container container;
		
		ResolvedSymbol Symbolize(const std::string& name, const uint8_t* address) const;
		ResolvedSymbol Symbolize(const std::string& name, const PEF::LoaderHeader::SectionWithOffset& sectionWithOffset) const;
		
	public:
		PEFSymbolResolver(Common::IAllocator& allocator, FragmentManager& cfm, const std::string& filePath);
		PEFSymbolResolver(Common::IAllocator& allocator, FragmentManager& cfm, Common::FileMapping&& mapping);
		
		PEF::Container& GetContainer();
		const PEF::Container& GetContainer() const;
		
		virtual const std::string* FilePath() const override;
		virtual std::vector<std::string> SymbolList() const override;
		
		virtual ResolvedSymbol ResolveSymbol(const std::string& symbolName) override;
		virtual std::vector<ResolvedSymbol> GetEntryPoints() override;
		
		virtual ~PEFSymbolResolver() override;
	};
}

#endif /* defined(__pefdump__PEFSymbolResolver__) */
