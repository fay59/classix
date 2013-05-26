//
// MathLibFunctions.h
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

#ifndef __Classix__MathLibFunctions__
#define __Classix__MathLibFunctions__

namespace PPCVM
{
	struct MachineState;
}

namespace MathLib
{
	struct Globals;
}

extern "C"
{
	void MathLib___fpclassify(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___fpclassifyd(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___fpclassifyf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___inf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isfinite(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isfinited(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isfinitef(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isnan(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isnand(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isnanf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isnormal(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isnormald(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___isnormalf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___signbit(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___signbitd(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib___signbitf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_acos(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_acosh(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_acoshl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_acosl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_annuity(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_asin(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_asinh(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_asinhl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_asinl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_atan(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_atan2(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_atan2l(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_atanh(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_atanhl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_atanl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_ceil(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_ceill(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_compound(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_copysign(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_copysignl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_cos(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_cosh(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_coshl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_cosl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_dec2f(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_dec2l(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_dec2num(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_dec2numl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_dec2s(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_dec2str(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_dtox80(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_erf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_erfc(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_erfcl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_erfl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_exp(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_exp2(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_exp2l(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_expl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_expm1(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_expm1l(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fabs(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fabsl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fdim(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fdiml(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_feclearexcept(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fegetenv(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fegetexcept(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fegetround(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_feholdexcept(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_feraiseexcept(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fesetenv(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fesetexcept(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fesetround(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fetestexcept(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_feupdateenv(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_floor(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_floorl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fmax(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fmaxl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fmin(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fminl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_fmod(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_frexp(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_frexpl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_gamma(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_gammal(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_hypot(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_hypotl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_ldexp(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_ldexpl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_ldtox80(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_lgamma(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_lgammal(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_log(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_log10(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_log10l(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_log1p(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_log1pl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_log2(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_log2l(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_logb(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_logbl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_logl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_modf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_modff(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_modfl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_nan(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_nanf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_nanl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_nearbyint(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_nearbyintl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_nextafterd(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_nextafterf(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_nextafterl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_num2dec(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_num2decl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_pow(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_powl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_randomx(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_relation(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_relationl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_remainder(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_remainderl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_remquo(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_remquol(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_rint(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_rintl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_rinttol(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_rinttoll(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_round(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_roundl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_roundtol(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_roundtoll(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_scalb(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_scalbl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_sin(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_sinh(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_sinhl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_sinl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_sqrt(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_sqrtl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_str2dec(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_tan(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_tanh(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_tanhl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_tanl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_trunc(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_truncl(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_x80tod(MathLib::Globals* globals, PPCVM::MachineState* state);
	void MathLib_x80told(MathLib::Globals* globals, PPCVM::MachineState* state);
}

#endif /* defined(__Classix__MathLibFunctions__) */
