//
//  MathLib.m
//  pefdump
//
//  Created by Félix on 2012-10-27.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import "MathLib.h"
#include "BigEndian.h"

struct MathLibGlobals
{
	uint32_t _FE_DFL_ENV;
	Common::Real64 pi;
};

@implementation MathLib

-(id)initWithAllocator:(PPCAllocator *)aAllocator
{
	if (!(self = [super init]))
		return nil;
	
	allocator = [aAllocator retain];
	globals = reinterpret_cast<MathLibGlobals*>([allocator allocate:sizeof(MathLibGlobals)]);
	
	globals->pi = M_PI;
	globals->_FE_DFL_ENV = 0; // what's this?
	
	return self;
}

-(NSString*)libraryName
{
	return @"MathLib";
}

-(PPCLibraryFunction)resolve:(NSString *)functionName
{
	return nullptr;
}

-(void)dealloc
{
	[allocator deallocate:globals];
	[allocator release];
	[super dealloc];
}

#pragma mark -
#pragma mark MathLib Data Symbols

-(void*)MathLib__FE_DFL_ENV
{
	return &globals->_FE_DFL_ENV;
}

-(void*)MathLib_pi
{
	return &globals->pi;
}

#pragma mark -
#pragma mark MathLib Code Symbols

// TODO implement me!
-(void)MathLib___fpclassify:(PPCMachineState*)state { }
-(void)MathLib___fpclassifyd:(PPCMachineState*)state { }
-(void)MathLib___fpclassifyf:(PPCMachineState*)state { }
-(void)MathLib___inf:(PPCMachineState*)state { }
-(void)MathLib___isfinite:(PPCMachineState*)state { }
-(void)MathLib___isfinited:(PPCMachineState*)state { }
-(void)MathLib___isfinitef:(PPCMachineState*)state { }
-(void)MathLib___isnan:(PPCMachineState*)state { }
-(void)MathLib___isnand:(PPCMachineState*)state { }
-(void)MathLib___isnanf:(PPCMachineState*)state { }
-(void)MathLib___isnormal:(PPCMachineState*)state { }
-(void)MathLib___isnormald:(PPCMachineState*)state { }
-(void)MathLib___isnormalf:(PPCMachineState*)state { }
-(void)MathLib___signbit:(PPCMachineState*)state { }
-(void)MathLib___signbitd:(PPCMachineState*)state { }
-(void)MathLib___signbitf:(PPCMachineState*)state { }
-(void)MathLib_acos:(PPCMachineState*)state { }
-(void)MathLib_acosh:(PPCMachineState*)state { }
-(void)MathLib_acoshl:(PPCMachineState*)state { }
-(void)MathLib_acosl:(PPCMachineState*)state { }
-(void)MathLib_annuity:(PPCMachineState*)state { }
-(void)MathLib_asin:(PPCMachineState*)state { }
-(void)MathLib_asinh:(PPCMachineState*)state { }
-(void)MathLib_asinhl:(PPCMachineState*)state { }
-(void)MathLib_asinl:(PPCMachineState*)state { }
-(void)MathLib_atan:(PPCMachineState*)state { }
-(void)MathLib_atan2:(PPCMachineState*)state { }
-(void)MathLib_atan2l:(PPCMachineState*)state { }
-(void)MathLib_atanh:(PPCMachineState*)state { }
-(void)MathLib_atanhl:(PPCMachineState*)state { }
-(void)MathLib_atanl:(PPCMachineState*)state { }
-(void)MathLib_ceil:(PPCMachineState*)state { }
-(void)MathLib_ceill:(PPCMachineState*)state { }
-(void)MathLib_compound:(PPCMachineState*)state { }
-(void)MathLib_copysign:(PPCMachineState*)state { }
-(void)MathLib_copysignl:(PPCMachineState*)state { }
-(void)MathLib_cos:(PPCMachineState*)state { }
-(void)MathLib_cosh:(PPCMachineState*)state { }
-(void)MathLib_coshl:(PPCMachineState*)state { }
-(void)MathLib_cosl:(PPCMachineState*)state { }
-(void)MathLib_dec2f:(PPCMachineState*)state { }
-(void)MathLib_dec2l:(PPCMachineState*)state { }
-(void)MathLib_dec2num:(PPCMachineState*)state { }
-(void)MathLib_dec2numl:(PPCMachineState*)state { }
-(void)MathLib_dec2s:(PPCMachineState*)state { }
-(void)MathLib_dec2str:(PPCMachineState*)state { }
-(void)MathLib_dtox80:(PPCMachineState*)state { }
-(void)MathLib_erf:(PPCMachineState*)state { }
-(void)MathLib_erfc:(PPCMachineState*)state { }
-(void)MathLib_erfcl:(PPCMachineState*)state { }
-(void)MathLib_erfl:(PPCMachineState*)state { }
-(void)MathLib_exp:(PPCMachineState*)state { }
-(void)MathLib_exp2:(PPCMachineState*)state { }
-(void)MathLib_exp2l:(PPCMachineState*)state { }
-(void)MathLib_expl:(PPCMachineState*)state { }
-(void)MathLib_expm1:(PPCMachineState*)state { }
-(void)MathLib_expm1l:(PPCMachineState*)state { }
-(void)MathLib_fabs:(PPCMachineState*)state { }
-(void)MathLib_fabsl:(PPCMachineState*)state { }
-(void)MathLib_fdim:(PPCMachineState*)state { }
-(void)MathLib_fdiml:(PPCMachineState*)state { }
-(void)MathLib_feclearexcept:(PPCMachineState*)state { }
-(void)MathLib_fegetenv:(PPCMachineState*)state { }
-(void)MathLib_fegetexcept:(PPCMachineState*)state { }
-(void)MathLib_fegetround:(PPCMachineState*)state { }
-(void)MathLib_feholdexcept:(PPCMachineState*)state { }
-(void)MathLib_feraiseexcept:(PPCMachineState*)state { }
-(void)MathLib_fesetenv:(PPCMachineState*)state { }
-(void)MathLib_fesetexcept:(PPCMachineState*)state { }
-(void)MathLib_fesetround:(PPCMachineState*)state { }
-(void)MathLib_fetestexcept:(PPCMachineState*)state { }
-(void)MathLib_feupdateenv:(PPCMachineState*)state { }
-(void)MathLib_floor:(PPCMachineState*)state { }
-(void)MathLib_floorl:(PPCMachineState*)state { }
-(void)MathLib_fmax:(PPCMachineState*)state { }
-(void)MathLib_fmaxl:(PPCMachineState*)state { }
-(void)MathLib_fmin:(PPCMachineState*)state { }
-(void)MathLib_fminl:(PPCMachineState*)state { }
-(void)MathLib_fmod:(PPCMachineState*)state { }
-(void)MathLib_frexp:(PPCMachineState*)state { }
-(void)MathLib_frexpl:(PPCMachineState*)state { }
-(void)MathLib_gamma:(PPCMachineState*)state { }
-(void)MathLib_gammal:(PPCMachineState*)state { }
-(void)MathLib_hypot:(PPCMachineState*)state { }
-(void)MathLib_hypotl:(PPCMachineState*)state { }
-(void)MathLib_ldexp:(PPCMachineState*)state { }
-(void)MathLib_ldexpl:(PPCMachineState*)state { }
-(void)MathLib_ldtox80:(PPCMachineState*)state { }
-(void)MathLib_lgamma:(PPCMachineState*)state { }
-(void)MathLib_lgammal:(PPCMachineState*)state { }
-(void)MathLib_log:(PPCMachineState*)state { }
-(void)MathLib_log10:(PPCMachineState*)state { }
-(void)MathLib_log10l:(PPCMachineState*)state { }
-(void)MathLib_log1p:(PPCMachineState*)state { }
-(void)MathLib_log1pl:(PPCMachineState*)state { }
-(void)MathLib_log2:(PPCMachineState*)state { }
-(void)MathLib_log2l:(PPCMachineState*)state { }
-(void)MathLib_logb:(PPCMachineState*)state { }
-(void)MathLib_logbl:(PPCMachineState*)state { }
-(void)MathLib_logl:(PPCMachineState*)state { }
-(void)MathLib_modf:(PPCMachineState*)state { }
-(void)MathLib_modff:(PPCMachineState*)state { }
-(void)MathLib_modfl:(PPCMachineState*)state { }
-(void)MathLib_nan:(PPCMachineState*)state { }
-(void)MathLib_nanf:(PPCMachineState*)state { }
-(void)MathLib_nanl:(PPCMachineState*)state { }
-(void)MathLib_nearbyint:(PPCMachineState*)state { }
-(void)MathLib_nearbyintl:(PPCMachineState*)state { }
-(void)MathLib_nextafterd:(PPCMachineState*)state { }
-(void)MathLib_nextafterf:(PPCMachineState*)state { }
-(void)MathLib_nextafterl:(PPCMachineState*)state { }
-(void)MathLib_num2dec:(PPCMachineState*)state { }
-(void)MathLib_num2decl:(PPCMachineState*)state { }
-(void)MathLib_pow:(PPCMachineState*)state { }
-(void)MathLib_powl:(PPCMachineState*)state { }
-(void)MathLib_randomx:(PPCMachineState*)state { }
-(void)MathLib_relation:(PPCMachineState*)state { }
-(void)MathLib_relationl:(PPCMachineState*)state { }
-(void)MathLib_remainder:(PPCMachineState*)state { }
-(void)MathLib_remainderl:(PPCMachineState*)state { }
-(void)MathLib_remquo:(PPCMachineState*)state { }
-(void)MathLib_remquol:(PPCMachineState*)state { }
-(void)MathLib_rint:(PPCMachineState*)state { }
-(void)MathLib_rintl:(PPCMachineState*)state { }
-(void)MathLib_rinttol:(PPCMachineState*)state { }
-(void)MathLib_rinttoll:(PPCMachineState*)state { }
-(void)MathLib_round:(PPCMachineState*)state { }
-(void)MathLib_roundl:(PPCMachineState*)state { }
-(void)MathLib_roundtol:(PPCMachineState*)state { }
-(void)MathLib_roundtoll:(PPCMachineState*)state { }
-(void)MathLib_scalb:(PPCMachineState*)state { }
-(void)MathLib_scalbl:(PPCMachineState*)state { }
-(void)MathLib_sin:(PPCMachineState*)state { }
-(void)MathLib_sinh:(PPCMachineState*)state { }
-(void)MathLib_sinhl:(PPCMachineState*)state { }
-(void)MathLib_sinl:(PPCMachineState*)state { }
-(void)MathLib_sqrt:(PPCMachineState*)state { }
-(void)MathLib_sqrtl:(PPCMachineState*)state { }
-(void)MathLib_str2dec:(PPCMachineState*)state { }
-(void)MathLib_tan:(PPCMachineState*)state { }
-(void)MathLib_tanh:(PPCMachineState*)state { }
-(void)MathLib_tanhl:(PPCMachineState*)state { }
-(void)MathLib_tanl:(PPCMachineState*)state { }
-(void)MathLib_trunc:(PPCMachineState*)state { }
-(void)MathLib_truncl:(PPCMachineState*)state { }
-(void)MathLib_x80tod:(PPCMachineState*)state { }
-(void)MathLib_x80told:(PPCMachineState*)state { }

@end
