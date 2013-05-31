//
// StdCLib.cpp
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

#include <cstdlib>
#include <cfloat>
#include <cassert>
#include <cctype>
#include <map>
#include <string>
#include <sstream>
#include <regex>

#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "BigEndian.h"
#include "StdCLib.h"
#include "StdCLibFunctions.h"
#include "SymbolResolver.h"
#include "NotImplementedException.h"

using PPCVM::MachineState;

namespace
{
	FILE* fdup(FILE* fp)
	{
		static const char* modes[] = {
			[O_RDONLY] = "r",
			[O_WRONLY] = "a",
			[O_RDWR] = "r+"
		};
		
		int fd = fileno(fp);
		int accmode = fcntl(fd, F_GETFL) & O_ACCMODE;
		return fdopen(dup(fd), modes[accmode]);
	}
}

namespace StdCLib
{
	const int NFILE = 40;

	union PPCFILE
	{
		static const char* OffsetNames[28];
		
		struct
		{
			int32_t _cnt;
			uint32_t _ptr;
			uint32_t _base;
			uint32_t _end;
			uint16_t _size;
			uint16_t _flag;
			uint16_t _file;
		};
		
		struct
		{
			uint32_t : 32;
			FILE* fptr;
		};
	};

	struct IntEnv
	{
		Common::UInt16 unknown;
		Common::UInt32 argc;
		Common::UInt32 argv;
		Common::UInt32 envp;
	} __attribute__((packed));

	const char* PPCFILE::OffsetNames[28] = {
		"_cnt", "_cnt + 1", "_cnt + 2", "_cnt + 3",
		"_ptr", "_ptr + 1", "_ptr + 2", "_ptr + 3",
		"_base", "_base + 1", "_base + 2", "_base + 3",
		"_end", "_end + 1", "_end + 2", "_end + 3",
		"_size", "_size + 1", "_size + 2", "_size + 3",
		"_flag", "_flag + 1", "_flag + 2", "_flag + 3",
		"_file", "_file + 1", "_file + 2", "_file + 3",
	};

	#define _UPP		 0x01
	#define _LOW		 0x02
	#define _DIG		 0x04
	#define _WSP		 0x08
	#define _PUN		 0x10
	#define _CTL		 0x20
	#define _BLA		 0x40
	#define _HEX		 0x80

	const uint8_t cTypeCharClasses[0x100]={
		_CTL, _CTL, _CTL, _CTL, _CTL, _CTL, _CTL, _CTL,
		_CTL, _CTL|_WSP, _CTL|_WSP, _CTL|_WSP, _CTL|_WSP, _CTL|_WSP, _CTL, _CTL,
		_CTL, _CTL, _CTL, _CTL, _CTL, _CTL, _CTL, _CTL,
		_CTL, _CTL, _CTL, _CTL, _CTL, _CTL, _CTL, _CTL,
		_WSP|_BLA, _PUN, _PUN, _PUN, _PUN, _PUN, _PUN, _PUN,
		_PUN, _PUN, _PUN, _PUN, _PUN, _PUN, _PUN, _PUN,
		_DIG, _DIG, _DIG, _DIG, _DIG, _DIG, _DIG, _DIG,
		_DIG, _DIG, _PUN, _PUN, _PUN, _PUN, _PUN, _PUN,
		_PUN, _UPP|_HEX, _UPP|_HEX, _UPP|_HEX, _UPP|_HEX, _UPP|_HEX, _UPP|_HEX, _UPP,
		_UPP, _UPP, _UPP, _UPP, _UPP, _UPP, _UPP, _UPP,
		_UPP, _UPP, _UPP, _UPP, _UPP, _UPP, _UPP, _UPP,
		_UPP, _UPP, _UPP, _PUN, _PUN, _PUN, _PUN, _PUN,
		_PUN, _LOW|_HEX, _LOW|_HEX, _LOW|_HEX, _LOW|_HEX, _LOW|_HEX, _LOW|_HEX, _LOW,
		_LOW, _LOW, _LOW, _LOW, _LOW, _LOW, _LOW, _LOW,
		_LOW, _LOW, _LOW, _LOW, _LOW, _LOW, _LOW, _LOW,
		_LOW, _LOW, _LOW, _PUN, _PUN, _PUN, _PUN, _CTL
	};

	typedef uint8_t UnknownType[0x1000];
	typedef uint32_t JumpBuf[64];

	struct Scalars
	{
		UnknownType __C_phase; // apparently an integer that should not be 5
		UnknownType __loc;
		UnknownType __NubAt3; // apparently a function pointer
		Common::UInt32 __p_CType;
		UnknownType __SigEnv;
		JumpBuf __target_for_exit;
		UnknownType __yd;
		Common::UInt64 _CategoryLoc;
		Common::Real64 _DBL_EPSILON;
		Common::Real64 _DBL_MAX;
		Common::Real64 _DBL_MIN;
		Common::UInt32 _exit_status;
		Common::Real64 _FLT_EPSILON;
		Common::Real64 _FLT_MAX;
		Common::Real64 _FLT_MIN;
		IntEnv _IntEnv;
		PPCFILE _iob[NFILE];
		UnknownType _lastbuf;
		Common::Real64 _LDBL_EPSILON;
		Common::Real64 _LDBL_MIN;
		Common::Real64 _LDBL_MAX;
		Common::UInt64 _PublicTimeInfo;
		UnknownType _StdDevs;
		Common::UInt32 errno_;
		UnknownType MacOSErr;
		Common::UInt64 MoneyData;
		Common::UInt32 NoMoreDebugStr;
		Common::UInt64 NumericData;
		Common::UInt32 StandAlone;
		Common::UInt64 TimeData;
		uint8_t cType[256];
	};

	struct Globals
	{
		Scalars scalars;
		uint8_t padding[32]; // just some buffer space before we get to the allocator
		Common::IAllocator& allocator;
		
		static std::map<off_t, std::string> FieldOffsets;
		static std::map<std::string, size_t> FieldLocations;
		
		Globals(Common::IAllocator* allocator)
		: allocator(*allocator)
		{
			memset(&scalars, 0, sizeof scalars);
			memcpy(&scalars.cType, cTypeCharClasses, sizeof scalars.cType);
			
			scalars.__p_CType = this->allocator.ToIntPtr(&scalars.cType);
			
			scalars._iob[0].fptr = fdup(stdin);
			scalars._iob[1].fptr = fdup(stdout);
			scalars._iob[2].fptr = fdup(stderr);
			
			scalars._DBL_EPSILON = DBL_EPSILON;
			scalars._DBL_MIN = DBL_MIN;
			scalars._DBL_MAX = DBL_MAX;
			
			scalars._FLT_EPSILON = FLT_EPSILON;
			scalars._FLT_MIN = FLT_MIN;
			scalars._FLT_MAX = FLT_MAX;
			
			scalars._LDBL_EPSILON = DBL_EPSILON;
			scalars._DBL_MIN = DBL_MIN;
			scalars._LDBL_MAX = DBL_MAX;
			
			scalars.MoneyData = 0x3100000000000f68ull;
			scalars.NumericData = 0x3100000000000fc8ull;
			scalars.TimeData = 0x3100000000000ff0ull;
			scalars._PublicTimeInfo = 0x3100000000000ff0ull;
			scalars._CategoryLoc = 0x3030313131000000ull;
		}
	};

	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Winvalid-offsetof"

	std::map<off_t, std::string> Globals::FieldOffsets
	{
		std::make_pair(offsetof(Scalars, __C_phase), "__C_phase"),
		std::make_pair(offsetof(Scalars, __loc), "__loc"),
		std::make_pair(offsetof(Scalars, __NubAt3), "__NubAt3"),
		std::make_pair(offsetof(Scalars, __p_CType), "__p_CType"),
		std::make_pair(offsetof(Scalars, __SigEnv), "__SigEnv"),
		std::make_pair(offsetof(Scalars, __target_for_exit), "__target_for_exit"),
		std::make_pair(offsetof(Scalars, __yd), "__yd"),
		std::make_pair(offsetof(Scalars, _CategoryLoc), "_CategoryLoc"),
		std::make_pair(offsetof(Scalars, _DBL_EPSILON), "_DBL_EPSILON"),
		std::make_pair(offsetof(Scalars, _DBL_MAX), "_DBL_MAX"),
		std::make_pair(offsetof(Scalars, _DBL_MIN), "_DBL_MIN"),
		std::make_pair(offsetof(Scalars, _exit_status), "_exit_status"),
		std::make_pair(offsetof(Scalars, _FLT_EPSILON), "_FLT_EPSILON"),
		std::make_pair(offsetof(Scalars, _FLT_MAX), "_FLT_MAX"),
		std::make_pair(offsetof(Scalars, _FLT_MIN), "_FLT_MIN"),
		std::make_pair(offsetof(Scalars, _IntEnv), "_IntEnv"),
		std::make_pair(offsetof(Scalars, _iob), "_iob"),
		std::make_pair(offsetof(Scalars, _lastbuf), "_lastbuf"),
		std::make_pair(offsetof(Scalars, _LDBL_EPSILON), "_LDBL_EPSILON"),
		std::make_pair(offsetof(Scalars, _LDBL_MIN), "_LDBL_MIN"),
		std::make_pair(offsetof(Scalars, _LDBL_MAX), "_LDBL_MAX"),
		std::make_pair(offsetof(Scalars, _PublicTimeInfo), "_PublicTimeInfo"),
		std::make_pair(offsetof(Scalars, _StdDevs), "_StdDevs"),
		std::make_pair(offsetof(Scalars, errno_), "errno"),
		std::make_pair(offsetof(Scalars, MacOSErr), "MacOSErr"),
		std::make_pair(offsetof(Scalars, MoneyData), "MoneyData"),
		std::make_pair(offsetof(Scalars, NoMoreDebugStr), "NoMoreDebugStr"),
		std::make_pair(offsetof(Scalars, NumericData), "NumericData"),
		std::make_pair(offsetof(Scalars, StandAlone), "StandAlone"),
		std::make_pair(offsetof(Scalars, TimeData), "TimeData"),
		std::make_pair(offsetof(Scalars, cType), "cType"),
	};

	std::map<std::string, size_t> Globals::FieldLocations
	{
		std::make_pair("__C_phase", offsetof(Scalars, __C_phase)),
		std::make_pair("__loc", offsetof(Scalars, __loc)),
		std::make_pair("__NubAt3", offsetof(Scalars, __NubAt3)),
		std::make_pair("__p_CType", offsetof(Scalars, __p_CType)),
		std::make_pair("__SigEnv", offsetof(Scalars, __SigEnv)),
		std::make_pair("__target_for_exit", offsetof(Scalars, __target_for_exit)),
		std::make_pair("__yd", offsetof(Scalars, __yd)),
		std::make_pair("_CategoryLoc", offsetof(Scalars, _CategoryLoc)),
		std::make_pair("_DBL_EPSILON", offsetof(Scalars, _DBL_EPSILON)),
		std::make_pair("_DBL_MAX", offsetof(Scalars, _DBL_MAX)),
		std::make_pair("_DBL_MIN", offsetof(Scalars, _DBL_MIN)),
		std::make_pair("_exit_status", offsetof(Scalars, _exit_status)),
		std::make_pair("_FLT_EPSILON", offsetof(Scalars, _FLT_EPSILON)),
		std::make_pair("_FLT_MAX", offsetof(Scalars, _FLT_MAX)),
		std::make_pair("_FLT_MIN", offsetof(Scalars, _FLT_MIN)),
		std::make_pair("_IntEnv", offsetof(Scalars, _IntEnv)),
		std::make_pair("_iob", offsetof(Scalars, _iob)),
		std::make_pair("_lastbuf", offsetof(Scalars, _lastbuf)),
		std::make_pair("_LDBL_EPSILON", offsetof(Scalars, _LDBL_EPSILON)),
		std::make_pair("_LDBL_MIN", offsetof(Scalars, _LDBL_MIN)),
		std::make_pair("_LDBL_MAX", offsetof(Scalars, _LDBL_MAX)),
		std::make_pair("_PublicTimeInfo", offsetof(Scalars, _PublicTimeInfo)),
		std::make_pair("_StdDevs", offsetof(Scalars, _StdDevs)),
		std::make_pair("errno", offsetof(Scalars, errno_)),
		std::make_pair("MacOSErr", offsetof(Scalars, MacOSErr)),
		std::make_pair("MoneyData", offsetof(Scalars, MoneyData)),
		std::make_pair("NoMoreDebugStr", offsetof(Scalars, NoMoreDebugStr)),
		std::make_pair("NumericData", offsetof(Scalars, NumericData)),
		std::make_pair("StandAlone", offsetof(Scalars, StandAlone)),
		std::make_pair("TimeData", offsetof(Scalars, TimeData)),
		std::make_pair("cType", offsetof(Scalars, cType)),
	};

	#pragma clang diagnostics pop

	class GlobalsDetails : public Common::AllocationDetails
	{
	public:
		GlobalsDetails()
		: Common::AllocationDetails("StdCLib Globals", sizeof(Globals))
		{ }
		
		virtual std::string GetAllocationDetails(uint32_t offset) const override
		{
			std::stringstream ss;
			ss << "StdCLib::Globals::";
			for (auto iter = StdCLib::Globals::FieldOffsets.rbegin(); iter != StdCLib::Globals::FieldOffsets.rend(); iter++)
			{
				if (iter->first <= offset)
				{
					ss << iter->second;
					off_t subOffset = offset - iter->first;
					if (iter->second == "_iob")
					{
						ss << '[' << (subOffset / sizeof (PPCFILE)) << ']';
						uint32_t fieldOffset = subOffset % sizeof(PPCFILE);
						if (fieldOffset != 0)
						{
							ss << '.' << PPCFILE::OffsetNames[fieldOffset];
						}
					}
					else if (subOffset != 0)
					{
						ss << " +" << subOffset;
					}
					return ss.str();
				}
			}
			
			assert(false && "this should never happen");
			return "<not found>";
		}
		
		virtual AllocationDetails* ToHeapAlloc() const override
		{
			return new GlobalsDetails(*this);
		}
		
		virtual ~GlobalsDetails() override
		{}
	};
	
	// TODO handle the vararg calling convention correctly: this can fail horribly with large calls, or calls that
	// have floats as first and second arguments
	std::string StringPrintF(const std::string& formatString, Globals& globals, const uint32_t* gpr, const double* fpr)
	{
		std::string doubleTypes = "aAeEfFgG";
		std::regex rx("%([0-9]+\\$)?(#?)(0?)(-?)( ?)(\\+?)('?)([0-9]?)(\\.[0-9]*)?[hLljtzq]*[diouxXDOUeEfFgGaAcCsSpn]");
		std::smatch match;
		
		std::stringstream result;
		std::string::const_iterator lastMatchEnd = formatString.begin();
		std::string::const_iterator matchStart = formatString.begin();
		while (std::regex_search(matchStart, formatString.cend(), match, rx))
		{
			matchStart += match.position();
			result << std::string(lastMatchEnd, matchStart);
			
			char* out = nullptr;
			const std::string& formatSpecifier = match[0].str();
			auto reverseIter = formatSpecifier.rbegin();
			if (*reverseIter == 's' || *reverseIter == 'S')
			{
				const char* pointer = globals.allocator.ToPointer<char>(*gpr);
				asprintf(&out, formatSpecifier.c_str(), pointer);
				gpr++;
			}
			else if (doubleTypes.find_first_of(*reverseIter) != std::string::npos)
			{
				if (*(reverseIter + 1) == 'L')
					throw PPCVM::NotImplementedException(__func__, "Long doubles are not supported in format strings");
				
				asprintf(&out, formatSpecifier.c_str(), *fpr);
				fpr++;
			}
			else
			{
				if (*(reverseIter + 1) == 'l' && *(reverseIter + 2) == 'l')
				{
					uint64_t argument = ((uint64_t)gpr[0] << 32) | gpr[1];
					asprintf(&out, formatSpecifier.c_str(), argument);
					gpr += 2;
				}
				else
				{
					asprintf(&out, formatSpecifier.c_str(), *gpr);
					gpr++;
				}
			}
			
			result << out;
			free(out);
			
			lastMatchEnd = matchStart;
			matchStart += match.length();
		}
		
		result << std::string(lastMatchEnd, formatString.end());
		return result.str();
	}
}

#pragma mark -
#pragma mark Lifecycle
extern "C"
{
	StdCLib::Globals* LibraryLoad(Common::IAllocator* allocator, OSEnvironment::Managers* managers)
	{
		return allocator->Allocate<StdCLib::Globals>(StdCLib::GlobalsDetails(), allocator);
	}

	SymbolType LibraryLookup(StdCLib::Globals* globals, const char* name, void** result)
	{
		if (name == CFM::SymbolResolver::InitSymbolName)
			name = "__StdCLib_IntEnvInit";
		
		char functionName[36] = "StdCLib_";
		char* end = stpncpy(functionName + 8, name, 27);
		if (*end != 0)
		{
			*result = nullptr;
			return SymbolNotFound;
		}
		
		if (void* symbol = dlsym(RTLD_SELF, functionName))
		{
			*result = symbol;
			return CodeSymbol;
		}
		
		auto iter = StdCLib::Globals::FieldLocations.find(name);
		if (iter != StdCLib::Globals::FieldLocations.end())
		{
			*result = reinterpret_cast<uint8_t*>(&globals->scalars) + iter->second;
			return DataSymbol;
		}
		
		*result = nullptr;
		return SymbolNotFound;
	}

	void LibraryUnload(StdCLib::Globals* globals)
	{
		for (int i = 0; i < StdCLib::NFILE; i++)
		{
			FILE* file = globals->scalars._iob[i].fptr;
			if (file != nullptr)
				fclose(file);
		}
		globals->allocator.Deallocate(globals);
	}
}

#pragma mark -
#pragma mark Implementation

#define ToPointer	globals->allocator.ToPointer
#define ToIntPtr	globals->allocator.ToIntPtr

namespace
{
	FILE* MakeFilePtr(StdCLib::Globals* globals, uint32_t ptr)
	{
		for (int i = 0; i < StdCLib::NFILE; i++)
		{
			uint32_t thisIOB = ToIntPtr(&globals->scalars._iob[i]);
			if (ptr == thisIOB)
				return globals->scalars._iob[i].fptr;
		}
		return nullptr;
	}
}

extern "C"
{
	void StdCLib___StdCLib_IntEnvInit(StdCLib::Globals* globals, MachineState* state)
	{
		globals->scalars._IntEnv.argc = state->r3;
		globals->scalars._IntEnv.argv = state->r4;
		globals->scalars._IntEnv.envp = state->r5;
	}
	
	void StdCLib___abort(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___assertprint(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___DebugMallocHeap(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___GetTrapType(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___growFileTable(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___NumToolboxTraps(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___RestoreInitialCFragWorld(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___RevertCFragWorld(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___setjmp(StdCLib::Globals* globals, MachineState* state)
	{
		// since the client is not really supposed to read from jmpBuf values, we
		// shouldn't have to worry about endianness
		uint32_t* jmpBuf = ToPointer<uint32_t>(state->r3);
		jmpBuf[0] = state->lr;
		jmpBuf[1] = state->GetCR();
		jmpBuf[2] = state->r1;
		jmpBuf[3] = state->r2;
		jmpBuf[4] = 0;
		memcpy(jmpBuf + 5, state->gpr + 13, 19 * sizeof(uint32_t));
		memcpy(jmpBuf + 24, state->fpr + 14, 18 * sizeof(double));
		jmpBuf[61] = 0;
		jmpBuf[62] = 0;
		
		// ??? there are two fields left over
		jmpBuf[63] = 0;
		jmpBuf[64] = 0;
		
		state->r3 = 0;
		globals->scalars.errno_ = 0;
	}

	void StdCLib___vec_longjmp(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib___vec_setjmp(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__addDevHandler(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__badPtr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__Bogus(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__BreakPoint(StdCLib::Globals* globals, MachineState* state)
	{
		const char* reason = ToPointer<char>(state->r3);
		printf("Interrupted by %s", reason);
		asm("int $3");
	}

	void StdCLib__bufsync(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__c2pstrcpy(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__coClose(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__coExit(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__coFAccess(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__coIoctl(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__coRead(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__coreIOExit(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__coWrite(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__cvt(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__DoExitProcs(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__doprnt(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__doscan(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__exit(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__faccess(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__filbuf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__findiop(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__flsbuf(StdCLib::Globals* globals, MachineState* state)
	{
		int character = state->r3;
		FILE* fptr = MakeFilePtr(globals, state->r4);
		fputc(character, fptr);
		fflush(fptr);
		state->r3 = character & 0xff;
	}

	void StdCLib__fsClose(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__fsFAccess(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__fsIoctl(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__fsRead(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__FSSpec2Path(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__fsWrite(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__GetAliasInfo(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__getDevHandler(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__getIOPort(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__memchr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__memcpy(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__ResolveFileAlias(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__rmemcpy(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__RTExit(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__RTInit(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__SA_DeletePtr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__SA_GetPID(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__SA_SetPtrSize(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__syClose(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__syFAccess(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__syIoctl(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__syRead(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__syWrite(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__uerror(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__wrtchk(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib__xflsbuf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_abort(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_abs(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_access(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_asctime(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_atexit(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_atof(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_atoi(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_atol(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_atoll(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_binhex(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_bsearch(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_calloc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_clearerr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_clock(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_close(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ConvertTheString(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_creat(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ctime(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_difftime(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_div(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_dup(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ecvt(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_exit(StdCLib::Globals* globals, MachineState* state)
	{
		// TODO longjmp to __target_for_exit
		// for now we'll just kill the host (which is really, really bad)
		exit(state->r3);
	}

	void StdCLib_faccess(StdCLib::Globals* globals, MachineState* state)
	{
		fprintf(stderr, "Using 'faccess' with mode %08x\n", state->r4);
		state->r3 = 0;
		globals->scalars.errno_ = 0;
	}

	void StdCLib_fclose(StdCLib::Globals* globals, MachineState* state)
	{
		for (int i = 0; i < StdCLib::NFILE; i++)
		{
			auto& ioBuffer = globals->scalars._iob[i];
			uint32_t ioBufferAddress = ToIntPtr(&ioBuffer);
			if (ioBufferAddress == state->r3)
			{
				state->r3 = fclose(ioBuffer.fptr);
				ioBuffer.fptr = nullptr;
				return;
			}
		}
		
		globals->scalars.errno_ = EBADF;
		state->r3 = EOF;
	}

	void StdCLib_fcntl(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fcvt(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fdopen(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_feof(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ferror(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fflush(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fgetc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fgetpos(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fgets(StdCLib::Globals* globals, MachineState* state)
	{
		char* buffer = ToPointer<char>(state->r3);
		int32_t size = state->r4;
		FILE* fptr = MakeFilePtr(globals, state->r5);
		
		char* result = fgets(buffer, size, fptr);
		state->r3 = ToIntPtr(result);
		globals->scalars.errno_ = errno;
	}

	void StdCLib_fopen(StdCLib::Globals* globals, MachineState* state)
	{
		const char* filename = ToPointer<const char>(state->r3);
		const char* mode = ToPointer<const char>(state->r4);
		
		for (int i = 0; i < StdCLib::NFILE; i++)
		{
			auto& ioBuffer = globals->scalars._iob[i];
			if (ioBuffer.fptr == nullptr)
			{
				ioBuffer.fptr = fopen(filename, mode);
				state->r3 = ToIntPtr(&ioBuffer);
				globals->scalars.errno_ = errno;
				return;
			}
		}
		
		state->r3 = 0;
		globals->scalars.errno_ = EMFILE;
	}

	void StdCLib_fprintf(StdCLib::Globals* globals, MachineState* state)
	{
		FILE* fptr = MakeFilePtr(globals, state->r3);
		const char* formatString = ToPointer<const char>(state->r4);
		// gah, let's just print the format string for now.
		state->r3 = fputs(formatString, fptr);
		globals->scalars.errno_ = errno;
	}

	void StdCLib_fputc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fputs(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fread(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_free(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_freopen(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fscanf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fseek(StdCLib::Globals* globals, MachineState* state)
	{
		FILE* fptr = MakeFilePtr(globals, state->r3);
		int32_t offset = state->r4;
		int whence = state->r5;
		state->r3 = fseek(fptr, offset, whence);
		globals->scalars.errno_ = errno;
	}

	void StdCLib_fsetfileinfo(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fsetpos(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSMakeFSSpec_Long(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_creat(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_faccess(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_fopen(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_freopen(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_fsetfileinfo(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_open(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_remove(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_rename(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSp_unlink(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_FSSpec2Path_Long(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ftell(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_fwrite(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_getc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_getchar(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_getenv(StdCLib::Globals* globals, MachineState* state)
	{
	#ifdef __LP64__
	# error This will break in 64 bits because getenv() will return a value out of the address space
	#endif
		
		const char* name = ToPointer<const char>(state->r3);
		char* env = getenv(name);
		state->r3 = ToIntPtr(env);
		globals->scalars.errno_ = errno;
	}

	void StdCLib_getIDstring(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_getpid(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_gets(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_getw(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_gmtime(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_IEResolvePath(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ioctl(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isalnum(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isalpha(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isascii(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_iscntrl(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isdigit(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isgraph(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_islower(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isprint(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ispunct(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isspace(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isupper(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_isxdigit(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_labs(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ldiv(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_llabs(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_lldiv(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_localeconv(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_localtime(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_longjmp(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_lseek(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_MakeResolvedFSSpec(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_MakeResolvedFSSpec_Long(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_MakeResolvedPath(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_MakeResolvedPath_Long(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_MakeTheLocaleString(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_malloc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_mblen(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_mbstowcs(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_mbtowc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_memccpy(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_memchr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_memcmp(StdCLib::Globals* globals, MachineState* state)
	{
		const void* s1 = ToPointer<const void>(state->r3);
		const void* s2 = ToPointer<const void>(state->r4);
		size_t size = state->r5;
		state->r3 = memcmp(s1, s2, size);
	}

	void StdCLib_memcpy(StdCLib::Globals* globals, MachineState* state)
	{
		void* s1 = ToPointer<void>(state->r3);
		const void* s2 = ToPointer<const void>(state->r4);
		size_t size = state->r5;
		state->r3 = ToIntPtr(memcpy(s1, s2, size));
	}

	void StdCLib_memmove(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_memset(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_mktemp(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_mktime(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_open(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ParseTheLocaleString(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_perror(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLpos(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrcat(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrchr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrcmp(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrcpy(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrlen(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrncat(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrncmp(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrncpy(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrpbrk(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrrchr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrspn(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_PLstrstr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_printf(StdCLib::Globals* globals, MachineState* state)
	{
		const char* formatString = ToPointer<const char>(state->r3);
		std::string toPrint = StdCLib::StringPrintF(formatString, *globals, state->gpr + 4, state->fpr);
		state->r3 = printf("%s", toPrint.c_str());
		globals->scalars.errno_ = errno;
	}

	void StdCLib_putc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_putchar(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_puts(StdCLib::Globals* globals, MachineState* state)
	{
		const char* address = ToPointer<const char>(state->r3);
		state->r3 = puts(address);
		globals->scalars.errno_ = errno;
	}

	void StdCLib_putw(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_qsort(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_raise(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_rand(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_read(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_realloc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_remove(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_rename(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ResolveFolderAliases(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ResolveFolderAliases_Long(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ResolvePath(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ResolvePath_Long(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_rewind(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_scanf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_setbuf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_setenv(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_setlocale(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_setvbuf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_signal(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_sprintf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_srand(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_sscanf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strcat(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strchr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strcmp(StdCLib::Globals* globals, MachineState* state)
	{
		const char* s1 = ToPointer<const char>(state->r3);
		const char* s2 = ToPointer<const char>(state->r4);
		state->r3 = strcmp(s1, s2);
	}

	void StdCLib_strcoll(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strcpy(StdCLib::Globals* globals, MachineState* state)
	{
		char* s1 = ToPointer<char>(state->r3);
		const char* s2 = ToPointer<const char>(state->r4);
		state->r3 = ToIntPtr(strcpy(s1, s2));
	}

	void StdCLib_strcspn(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strerror(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strftime(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strlen(StdCLib::Globals* globals, MachineState* state)
	{
		char* s = ToPointer<char>(state->r3);
		state->r3 = strlen(s);
	}

	void StdCLib_strncat(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strncmp(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strncpy(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strpbrk(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strrchr(StdCLib::Globals* globals, MachineState* state)
	{
		const char* s = ToPointer<const char>(state->r3);
		int c = state->r4;
		char* result = strrchr(s, c);
		state->r3 = ToIntPtr(result);
	}

	void StdCLib_strspn(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strstr(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strtod(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strtok(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strtol(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strtoll(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strtoul(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strtoull(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_strxfrm(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_system(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_time(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_tmpfile(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_tmpnam(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_toascii(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_tolower(StdCLib::Globals* globals, MachineState* state)
	{
		state->r3 = tolower(state->r3);
	}

	void StdCLib_toupper(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_TrapAvailable(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_ungetc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_unlink(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_vec_calloc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_vec_free(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_vec_malloc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_vec_realloc(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_vfprintf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_vprintf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_vsprintf(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_wcstombs(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_wctomb(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}

	void StdCLib_write(StdCLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
}
