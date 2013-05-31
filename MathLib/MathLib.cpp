//
// MathLib.cpp
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

#include <cmath>
#include <dlfcn.h>
#include "MathLib.h"
#include "MathLibFunctions.h"
#include "NotImplementedException.h"
#include "MachineState.h"

namespace MathLib
{
	struct Globals
	{
		Common::IAllocator& allocator;
		uint32_t FE_DFL_ENV;
		Common::Real64 pi;
		
		Globals(Common::IAllocator& allocator)
		: allocator(allocator)
		{
			pi = M_PI;
			// TODO give a value to FE_DFL_ENV
		}
	};
}

using PPCVM::MachineState;
using namespace MathLib;

const std::string piName = "pi";
const std::string envName = "_FE_DFL_ENV";

extern "C"
{
	Globals* LibraryLoad(Common::IAllocator* allocator, OSEnvironment::Managers* managers)
	{
		return allocator->Allocate<Globals>("MathLib Globals", *allocator);
	}
	
	SymbolType LibraryLookup(Globals* globals, const char* name, void** result)
	{
		if (name == piName)
		{
			*result = &globals->pi;
			return DataSymbol;
		}
		
		if (name == envName)
		{
			*result = &globals->FE_DFL_ENV;
			return DataSymbol;
		}
		
		char functionName[22] = "MathLib_";
		char* end = stpncpy(functionName + 8, name, 13);
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
		
		*result = nullptr;
		return SymbolNotFound;
	}
	
	void LibraryUnload(Globals* globals)
	{
		globals->allocator.Deallocate(globals);
	}
	
	void MathLib___fpclassify(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___fpclassifyd(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___fpclassifyf(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___inf(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isfinite(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isfinited(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isfinitef(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnan(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnand(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnanf(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnormal(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnormald(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnormalf(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___signbit(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___signbitd(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___signbitf(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_acos(Globals* globals, MachineState* state)
	{
		state->fpr[1] = acos(state->fpr[1]);
	}
	
	void MathLib_acosh(Globals* globals, MachineState* state)
	{
		state->fpr[1] = acosh(state->fpr[1]);
	}
	
	void MathLib_acoshl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_acosl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_annuity(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_asin(Globals* globals, MachineState* state)
	{
		state->fpr[1] = asin(state->fpr[1]);
	}
	
	void MathLib_asinh(Globals* globals, MachineState* state)
	{
		state->fpr[1] = asinh(state->fpr[1]);
	}
	
	void MathLib_asinhl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_asinl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_atan(Globals* globals, MachineState* state)
	{
		state->fpr[1] = atan(state->fpr[1]);
	}
	
	void MathLib_atan2(Globals* globals, MachineState* state)
	{
		state->fpr[1] = atan2(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_atan2l(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_atanh(Globals* globals, MachineState* state)
	{
		state->fpr[1] = atanh(state->fpr[1]);
	}
	
	void MathLib_atanhl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_atanl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_ceil(Globals* globals, MachineState* state)
	{
		state->fpr[1] = ceil(state->fpr[1]);
	}
	
	void MathLib_ceill(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_compound(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_copysign(Globals* globals, MachineState* state)
	{
		state->fpr[1] = copysign(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_copysignl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_cos(Globals* globals, MachineState* state)
	{
		state->fpr[1] = cos(state->fpr[1]);
	}
	
	void MathLib_cosh(Globals* globals, MachineState* state)
	{
		state->fpr[1] = cosh(state->fpr[1]);
	}
	
	void MathLib_coshl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_cosl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2f(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2l(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2num(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2numl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2s(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2str(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dtox80(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_erf(Globals* globals, MachineState* state)
	{
		state->fpr[1] = erf(state->fpr[1]);
	}
	
	void MathLib_erfc(Globals* globals, MachineState* state)
	{
		state->fpr[1] = erfc(state->fpr[1]);
	}
	
	void MathLib_erfcl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_erfl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_exp(Globals* globals, MachineState* state)
	{
		state->fpr[1] = exp(state->fpr[1]);
	}
	
	void MathLib_exp2(Globals* globals, MachineState* state)
	{
		state->fpr[1] = exp2(state->fpr[1]);
	}
	
	void MathLib_exp2l(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_expl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_expm1(Globals* globals, MachineState* state)
	{
		state->fpr[1] = expm1(state->fpr[1]);
	}
	
	void MathLib_expm1l(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fabs(Globals* globals, MachineState* state)
	{
		state->fpr[1] = fabs(state->fpr[1]);
	}
	
	void MathLib_fabsl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fdim(Globals* globals, MachineState* state)
	{
		state->fpr[1] = fdim(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_fdiml(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_feclearexcept(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fegetenv(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fegetexcept(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fegetround(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_feholdexcept(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_feraiseexcept(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fesetenv(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fesetexcept(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fesetround(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fetestexcept(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_feupdateenv(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_floor(Globals* globals, MachineState* state)
	{
		state->fpr[1] = floor(state->fpr[1]);
	}
	
	void MathLib_floorl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fmax(Globals* globals, MachineState* state)
	{
		state->fpr[1] = fmax(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_fmaxl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fmin(Globals* globals, MachineState* state)
	{
		state->fpr[1] = fmin(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_fminl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fmod(Globals* globals, MachineState* state)
	{
		state->fpr[1] = fmod(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_frexp(Globals* globals, MachineState* state)
	{
		int intpart;
		state->fpr[1] = frexp(state->fpr[1], &intpart);
		*globals->allocator.ToPointer<Common::SInt32>(state->r4) = intpart;
	}
	
	void MathLib_frexpl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_gamma(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_gammal(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_hypot(Globals* globals, MachineState* state)
	{
		state->fpr[1] = hypot(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_hypotl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_ldexp(Globals* globals, MachineState* state)
	{
		state->fpr[1] = ldexp(state->fpr[1], state->r4);
	}
	
	void MathLib_ldexpl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_ldtox80(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_lgamma(Globals* globals, MachineState* state)
	{
		state->fpr[1] = lgamma(state->fpr[1]);
	}
	
	void MathLib_lgammal(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_log(Globals* globals, MachineState* state)
	{
		state->fpr[1] = log(state->fpr[1]);
	}
	
	void MathLib_log10(Globals* globals, MachineState* state)
	{
		state->fpr[1] = log10(state->fpr[1]);
	}
	
	void MathLib_log10l(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_log1p(Globals* globals, MachineState* state)
	{
		state->fpr[1] = log1p(state->fpr[1]);
	}
	
	void MathLib_log1pl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_log2(Globals* globals, MachineState* state)
	{
		state->fpr[1] = log2(state->fpr[1]);
	}
	
	void MathLib_log2l(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_logb(Globals* globals, MachineState* state)
	{
		state->fpr[1] = logb(state->fpr[1]);
	}
	
	void MathLib_logbl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_logl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_modf(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_modff(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_modfl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nan(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nanf(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nanl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nearbyint(Globals* globals, MachineState* state)
	{
		state->fpr[1] = nearbyint(state->fpr[1]);
	}
	
	void MathLib_nearbyintl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nextafterd(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nextafterf(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nextafterl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_num2dec(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_num2decl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_pow(Globals* globals, MachineState* state)
	{
		state->fpr[1] = pow(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_powl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_randomx(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_relation(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_relationl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_remainder(Globals* globals, MachineState* state)
	{
		state->fpr[1] = remainder(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_remainderl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_remquo(Globals* globals, MachineState* state)
	{
		int remquoOut;
		state->fpr[1] = remquo(state->fpr[1], state->fpr[2], &remquoOut);
		Common::UInt32* out = globals->allocator.ToPointer<Common::UInt32>(state->r5);
		*out = remquoOut;
	}
	
	void MathLib_remquol(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_rint(Globals* globals, MachineState* state)
	{
		state->fpr[1] = rint(state->fpr[1]);
	}
	
	void MathLib_rintl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_rinttol(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_rinttoll(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_round(Globals* globals, MachineState* state)
	{
		state->fpr[1] = round(state->fpr[1]);
	}
	
	void MathLib_roundl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_roundtol(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_roundtoll(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_scalb(Globals* globals, MachineState* state)
	{
		state->fpr[1] = scalb(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_scalbl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_sin(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_sinh(Globals* globals, MachineState* state)
	{
		state->fpr[1] = sinh(state->fpr[1]);
	}
	
	void MathLib_sinhl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_sinl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_sqrt(Globals* globals, MachineState* state)
	{
		state->fpr[1] = sqrt(state->fpr[1]);
	}
	
	void MathLib_sqrtl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_str2dec(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_tan(Globals* globals, MachineState* state)
	{
		state->fpr[1] = tan(state->fpr[1]);
	}
	
	void MathLib_tanh(Globals* globals, MachineState* state)
	{
		state->fpr[1] = tanh(state->fpr[1]);
	}
	
	void MathLib_tanhl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_tanl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_trunc(Globals* globals, MachineState* state)
	{
		state->fpr[1] = trunc(state->fpr[1]);
	}
	
	void MathLib_truncl(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_x80tod(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_x80told(Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
}
