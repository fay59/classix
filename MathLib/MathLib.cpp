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
#include "MathLib.h"
#include "MathLibFunctions.h"
#include "NotImplementedException.h"
#include "MachineState.h"

namespace MathLib
{
	const double pi = M_PI;
	
	struct Globals
	{
		Common::IAllocator& allocator;
		uint32_t _FE_DFL_ENV;
		
		Globals(Common::IAllocator& allocator)
		: allocator(allocator)
		{ }
	};
}

using PPCVM::MachineState;

extern "C"
{
	void MathLib___fpclassify(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___fpclassifyd(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___fpclassifyf(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___inf(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isfinite(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isfinited(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isfinitef(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnan(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnand(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnanf(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnormal(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnormald(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___isnormalf(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___signbit(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___signbitd(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib___signbitf(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_acos(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = acos(state->fpr[1]);
	}
	
	void MathLib_acosh(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = acosh(state->fpr[1]);
	}
	
	void MathLib_acoshl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_acosl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_annuity(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_asin(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = asin(state->fpr[1]);
	}
	
	void MathLib_asinh(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = asinh(state->fpr[1]);
	}
	
	void MathLib_asinhl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_asinl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_atan(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = atan(state->fpr[1]);
	}
	
	void MathLib_atan2(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = atan2(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_atan2l(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_atanh(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = atanh(state->fpr[1]);
	}
	
	void MathLib_atanhl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_atanl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_ceil(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = ceil(state->fpr[1]);
	}
	
	void MathLib_ceill(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_compound(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_copysign(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = copysign(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_copysignl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_cos(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = cos(state->fpr[1]);
	}
	
	void MathLib_cosh(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = cosh(state->fpr[1]);
	}
	
	void MathLib_coshl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_cosl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2f(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2l(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2num(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2numl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2s(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dec2str(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_dtox80(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_erf(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = erf(state->fpr[1]);
	}
	
	void MathLib_erfc(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = erfc(state->fpr[1]);
	}
	
	void MathLib_erfcl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_erfl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_exp(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = exp(state->fpr[1]);
	}
	
	void MathLib_exp2(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = exp2(state->fpr[1]);
	}
	
	void MathLib_exp2l(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_expl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_expm1(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = expm1(state->fpr[1]);
	}
	
	void MathLib_expm1l(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fabs(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = fabs(state->fpr[1]);
	}
	
	void MathLib_fabsl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fdim(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = fdim(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_fdiml(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_feclearexcept(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fegetenv(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fegetexcept(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fegetround(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_feholdexcept(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_feraiseexcept(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fesetenv(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fesetexcept(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fesetround(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fetestexcept(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_feupdateenv(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_floor(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = floor(state->fpr[1]);
	}
	
	void MathLib_floorl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fmax(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = fmax(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_fmaxl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fmin(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = fmin(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_fminl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_fmod(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = fmod(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_frexp(MathLib::Globals* globals, MachineState* state)
	{
		int intpart;
		state->fpr[1] = frexp(state->fpr[1], &intpart);
		*globals->allocator.ToPointer<Common::SInt32>(state->r4) = intpart;
	}
	
	void MathLib_frexpl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_gamma(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_gammal(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_hypot(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = hypot(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_hypotl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_ldexp(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = ldexp(state->fpr[1], state->r4);
	}
	
	void MathLib_ldexpl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_ldtox80(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_lgamma(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = lgamma(state->fpr[1]);
	}
	
	void MathLib_lgammal(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_log(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = log(state->fpr[1]);
	}
	
	void MathLib_log10(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = log10(state->fpr[1]);
	}
	
	void MathLib_log10l(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_log1p(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = log1p(state->fpr[1]);
	}
	
	void MathLib_log1pl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_log2(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = log2(state->fpr[1]);
	}
	
	void MathLib_log2l(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_logb(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = logb(state->fpr[1]);
	}
	
	void MathLib_logbl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_logl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_modf(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_modff(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_modfl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nan(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nanf(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nanl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nearbyint(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = nearbyint(state->fpr[1]);
	}
	
	void MathLib_nearbyintl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nextafterd(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nextafterf(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_nextafterl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_num2dec(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_num2decl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_pow(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = pow(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_powl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_randomx(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_relation(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_relationl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_remainder(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = remainder(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_remainderl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_remquo(MathLib::Globals* globals, MachineState* state)
	{
		int remquoOut;
		state->fpr[1] = remquo(state->fpr[1], state->fpr[2], &remquoOut);
		Common::UInt32* out = globals->allocator.ToPointer<Common::UInt32>(state->r5);
		*out = remquoOut;
	}
	
	void MathLib_remquol(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_rint(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = rint(state->fpr[1]);
	}
	
	void MathLib_rintl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_rinttol(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_rinttoll(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_round(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = round(state->fpr[1]);
	}
	
	void MathLib_roundl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_roundtol(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_roundtoll(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_scalb(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = scalb(state->fpr[1], state->fpr[2]);
	}
	
	void MathLib_scalbl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_sin(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_sinh(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = sinh(state->fpr[1]);
	}
	
	void MathLib_sinhl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_sinl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_sqrt(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = sqrt(state->fpr[1]);
	}
	
	void MathLib_sqrtl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_str2dec(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_tan(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = tan(state->fpr[1]);
	}
	
	void MathLib_tanh(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = tanh(state->fpr[1]);
	}
	
	void MathLib_tanhl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_tanl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_trunc(MathLib::Globals* globals, MachineState* state)
	{
		state->fpr[1] = trunc(state->fpr[1]);
	}
	
	void MathLib_truncl(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_x80tod(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
	
	void MathLib_x80told(MathLib::Globals* globals, MachineState* state)
	{
		throw PPCVM::NotImplementedException(__func__);
	}
}
