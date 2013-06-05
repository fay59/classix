//
// CXUnmangle.cpp
// Classix
//
// Copyright (C) 2013 Félix Cloutier
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

#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <iostream>

namespace
{
	struct Unimplemented {};
	struct Illegal {};
	
	typedef std::string::const_iterator siter;
	
	const std::unordered_map<std::string, std::string> specialNames = {
		std::make_pair("_vtbl", "(virtual table)"),
		std::make_pair("_rttivtbl", "(virtual table)"),
		std::make_pair("__vt", "(virtual table)"),
		std::make_pair("_vbtbl", "(virtual base class table)"),
		std::make_pair("_rtti", "(RTTI)"),
		std::make_pair("__ti", "(RTTI)"),
		std::make_pair("___ti", "(RTTI)"),
		std::make_pair("__nw", "operator new"),
		std::make_pair("__nwa", "operator new[]"),
		std::make_pair("__dl", "operator delete"),
		std::make_pair("__dla", "operator delete[]"),
		std::make_pair("__pl", "operator+"),
		std::make_pair("__mi", "operator-"),
		std::make_pair("__ml", "operator*"),
		std::make_pair("__dv", "operator/"),
		std::make_pair("__md", "operator%"),
		std::make_pair("__er", "operator^"),
		std::make_pair("__adv", "operator/="),
		std::make_pair("__ad", "operator&"),
		std::make_pair("__or", "operator|"),
		std::make_pair("__co", "operator~"),
		std::make_pair("__nt", "operator!"),
		std::make_pair("__as", "operator="),
		std::make_pair("__lt", "operator<"),
		std::make_pair("__gt", "operator>"),
		std::make_pair("__apl", "operator+="),
		std::make_pair("__ami", "operator-="),
		std::make_pair("__amu", "operator*="),
		std::make_pair("__amd", "operator%="),
		std::make_pair("__aer", "operator^="),
		std::make_pair("__aad", "operator&="),
		std::make_pair("__aor", "operator|="),
		std::make_pair("__ls", "operator<<"),
		std::make_pair("__rs", "operator>>"),
		std::make_pair("__ars", "operator>>="),
		std::make_pair("__als", "operator<<="),
		std::make_pair("__eq", "operator=="),
		std::make_pair("__ne", "operator!="),
		std::make_pair("__le", "operator<="),
		std::make_pair("__ge", "operator>="),
		std::make_pair("__aa", "operator&&"),
		std::make_pair("__oo", "operator||"),
		std::make_pair("__pp", "operator++"),
		std::make_pair("__mm", "operator--"),
		std::make_pair("__cl", "operator()"),
		std::make_pair("__vc", "operator[]"),
		std::make_pair("__rf", "operator->"),
		std::make_pair("__cm", "operator,"),
		std::make_pair("__rm", "operator->*")
	};
	
	const std::unordered_map<char, std::string> basicTypes = {
		std::make_pair('b', "bool"),
		std::make_pair('c', "char"),
		std::make_pair('d', "double"),
		std::make_pair('e', "..."),
		std::make_pair('f', "float"),
		std::make_pair('i', "int"),
		std::make_pair('l', "long"),
		std::make_pair('r', "long double"),
		std::make_pair('x', "long long"),
		std::make_pair('s', "short"),
		std::make_pair('v', "void"),
		std::make_pair('w', "wchar_t"),
	};
	
	siter CXParseType(siter begin, siter end, std::string& typeName);
	
	std::string CXRestituteTemplateParams(const std::string& name, std::string& shortName)
	{
		std::string::size_type bracket = name.find('<');
		if (bracket == std::string::npos)
		{
			shortName = name;
			return name;
		}
		
		shortName = name.substr(0, bracket);
		
		std::string typeArg;
		std::stringstream ss;
		
		siter templateArg = name.begin() + bracket;
		std::string baseName(name.cbegin(), templateArg);
		ss << baseName;
		
		templateArg++;
		templateArg = CXParseType(templateArg, name.end(), typeArg);
		assert(*templateArg == '>' || *templateArg == ',');
		ss << '<' << typeArg;
		
		while (*templateArg != '>')
		{
			templateArg++;
			templateArg = CXParseType(templateArg, name.end(), typeArg);
			assert(*templateArg == '>' || *templateArg == ',');
			ss << ", " << typeArg;
		}
		ss << '>';
		return ss.str();
	}
	
	siter CXParseArbitraryName(siter begin, siter end, std::string& qualifiedName, std::string& shortName)
	{
		size_t count = 0;
		siter iter = begin;
		while (*iter >= '0' && *iter <= '9')
		{
			count *= 10;
			count += *iter - '0';
			iter++;
		}
		
		siter nameEnd = iter + count;
		if (nameEnd > end)
			throw Illegal();
		
		qualifiedName = CXRestituteTemplateParams(std::string(iter, nameEnd), shortName);
		return nameEnd;
	}
	
	siter CXParseArbitraryName(siter begin, siter end, std::string& qualifiedName)
	{
		std::string null;
		return CXParseArbitraryName(begin, end, qualifiedName, null);
	}
	
	siter CXParseCompositeName(siter begin, siter end, std::string& qualifiedName, std::string& shortName)
	{
		assert(*begin == 'Q');
		std::stringstream ss;
		siter iter = begin + 1;
		size_t subnames = *iter - '0';
		if (subnames < 2)
			throw Illegal();
		
		std::string name;
		iter = CXParseArbitraryName(iter + 1, end, name);
		ss << name;
		for (size_t i = 1; i < subnames; i++)
		{
			iter = CXParseArbitraryName(iter, end, name);
			ss << "::" << name;
		}
		qualifiedName = ss.str();
		return iter;
	}
	
	siter CXParseCompositeName(siter begin, siter end, std::string& qualifiedName)
	{
		std::string null;
		return CXParseCompositeName(begin, end, qualifiedName, null);
	}
	
	siter CXParseFunctionType(siter begin, siter end, std::string& output)
	{
		assert(*begin == 'F');
		std::stringstream ss;
		
		std::string argType;
		siter iter = CXParseType(begin + 1, end, argType);
		ss << '(' << argType;
		while (*iter != '_')
		{
			if (iter > end)
				throw Illegal();
			
			iter = CXParseType(iter, end, argType);
			ss << ", " << argType;
		}
		
		ss << ')';
		iter = CXParseType(iter + 1, end, argType);
		output = argType + " (*)" + ss.str();
		return iter;
	}
	
	siter CXParseType(siter begin, siter end, std::string& typeName)
	{
		std::stringstream ss;
		siter iter = begin;
		while (iter != end)
		{
			switch (*iter)
			{
				case 'C':
					ss << "const ";
					iter++;
					break;
					
				case 'V':
					ss << "volatile ";
					iter++;
					break;
					
				case 'U':
					ss << "unsigned ";
					iter++;
					break;
					
				case 'S':
					ss << "signed ";
					iter++;
					break;
					
				case 'P':
				{
					std::string pointerType;
					char nextType = *(iter + 1);
					iter = CXParseType(iter + 1, end, pointerType);
					ss << pointerType;
					
					// special case for function pointers: don't display the (*) again
					if (nextType != 'F')
						ss << '*';
					
					goto done;
				}
					
				case 'R':
				{
					std::string referenceType;
					iter = CXParseType(iter + 1, end, referenceType);
					ss << referenceType << '&';
					goto done;
				}
					
				case 'F':
				{
					std::string functionType;
					iter = CXParseFunctionType(iter, end, functionType);
					ss << functionType;
					goto done;
				}
					
				case 'M': // pointer to member; what the hell is that
				case 'A': // array
					throw Unimplemented();
					
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'i':
				case 'l':
				case 'r':
				case 'x':
				case 's':
				case 'v':
				case 'w':
				{
					auto typeIter = basicTypes.find(*iter);
					assert(typeIter != basicTypes.end());
					ss << typeIter->second;
					iter++;
					goto done;
				}
					
				case '1'...'9':
				{
					std::string arbitraryName;
					iter = CXParseArbitraryName(iter, end, arbitraryName);
					ss << arbitraryName;
					goto done;
				}
					
				case 'Q':
				{
					std::string name;
					iter = CXParseCompositeName(iter, end, name);
					ss << name;
					goto done;
				}
					
				default:
					throw Illegal();
			}
		}
		// if we get here, we only partially parsed a name and that's not valid
		throw Illegal();
		
	done:
		typeName = ss.str();
		return iter;
	}
}

std::string CXUnmangle(const std::string& mangled)
{
	bool isSpecialName = mangled[0] == '_' && mangled[1] == '_';
	std::string::size_type nameEnd = mangled.find("__", isSpecialName ? 2 : 0);
	if (nameEnd == std::string::npos || nameEnd == mangled.size() - 2)
		return mangled; // not actually mangled
	
	try
	{
		std::string nonParameterized;
		std::string memberName = CXRestituteTemplateParams(mangled.substr(0, nameEnd), nonParameterized);
		siter begin = mangled.begin() + nameEnd + 2;
		siter end = mangled.end();
		
		std::stringstream ss;
		std::string classQualifiedName;
		std::string className;
		if (*begin != 'F')
		{
			// parse class name first
			if (*begin == 'Q')
			{
				begin = CXParseCompositeName(begin, end, classQualifiedName, className);
			}
			else if (*begin >= '0' && *begin <= '9')
			{
				begin = CXParseArbitraryName(begin, end, classQualifiedName, className);
			}
			else
			{
				throw Illegal();
			}
			
			ss << classQualifiedName << "::";
		}
		
		std::string modifiers;
		siter iter = begin;
		while (true)
		{
			switch (*iter)
			{
				case 'C':
					modifiers += " const";
					iter++;
					break;
					
				case 'V':
					modifiers += " volatile";
					iter++;
					break;
					
				case 'F':
					iter++;
					goto arguments;
					
				case 0:
					goto arguments;
					
				default:
					throw Illegal();
			}
		}
		
	arguments:
		bool isClassMember = className.size() != 0;
		if (isClassMember && memberName == "__ct")
		{
			ss << className;
		}
		else if (isClassMember && memberName == "__dt")
		{
			ss << '~' << className;
		}
		else
		{
			auto specialNameIter = specialNames.find(memberName);
			if (specialNameIter != specialNames.end())
			{
				ss << specialNameIter->second;
			}
			else
			{
				ss << memberName;
			}
		}
		
		if (iter != end)
		{
			std::string argumentType;
			iter = CXParseType(iter, end, argumentType);
			ss << '(' << argumentType;
			while (iter != end)
			{
				iter = CXParseType(iter, end, argumentType);
				ss << ", " << argumentType;
			}
			ss << ')';
		}
		ss << modifiers;
		return ss.str();
	}
	catch (Unimplemented&)
	{
		return mangled;
	}
	catch (Illegal&)
	{
		std::cerr << "*** couldn't unmangle " << mangled << " because its encoding seems invalid\n";
		return mangled;
	}
}
