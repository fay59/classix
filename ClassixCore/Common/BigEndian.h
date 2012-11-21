//
//  BigEndian.h
//  pefdump
//
//  Created by Félix on 2012-10-21.
//  Copyright (c) 2012 Félix. All rights reserved.
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
	
	template<typename TTargetType, typename TNativeInt>
	struct BigEndianIntBase
	{
		TNativeInt AsBigEndian;
		
		inline BigEndianIntBase()
		: AsBigEndian(0)
		{ }
		
		static inline TTargetType FromBigEndian(TNativeInt value)
		{
			TTargetType result;
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
		
		inline TTargetType& operator=(TNativeInt that)
		{
			Set(that);
			return static_cast<TTargetType&>(*this);
		}
		
		inline operator TNativeInt() const
		{
			return Get();
		}
	};
		
#define BIGENDIAN_TYPE(name, type)	struct name : BigEndianIntBase<name, type> { name() {} name(type v) { Set(v); } }
	BIGENDIAN_TYPE(SInt16, int16_t);
	BIGENDIAN_TYPE(UInt16, uint16_t);
	BIGENDIAN_TYPE(SInt32, int32_t);
	BIGENDIAN_TYPE(UInt32, uint32_t);
	BIGENDIAN_TYPE(SInt64, int64_t);
	BIGENDIAN_TYPE(UInt64, uint64_t);
#undef BIGENDIAN_TYPE
		
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
