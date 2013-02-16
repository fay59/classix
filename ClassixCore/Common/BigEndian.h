//
// BigEndian.h
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

#ifndef __pefdump__BigEndian__
#define __pefdump__BigEndian__

#include <libkern/OSByteOrder.h>

namespace Common
{
	namespace CF
	{
		struct SwappedFloat32 { uint32_t v; };
		struct SwappedFloat64 { uint64_t v; };
		
		template<typename TIntType>
		inline TIntType Identity(TIntType integer)
		{
			return integer;
		}
		
		template<typename TInt>
		inline TInt SwapInt(TInt arg)
		{
			static_assert(sizeof(TInt) == 2 || sizeof(TInt) == 4 || sizeof(TInt) == 8, "Unsupported type");
			
			switch (sizeof(TInt))
			{
				case 2: return OSSwapInt16(arg);
				case 4: return OSSwapInt32(arg);
				case 8: return OSSwapInt64(arg);
			}
			
			// shut up the compiler; this can never happen
			return 0;
		}
		
#if __LITTLE_ENDIAN__
#define	BigToHost		::Common::CF::SwapInt
#define HostToBig		::Common::CF::SwapInt
#define LittleToHost	::Common::CF::Identity
#define HostToLittle	::Common::CF::Identity
#else
#define BigToHost		::Common::CF::Identity
#define HostToBig		::Common::CF::Identity
#define LittleToHost	::Common::CF::SwapInt
#define HostToLittle	::Common::CF::SwapInt
#endif
		
		inline SwappedFloat64 ConvertDoubleHostToSwapped(double arg) {
			union Swap {
				double v;
				SwappedFloat64 sv;
			} result;
			result.v = arg;
#if __LITTLE_ENDIAN__
			result.sv.v = SwapInt(result.sv.v);
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
			result.sv.v = SwapInt(result.sv.v);
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
			result.sv.v = SwapInt(result.sv.v);
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
			result.sv.v = SwapInt(result.sv.v);
#endif
			return result.v;
		}
	}
	
	template<typename TNativeInt>
	struct BigEndianInt
	{
		typedef BigEndianInt<TNativeInt> self;
		
		TNativeInt AsBigEndian;
		
		inline BigEndianInt()
		: AsBigEndian(0)
		{ }
		
		inline explicit BigEndianInt(TNativeInt nativeInt)
		{
			Set(nativeInt);
		}
		
		static inline self FromBigEndian(TNativeInt value)
		{
			self result;
			result.AsBigEndian = value;
			return result;
		}
		
		inline TNativeInt Get() const
		{
			return BigToHost(AsBigEndian);
		}
		
		inline void Set(TNativeInt that)
		{
			AsBigEndian = HostToBig(that);
		}
		
		inline self& operator=(TNativeInt that)
		{
			Set(that);
			return *this;
		}
		
		inline operator TNativeInt() const
		{
			return Get();
		}
	};
		
	typedef BigEndianInt<int16_t>	SInt16;
	typedef BigEndianInt<uint16_t>	UInt16;
	typedef BigEndianInt<int32_t>	SInt32;
	typedef BigEndianInt<uint32_t>	UInt32;
	typedef BigEndianInt<int64_t>	SInt64;
	typedef BigEndianInt<uint64_t>	UInt64;
		
	struct Real32
	{
		inline Real32()
		{
			AsBigEndian.v = 0;
		}
		
		inline float Get() const
		{
			return CF::ConvertFloatSwappedToHost(AsBigEndian);
		}
		
		inline void Set(float nativeEndian)
		{
			this->AsBigEndian = CF::ConvertFloatHostToSwapped(nativeEndian);
		}
		
		inline Real32& operator=(float that)
		{
			Set(that);
			return *this;
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
		
		inline double Get() const
		{
			return CF::ConvertDoubleSwappedToHost(AsBigEndian);
		}
		
		inline void Set(double nativeEndian)
		{
			this->AsBigEndian = CF::ConvertDoubleHostToSwapped(nativeEndian);
		}
		
		inline Real64& operator=(double that)
		{
			Set(that);
			return *this;
		}
		
		inline operator double() const
		{
			return Get();
		}
		
		CF::SwappedFloat64 AsBigEndian;
	};
}

#endif /* defined(__pefdump__BigEndian__) */
