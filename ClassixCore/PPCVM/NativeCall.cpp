//
//  NativeCall.cpp
//  pefdump
//
//  Created by Félix on 2012-11-10.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#include "NativeCall.h"

extern const uint32_t PPCVM::Execution::NativeTag = 0x4e544956; // 'NTIV'

PPCVM::Execution::NativeCall::NativeCall(NativeCallback cb)
{
	Tag = NativeTag;
	this->Callback = cb;
}