//
//  Unmangle.cpp
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "Unmangle.h"
#include <map>

namespace
{
#define OP(x, y)	std::make_pair("__op" y, x), std::make_pair("__" y, x)
	std::map<std::string, std::string> specialNames {
		std::make_pair("_vtbl", "(virtual table)"),
		std::make_pair("_rttivtbl", "(virtual table)"),
		std::make_pair("_vbtbl", "(virtual base class table)"),
		std::make_pair("__rtti", "(RTTI)"),
		std::make_pair("__ti", "(RTTI)"),
		std::make_pair("___ti", "(RTTI)"),
		OP(" new", "nw"),
		OP(" new[]", "nwa"),
		OP(" delete", "dl"),
		OP(" delete[]", "dla"),
		OP("+", "pl"),
		OP("-", "mi"),
		OP("*", "ml"),
		OP("/", "dv"),
		OP("%", "md"),
		OP("^", "er"),
		OP("/=", "adv"),
		OP("&", "ad"),
		OP("|", "or"),
		OP("~", "co"),
		OP("!", "nt"),
		OP("=", "as"),
		OP("<", "lt"),
		OP(">", "gt"),
		OP("+=", "apl"),
		OP("-=", "ami"),
		OP("*=", "amu"),
		OP("%=", "amd"),
		OP("^=", "aer"),
		OP("&=", "aad"),
		OP("|=", "aor"),
		OP("<<", "ls"),
		OP(">>", "rs"),
		OP("<<=", "als"),
		OP(">>=", "ars"),
		OP("==", "eq"),
		OP("!=", "ne"),
		OP("<=", "le"),
		OP(">=", "ge"),
		OP("&&", "aa"),
		OP("||", "oo"),
		OP("++", "pp"),
		OP("--", "mm"),
		OP("()", "cl"),
		OP("[]", "vc"),
		OP("->", "rf"),
		OP(",", "cm"),
		OP("->*", "rm")
	};
#undef OP
}

std::string Common::Unmangle(const std::string& mangled)
{
	size_t offset = mangled.find("__", 2);
	
	// is it a mangled name?
	if (offset == std::string::npos)
		return mangled;
	
	
	// TODO complete
	return mangled;
}