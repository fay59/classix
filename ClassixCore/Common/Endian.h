/*
 * Copyright (c) 1997-2008 by Apple Inc.. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
 
/*
     File:       Endian.h
 
     Contains:   Endian swapping utilties
 
     Version:    CarbonCore-769~1
  
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/

// Modified and stripped down by FŽlix Cloutier on Oct 21, 2012

#ifndef __ENDIAN__
#define __ENDIAN__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif

#pragma pack(push, 2)

/*
    This file provides Endian Flipping routines for dealing with converting data
    between Big-Endian and Little-Endian machines.  These routines are useful
    when writing code to compile for both Big and Little Endian machines and  
    which must handle other endian number formats, such as reading or writing 
    to a file or network packet.
    
    These routines are named as follows:
    
        Endian<U><W>_<S>to<D>

    where
        <U> is whether the integer is signed ('S') or unsigned ('U')
        <W> is integer bit width: 16, 32, or 64 
        <S> is the source endian format: 'B' for big, 'L' for little, or 'N' for native
        <D> is the destination endian format: 'B' for big, 'L' for little, or 'N' for native
    
    For example, to convert a Big Endian 32-bit unsigned integer to the current native format use:
        
        long i = EndianU32_BtoN(data);
        
    This file is set up so that the function macro to nothing when the target runtime already
    is the desired format (e.g. on Big Endian machines, EndianU32_BtoN() macros away).
            
    If long long's are not supported, you cannot get 64-bit quantities as a single value.
    The macros are not defined in that case.

    For gcc, the macros build on top of the inline byte swapping
    routines from <libkern/OSByteOrder.h>, which may have better performance.
    
    
                                <<< W A R N I N G >>>
    
    It is very important not to put any autoincrements inside the macros.  This 
    will produce erroneous results because each time the address is accessed in the macro, 
    the increment occurs.
    
 */
/*
 If building for Mac OS X with GCC, use the inline versions.
 Otherwise, use the macros.
*/
#ifdef __GNUC__

#include <libkern/OSByteOrder.h>

/*
  Implement low level Å_Swap functions.
  
   These *always* swap the data, without regard of its underlying
 endian'ness.  If a constant, these will use the constant swapper
   macro.  
*/

#define Endian16_Swap(value)       (uint16_t) (__builtin_constant_p(value) ? OSSwapConstInt16(value) : OSSwapInt16(value))
#define Endian32_Swap(value)      (uint32_t) (__builtin_constant_p(value) ? OSSwapConstInt32(value) : OSSwapInt32(value))
#define Endian64_Swap(value)      (uint64_t) (__builtin_constant_p(value) ? OSSwapConstInt64(value) : OSSwapInt64(value))

#else

/*
    Macro versions for non-gcc compilers
*/
#define Endian16_Swap(value) \
      ((((uint16_t)((value) & 0x00FF)) << 8) | \
         (((uint16_t)((value) & 0xFF00)) >> 8))

#define Endian32_Swap(value) \
         ((((uint32_t)((value) & 0x000000FF)) << 24) | \
         (((uint32_t)((value) & 0x0000FF00)) << 8) | \
         (((uint32_t)((value) & 0x00FF0000)) >> 8) | \
         (((uint32_t)((value) & 0xFF000000)) >> 24))

#if TYPE_LONGLONG
        #define Endian64_Swap(value)                                \
                (((((uint64_t)value)<<56) & 0xFF00000000000000ULL)  | \
                 ((((uint64_t)value)<<40) & 0x00FF000000000000ULL)  | \
                 ((((uint64_t)value)<<24) & 0x0000FF0000000000ULL)  | \
                 ((((uint64_t)value)<< 8) & 0x000000FF00000000ULL)  | \
                 ((((uint64_t)value)>> 8) & 0x00000000FF000000ULL)  | \
                 ((((uint64_t)value)>>24) & 0x0000000000FF0000ULL)  | \
                 ((((uint64_t)value)>>40) & 0x000000000000FF00ULL)  | \
                 ((((uint64_t)value)>>56) & 0x00000000000000FFULL))
#else
/* 
    Note: When using compilers that don't support "long long",
          Endian64_Swap must be implemented as glue. 
*/
#ifdef __cplusplus
    inline static uint64_t Endian64_Swap(uint64_t value)
    {
        uint64_t temp;
        ((UnsignedWide*)&temp)->lo = Endian32_Swap(((UnsignedWide*)&value)->hi);
        ((UnsignedWide*)&temp)->hi = Endian32_Swap(((UnsignedWide*)&value)->lo);
        return temp;
    }
#else
/*
 *  Endian64_Swap()
 *  
 *  Mac OS X threading:
 *    Thread safe since version 10.3
 *  
 *  Availability:
 *    Mac OS X:         in version 10.3 and later in CoreServices.framework
 *    CarbonLib:        not available
 *    Non-Carbon CFM:   not available
 */
extern uint64_t 
Endian64_Swap(uint64_t value)                                   AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER;


#endif
#endif  /* TYPE_LONGLONG */

#endif  /* defined(__GNUC__) */


/*
    Macro away no-op functions
*/
#if TARGET_RT_BIG_ENDIAN
 #define EndianS16_BtoN(value)               (value)
    #define EndianS16_NtoB(value)               (value)
    #define EndianU16_BtoN(value)               (value)
    #define EndianU16_NtoB(value)               (value)
    #define EndianS32_BtoN(value)               (value)
    #define EndianS32_NtoB(value)               (value)
    #define EndianU32_BtoN(value)               (value)
    #define EndianU32_NtoB(value)               (value)
    #define EndianS64_BtoN(value)               (value)
    #define EndianS64_NtoB(value)               (value)
    #define EndianU64_BtoN(value)               (value)
    #define EndianU64_NtoB(value)               (value)
#else
  #define EndianS16_LtoN(value)               (value)
    #define EndianS16_NtoL(value)               (value)
    #define EndianU16_LtoN(value)               (value)
    #define EndianU16_NtoL(value)               (value)
    #define EndianS32_LtoN(value)               (value)
    #define EndianS32_NtoL(value)               (value)
    #define EndianU32_LtoN(value)               (value)
    #define EndianU32_NtoL(value)               (value)
    #define EndianS64_LtoN(value)               (value)
    #define EndianS64_NtoL(value)               (value)
    #define EndianU64_LtoN(value)               (value)
    #define EndianU64_NtoL(value)               (value)
#endif



/*
    Map native to actual
*/
#if TARGET_RT_BIG_ENDIAN
   #define EndianS16_LtoN(value)               EndianS16_LtoB(value)
  #define EndianS16_NtoL(value)               EndianS16_BtoL(value)
  #define EndianU16_LtoN(value)               EndianU16_LtoB(value)
  #define EndianU16_NtoL(value)               EndianU16_BtoL(value)
  #define EndianS32_LtoN(value)               EndianS32_LtoB(value)
  #define EndianS32_NtoL(value)               EndianS32_BtoL(value)
  #define EndianU32_LtoN(value)               EndianU32_LtoB(value)
  #define EndianU32_NtoL(value)               EndianU32_BtoL(value)
  #define EndianS64_LtoN(value)               EndianS64_LtoB(value)
  #define EndianS64_NtoL(value)               EndianS64_BtoL(value)
  #define EndianU64_LtoN(value)               EndianU64_LtoB(value)
  #define EndianU64_NtoL(value)               EndianU64_BtoL(value)
#else
    #define EndianS16_BtoN(value)               EndianS16_BtoL(value)
  #define EndianS16_NtoB(value)               EndianS16_LtoB(value)
  #define EndianU16_BtoN(value)               EndianU16_BtoL(value)
  #define EndianU16_NtoB(value)               EndianU16_LtoB(value)
  #define EndianS32_BtoN(value)               EndianS32_BtoL(value)
  #define EndianS32_NtoB(value)               EndianS32_LtoB(value)
  #define EndianU32_BtoN(value)               EndianU32_BtoL(value)
  #define EndianU32_NtoB(value)               EndianU32_LtoB(value)
  #define EndianS64_BtoN(value)               EndianS64_BtoL(value)
  #define EndianS64_NtoB(value)               EndianS64_LtoB(value)
  #define EndianU64_BtoN(value)               EndianU64_BtoL(value)
  #define EndianU64_NtoB(value)               EndianU64_LtoB(value)
#endif



/*
    Implement ÅLtoB and ÅBtoL
*/
#define EndianS16_LtoB(value)              ((int16_t)Endian16_Swap(value))
#define EndianS16_BtoL(value)                ((int16_t)Endian16_Swap(value))
#define EndianU16_LtoB(value)                ((uint16_t)Endian16_Swap(value))
#define EndianU16_BtoL(value)                ((uint16_t)Endian16_Swap(value))
#define EndianS32_LtoB(value)                ((int32_t)Endian32_Swap(value))
#define EndianS32_BtoL(value)                ((int32_t)Endian32_Swap(value))
#define EndianU32_LtoB(value)                ((uint32_t)Endian32_Swap(value))
#define EndianU32_BtoL(value)                ((uint32_t)Endian32_Swap(value))
#define EndianS64_LtoB(value)                ((int64_t)Endian64_Swap((uint64_t)value))
#define EndianS64_BtoL(value)                ((int64_t)Endian64_Swap((uint64_t)value))
#define EndianU64_LtoB(value)                ((uint64_t)Endian64_Swap(value))
#define EndianU64_BtoL(value)                ((uint64_t)Endian64_Swap(value))


#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* __ENDIAN__ */

