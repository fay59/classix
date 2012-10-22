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

namespace PEF
{
	namespace BigEndian
	{
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
	}
}

#endif /* defined(__pefdump__BigEndian__) */
