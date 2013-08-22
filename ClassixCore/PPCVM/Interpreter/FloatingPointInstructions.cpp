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
#include "InvalidInstructionException.h"
#include "Todo.h"

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
			throw InvalidInstructionException(inst);
		}

		void Interpreter::faddsx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::faddx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fcmpo(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fcmpu(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fctiwx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fctiwzx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fdivsx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fdivx(Instruction inst)
		{
			state.fpr[inst.FS] = state.fpr[inst.FA] / state.fpr[inst.FB];
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::fmaddsx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
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
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fmsubx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fmulsx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fmulx(Instruction inst)
		{
			state.fpr[inst.FS] = state.fpr[inst.FA] * state.fpr[inst.FC];
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::fnabsx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fnegx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fnmaddsx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fnmaddx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fnmsubsx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fnmsubx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fresx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::frspx(Instruction inst)
		{
			TODO("frsp should round with the correct rounding mode");
			float single = static_cast<float>(state.fpr[inst.FB]);
			state.fpr[inst.FS] = single;
			PRAY_NO_FLAGS_NO_EXCEPTIONS();
		}
		
		void Interpreter::frsqrtex(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fselx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
		}
		
		void Interpreter::fsqrtx(Instruction inst)
		{
			throw InvalidInstructionException(inst);
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
