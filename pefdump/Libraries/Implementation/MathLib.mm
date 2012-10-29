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
-(void)MathLib___fpclassify:(MachineState*)state { }
-(void)MathLib___fpclassifyd:(MachineState*)state { }
-(void)MathLib___fpclassifyf:(MachineState*)state { }
-(void)MathLib___inf:(MachineState*)state { }
-(void)MathLib___isfinite:(MachineState*)state { }
-(void)MathLib___isfinited:(MachineState*)state { }
-(void)MathLib___isfinitef:(MachineState*)state { }
-(void)MathLib___isnan:(MachineState*)state { }
-(void)MathLib___isnand:(MachineState*)state { }
-(void)MathLib___isnanf:(MachineState*)state { }
-(void)MathLib___isnormal:(MachineState*)state { }
-(void)MathLib___isnormald:(MachineState*)state { }
-(void)MathLib___isnormalf:(MachineState*)state { }
-(void)MathLib___signbit:(MachineState*)state { }
-(void)MathLib___signbitd:(MachineState*)state { }
-(void)MathLib___signbitf:(MachineState*)state { }
-(void)MathLib_acos:(MachineState*)state { }
-(void)MathLib_acosh:(MachineState*)state { }
-(void)MathLib_acoshl:(MachineState*)state { }
-(void)MathLib_acosl:(MachineState*)state { }
-(void)MathLib_annuity:(MachineState*)state { }
-(void)MathLib_asin:(MachineState*)state { }
-(void)MathLib_asinh:(MachineState*)state { }
-(void)MathLib_asinhl:(MachineState*)state { }
-(void)MathLib_asinl:(MachineState*)state { }
-(void)MathLib_atan:(MachineState*)state { }
-(void)MathLib_atan2:(MachineState*)state { }
-(void)MathLib_atan2l:(MachineState*)state { }
-(void)MathLib_atanh:(MachineState*)state { }
-(void)MathLib_atanhl:(MachineState*)state { }
-(void)MathLib_atanl:(MachineState*)state { }
-(void)MathLib_ceil:(MachineState*)state { }
-(void)MathLib_ceill:(MachineState*)state { }
-(void)MathLib_compound:(MachineState*)state { }
-(void)MathLib_copysign:(MachineState*)state { }
-(void)MathLib_copysignl:(MachineState*)state { }
-(void)MathLib_cos:(MachineState*)state { }
-(void)MathLib_cosh:(MachineState*)state { }
-(void)MathLib_coshl:(MachineState*)state { }
-(void)MathLib_cosl:(MachineState*)state { }
-(void)MathLib_dec2f:(MachineState*)state { }
-(void)MathLib_dec2l:(MachineState*)state { }
-(void)MathLib_dec2num:(MachineState*)state { }
-(void)MathLib_dec2numl:(MachineState*)state { }
-(void)MathLib_dec2s:(MachineState*)state { }
-(void)MathLib_dec2str:(MachineState*)state { }
-(void)MathLib_dtox80:(MachineState*)state { }
-(void)MathLib_erf:(MachineState*)state { }
-(void)MathLib_erfc:(MachineState*)state { }
-(void)MathLib_erfcl:(MachineState*)state { }
-(void)MathLib_erfl:(MachineState*)state { }
-(void)MathLib_exp:(MachineState*)state { }
-(void)MathLib_exp2:(MachineState*)state { }
-(void)MathLib_exp2l:(MachineState*)state { }
-(void)MathLib_expl:(MachineState*)state { }
-(void)MathLib_expm1:(MachineState*)state { }
-(void)MathLib_expm1l:(MachineState*)state { }
-(void)MathLib_fabs:(MachineState*)state { }
-(void)MathLib_fabsl:(MachineState*)state { }
-(void)MathLib_fdim:(MachineState*)state { }
-(void)MathLib_fdiml:(MachineState*)state { }
-(void)MathLib_feclearexcept:(MachineState*)state { }
-(void)MathLib_fegetenv:(MachineState*)state { }
-(void)MathLib_fegetexcept:(MachineState*)state { }
-(void)MathLib_fegetround:(MachineState*)state { }
-(void)MathLib_feholdexcept:(MachineState*)state { }
-(void)MathLib_feraiseexcept:(MachineState*)state { }
-(void)MathLib_fesetenv:(MachineState*)state { }
-(void)MathLib_fesetexcept:(MachineState*)state { }
-(void)MathLib_fesetround:(MachineState*)state { }
-(void)MathLib_fetestexcept:(MachineState*)state { }
-(void)MathLib_feupdateenv:(MachineState*)state { }
-(void)MathLib_floor:(MachineState*)state { }
-(void)MathLib_floorl:(MachineState*)state { }
-(void)MathLib_fmax:(MachineState*)state { }
-(void)MathLib_fmaxl:(MachineState*)state { }
-(void)MathLib_fmin:(MachineState*)state { }
-(void)MathLib_fminl:(MachineState*)state { }
-(void)MathLib_fmod:(MachineState*)state { }
-(void)MathLib_frexp:(MachineState*)state { }
-(void)MathLib_frexpl:(MachineState*)state { }
-(void)MathLib_gamma:(MachineState*)state { }
-(void)MathLib_gammal:(MachineState*)state { }
-(void)MathLib_hypot:(MachineState*)state { }
-(void)MathLib_hypotl:(MachineState*)state { }
-(void)MathLib_ldexp:(MachineState*)state { }
-(void)MathLib_ldexpl:(MachineState*)state { }
-(void)MathLib_ldtox80:(MachineState*)state { }
-(void)MathLib_lgamma:(MachineState*)state { }
-(void)MathLib_lgammal:(MachineState*)state { }
-(void)MathLib_log:(MachineState*)state { }
-(void)MathLib_log10:(MachineState*)state { }
-(void)MathLib_log10l:(MachineState*)state { }
-(void)MathLib_log1p:(MachineState*)state { }
-(void)MathLib_log1pl:(MachineState*)state { }
-(void)MathLib_log2:(MachineState*)state { }
-(void)MathLib_log2l:(MachineState*)state { }
-(void)MathLib_logb:(MachineState*)state { }
-(void)MathLib_logbl:(MachineState*)state { }
-(void)MathLib_logl:(MachineState*)state { }
-(void)MathLib_modf:(MachineState*)state { }
-(void)MathLib_modff:(MachineState*)state { }
-(void)MathLib_modfl:(MachineState*)state { }
-(void)MathLib_nan:(MachineState*)state { }
-(void)MathLib_nanf:(MachineState*)state { }
-(void)MathLib_nanl:(MachineState*)state { }
-(void)MathLib_nearbyint:(MachineState*)state { }
-(void)MathLib_nearbyintl:(MachineState*)state { }
-(void)MathLib_nextafterd:(MachineState*)state { }
-(void)MathLib_nextafterf:(MachineState*)state { }
-(void)MathLib_nextafterl:(MachineState*)state { }
-(void)MathLib_num2dec:(MachineState*)state { }
-(void)MathLib_num2decl:(MachineState*)state { }
-(void)MathLib_pow:(MachineState*)state { }
-(void)MathLib_powl:(MachineState*)state { }
-(void)MathLib_randomx:(MachineState*)state { }
-(void)MathLib_relation:(MachineState*)state { }
-(void)MathLib_relationl:(MachineState*)state { }
-(void)MathLib_remainder:(MachineState*)state { }
-(void)MathLib_remainderl:(MachineState*)state { }
-(void)MathLib_remquo:(MachineState*)state { }
-(void)MathLib_remquol:(MachineState*)state { }
-(void)MathLib_rint:(MachineState*)state { }
-(void)MathLib_rintl:(MachineState*)state { }
-(void)MathLib_rinttol:(MachineState*)state { }
-(void)MathLib_rinttoll:(MachineState*)state { }
-(void)MathLib_round:(MachineState*)state { }
-(void)MathLib_roundl:(MachineState*)state { }
-(void)MathLib_roundtol:(MachineState*)state { }
-(void)MathLib_roundtoll:(MachineState*)state { }
-(void)MathLib_scalb:(MachineState*)state { }
-(void)MathLib_scalbl:(MachineState*)state { }
-(void)MathLib_sin:(MachineState*)state { }
-(void)MathLib_sinh:(MachineState*)state { }
-(void)MathLib_sinhl:(MachineState*)state { }
-(void)MathLib_sinl:(MachineState*)state { }
-(void)MathLib_sqrt:(MachineState*)state { }
-(void)MathLib_sqrtl:(MachineState*)state { }
-(void)MathLib_str2dec:(MachineState*)state { }
-(void)MathLib_tan:(MachineState*)state { }
-(void)MathLib_tanh:(MachineState*)state { }
-(void)MathLib_tanhl:(MachineState*)state { }
-(void)MathLib_tanl:(MachineState*)state { }
-(void)MathLib_trunc:(MachineState*)state { }
-(void)MathLib_truncl:(MachineState*)state { }
-(void)MathLib_x80tod:(MachineState*)state { }
-(void)MathLib_x80told:(MachineState*)state { }

@end
