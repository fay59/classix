//
//  BigEndian.h
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#ifndef __pefdump__BigEndian__
#define __pefdump__BigEndian__

#include "Endian.h"

namespace Common
{
	namespace CF
	{
		// taken from CoreFoundation
		
		typedef struct {uint32_t v;} SwappedFloat32;
		typedef struct {uint64_t v;} SwappedFloat64;
		
		inline uint32_t SwapInt32(uint32_t arg) {
#if CF_USE_OSBYTEORDER_H
			return OSSwapInt32(arg);
#else
			uint32_t result;
			result = ((arg & 0xFF) << 24) | ((arg & 0xFF00) << 8) | ((arg >> 8) & 0xFF00) | ((arg >> 24) & 0xFF);
			return result;
#endif
		}
		
		inline uint64_t SwapInt64(uint64_t arg) {
#if CF_USE_OSBYTEORDER_H
			return OSSwapInt64(arg);
#else
			union Swap {
				uint64_t sv;
				uint32_t ul[2];
			} tmp, result;
			tmp.sv = arg;
			result.ul[0] = SwapInt32(tmp.ul[1]);
			result.ul[1] = SwapInt32(tmp.ul[0]);
			return result.sv;
#endif
		}
		
		inline SwappedFloat64 ConvertDoubleHostToSwapped(double arg) {
			union Swap {
				double v;
				SwappedFloat64 sv;
			} result;
			result.v = arg;
#if __LITTLE_ENDIAN__
			result.sv.v = SwapInt64(result.sv.v);
#endif
			return result.sv;
		}
		
		inline double ConvertDoubleSwappedToHost(SwappedFloat64 arg) {
			union Swap {
				double v;
				SwappedFloat64 sv;
			} result;
			result.sv = arg;
#if __LITTLE_ENDIAN__
			result.sv.v = SwapInt64(result.sv.v);
#endif
			return result.v;
		}
		
		inline SwappedFloat32 ConvertFloatHostToSwapped(float arg) {
			union Swap {
				float v;
				SwappedFloat32 sv;
			} result;
			result.v = arg;
#if __LITTLE_ENDIAN__
			result.sv.v = SwapInt32(result.sv.v);
#endif
			return result.sv;
		}
		
		inline float ConvertFloatSwappedToHost(SwappedFloat32 arg) {
			union Swap {
				float v;
				SwappedFloat32 sv;
			} result;
			result.sv = arg;
#if __LITTLE_ENDIAN__
			result.sv.v = SwapInt32(result.sv.v);
#endif
			return result.v;
		}
	}
	
	struct UInt32
	{
		inline UInt32()
		{
			AsBigEndian = 0;
		}
		
		inline UInt32(uint32_t AsBigEndian)
		{
			this->AsBigEndian = AsBigEndian;
		}
		
		inline uint32_t Get() const
		{
			return EndianU32_BtoN(AsBigEndian);
		}
		
		inline void Set(uint32_t AsBigEndian)
		{
			this->AsBigEndian = EndianU32_NtoB(AsBigEndian);
		}
		
		inline operator uint32_t() const
		{
			return Get();
		}
		
		uint32_t AsBigEndian;
	};
	
	struct SInt32
	{
		inline SInt32()
		{
			AsBigEndian = 0;
		}
		
		inline SInt32(int32_t AsBigEndian)
		{
			this->AsBigEndian = AsBigEndian;
		}
		
		inline int32_t Get() const
		{
			return EndianS32_BtoN(AsBigEndian);
		}
		
		inline void Set(int32_t AsBigEndian)
		{
			this->AsBigEndian = EndianS32_NtoB(AsBigEndian);
		}
		
		inline operator int32_t() const
		{
			return Get();
		}
		
		int32_t AsBigEndian;
	};
	
	struct UInt16
	{
		inline UInt16()
		{
			AsBigEndian = 0;
		}
		
		inline UInt16(uint16_t AsBigEndian)
		{
			this->AsBigEndian = AsBigEndian;
		}
		
		inline uint16_t Get() const
		{
			return EndianU16_BtoN(AsBigEndian);
		}
		
		inline void Set(uint16_t AsBigEndian)
		{
			this->AsBigEndian = EndianU16_NtoB(AsBigEndian);
		}
		
		inline operator uint16_t() const
		{
			return Get();
		}
		
		uint16_t AsBigEndian;
	};
	
	struct SInt16
	{
		inline SInt16()
		{
			AsBigEndian = 0;
		}
		
		inline SInt16(int16_t AsBigEndian)
		{
			this->AsBigEndian = AsBigEndian;
		}
		
		inline int16_t Get() const
		{
			return EndianS16_BtoN(AsBigEndian);
		}
		
		inline void Set(int16_t AsBigEndian)
		{
			this->AsBigEndian = EndianS16_NtoB(AsBigEndian);
		}
		
		inline operator int16_t() const
		{
			return Get();
		}
		
		int16_t AsBigEndian;
	};
	
	struct Real32
	{
		inline Real32()
		{
			AsBigEndian.v = 0;
		}
		
		inline Real32(float AsBigEndian)
		{
			this->AsBigEndian = CF::ConvertFloatHostToSwapped(AsBigEndian);
		}
		
		inline float Get() const
		{
			return CF::ConvertFloatSwappedToHost(AsBigEndian);
		}
		
		inline void Set(float AsBigEndian)
		{
			this->AsBigEndian = CF::ConvertFloatHostToSwapped(AsBigEndian);
		}
		
		inline operator double() const
		{
			return Get();
		}
		
		CF::SwappedFloat32 AsBigEndian;
	};
	
	struct Real64
	{
		inline Real64()
		{
			AsBigEndian.v = 0;
		}
		
		inline Real64(double AsBigEndian)
		{
			this->AsBigEndian = CF::ConvertDoubleHostToSwapped(AsBigEndian);
		}
		
		inline double Get() const
		{
			return CF::ConvertDoubleSwappedToHost(AsBigEndian);
		}
		
		inline void Set(double AsBigEndian)
		{
			this->AsBigEndian = CF::ConvertDoubleHostToSwapped(AsBigEndian);
		}
		
		inline operator double() const
		{
			return Get();
		}
		
		CF::SwappedFloat64 AsBigEndian;
	};
}

#endif /* defined(__pefdump__BigEndian__) */
