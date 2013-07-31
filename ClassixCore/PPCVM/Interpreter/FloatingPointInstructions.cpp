//
// FloatingPointInstructions.cpp
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

#include "Interpreter.h"
#include "FloatingPointStatus.h"
#include "TrapException.h"

// a macro conveys best the ugliness of the kludge
#define PRAY_NO_FLAGS_NO_EXCEPTIONS() \
	do { if (inst.Rc) Panic("FPU flags and exceptions are not implemented"); } while(0)

// I would like to state that single-precision instructions, as I understand them,
// are very silly

namespace PPCVM
{
	namespace Execution
	{
		void Interpreter::fabsx(Instruction inst)
		{
			Panic("fabsx is not implemented");
		}

		void Interpreter::faddsx(Instruction inst)
		{
			Panic("faddsx is not implemented");
		}
		
		void Interpreter::faddx(Instruction inst)
		{
			Panic("faddx is not implemented");
		}
		
		void Interpreter::fcmpo(Instruction inst)
		{
			Panic("fcmpo is not implemented");
		}
		
		void Interpreter::fcmpu(Instruction inst)
		{
			Panic("fcmpu is not implemented");
		}
		
		void Interpreter::fctiwx(Instruction inst)
		{
			Panic("fctiwx is not implemented");
		}
		
		void Interpreter::fctiwzx(Instruction inst)
		{
			Panic("fctiwzx is not implemented");
		}
		
		void Interpreter::fdivsx(Instruction inst)
		{
			Panic("fdivsx is not implemented");
		}
		
		void Interpreter::fdivx(Instruction inst)
		{
			state.fpr[inst.FS] = state.fpr[inst.FA] / state.fpr[inst.FB];
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::fmaddsx(Instruction inst)
		{
			Panic("fmaddsx is not implemented");
		}
		
		void Interpreter::fmaddx(Instruction inst)
		{
			state.fpr[inst.FS] = state.fpr[inst.FA] * state.fpr[inst.FC] + state.fpr[inst.FB];
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::fmrx(Instruction inst)
		{
			state.fpr[inst.FS] = state.fpr[inst.FB];
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::fmsubsx(Instruction inst)
		{
			Panic("fmsubsx is not implemented");
		}
		
		void Interpreter::fmsubx(Instruction inst)
		{
			Panic("fmsubx is not implemented");
		}
		
		void Interpreter::fmulsx(Instruction inst)
		{
			Panic("fmulsx is not implemented");
		}
		
		void Interpreter::fmulx(Instruction inst)
		{
			state.fpr[inst.FS] = state.fpr[inst.FA] * state.fpr[inst.FC];
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::fnabsx(Instruction inst)
		{
			Panic("fnabsx is not implemented");
		}
		
		void Interpreter::fnegx(Instruction inst)
		{
			Panic("fnegx is not implemented");
		}
		
		void Interpreter::fnmaddsx(Instruction inst)
		{
			Panic("fnmaddsx is not implemented");
		}
		
		void Interpreter::fnmaddx(Instruction inst)
		{
			Panic("fnmaddx is not implemented");
		}
		
		void Interpreter::fnmsubsx(Instruction inst)
		{
			Panic("fnmsubsx is not implemented");
		}
		
		void Interpreter::fnmsubx(Instruction inst)
		{
			Panic("fnmsubx is not implemented");
		}
		
		void Interpreter::fresx(Instruction inst)
		{
			Panic("fresx is not implemented");
		}
		
		void Interpreter::frspx(Instruction inst)
		{
			// TODO round with correct rounding mode
			float single = static_cast<float>(state.fpr[inst.FB]);
			state.fpr[inst.FS] = single;
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::frsqrtex(Instruction inst)
		{
			Panic("frsqrtex is not implemented");
		}
		
		void Interpreter::fselx(Instruction inst)
		{
			Panic("fselx is not implemented");
		}
		
		void Interpreter::fsqrtx(Instruction inst)
		{
			Panic("fsqrtx is not implemented");
		}
		
		void Interpreter::fsubsx(Instruction inst)
		{
			float result = static_cast<float>(state.fpr[inst.FA]) - static_cast<float>(state.fpr[inst.FB]);
			state.fpr[inst.FS] = result;
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::fsubx(Instruction inst)
		{
			state.fpr[inst.FS] = state.fpr[inst.FA] - state.fpr[inst.FB];
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
	}
}
