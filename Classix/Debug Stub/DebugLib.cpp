//
// DebugLib.cpp
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

#include <sstream>
#include <iomanip>

#include "DebugLib.h"
#include "MachineState.h"
#include "Allocator.h"

using namespace std;
using namespace CFM;
using namespace Common;
using namespace PPCVM;

struct DebugLibContext
{
	Allocator& allocator;
	FragmentManager& fragments;
	
	DebugLibContext(Allocator& allocator, FragmentManager& fragments)
	: allocator(allocator), fragments(fragments)
	{ }
	
	static void GetLoadedLibrariesCount(DebugLibContext* context, MachineState* state)
	{
		state->r3 = static_cast<uint32_t>(context->fragments.size());
	}
	
	static void GetExportedSymbols(DebugLibContext* context, MachineState* state)
	{
		auto iter = context->fragments.begin();
		for (uint32_t i = 0; i < state->r3 && iter != context->fragments.end(); i++)
			iter++;
		
		if (iter == context->fragments.end())
		{
			state->r3 = 0;
			return;
		}
		
		stringstream ss;
		for (const string& symbolName : iter->second->SymbolList())
		{
			ResolvedSymbol symbol = iter->second->ResolveSymbol(symbolName);
			ss << symbolName << ':' << hex << symbol.Address << ';';
		}
		
		string output = ss.str();
		char* visibleMemory = reinterpret_cast<char*>(context->allocator.Allocate("Symbol List from DebugLib", output.length()));
		copy(output.begin(), output.end(), visibleMemory);
		state->r3 = context->allocator.ToIntPtr(visibleMemory);
	}
	
	static void DisposeMemory(DebugLibContext* context, MachineState* state)
	{
		void* memory = context->allocator.ToPointer<void>(state->r3);
		context->allocator.Deallocate(memory);
	}
	
	typedef void NativeCallback(DebugLibContext*, MachineState*);
	static unordered_map<string, NativeCallback*> Callbacks;
};

const string DebugLib::LibraryName = ":DebugLib";

#define CALLBACK(n)	make_pair("DebugLib_" #n, &DebugLibContext::n)
unordered_map<string, DebugLibContext::NativeCallback*> DebugLibContext::Callbacks = {
	CALLBACK(GetLoadedLibrariesCount),
	CALLBACK(GetExportedSymbols),
	CALLBACK(DisposeMemory),
};
#undef CALLBACK

DebugLib::DebugLib(Allocator& allocator, FragmentManager& fragments)
: allocator(allocator), stAllocator(allocator), transitions(stAllocator), nativeCalls(stAllocator)
{
	context = allocator.Allocate<DebugLibContext>("DebugLib Globals", allocator, fragments);
}

SymbolResolver* DebugLib::ResolveLibrary(const string &name)
{
	if (name == LibraryName)
		return this;
	
	return nullptr;
}

const string* DebugLib::FilePath() const
{
	return &LibraryName;
}

vector<string> DebugLib::SymbolList() const
{
	vector<string> list;
	list.reserve(DebugLibContext::Callbacks.size());
	for (auto iter = DebugLibContext::Callbacks.begin(); iter != DebugLibContext::Callbacks.end(); iter++)
		list.push_back(iter->first);
	
	return list;
}

ResolvedSymbol DebugLib::ResolveSymbol(const string &name)
{
	auto iter = symbols.find(name);
	if (iter == symbols.end())
	{
		auto callbackIter = DebugLibContext::Callbacks.find(name);
		if (callbackIter != DebugLibContext::Callbacks.end())
		{
			stAllocator.SetNextName(name + " Native Call");
			auto nativeCallIter = nativeCalls.emplace(nativeCalls.end(), *callbackIter->second);
			PEF::TransitionVector vector;
			vector.EntryPoint = allocator.ToIntPtr(&*nativeCallIter);
			vector.TableOfContents = allocator.ToIntPtr(context);
			stAllocator.SetNextName(name + " Transition Vector");
			auto transitionIter = transitions.emplace(transitions.end(), vector);
			
			uint32_t address = allocator.ToIntPtr(&*transitionIter);
			iter = symbols.emplace(std::make_pair(name, ResolvedSymbol(SymbolUniverse::Intel, name, address))).first;
		}
		else
		{
			iter = symbols.emplace(std::make_pair(name, ResolvedSymbol::Invalid)).first;
		}
	}
	
	return iter->second;
}

vector<ResolvedSymbol> DebugLib::GetEntryPoints()
{
	return vector<ResolvedSymbol>();
}

DebugLib::~DebugLib()
{
	allocator.Deallocate(context);
}
