//
// Todo.h
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

#ifndef Classix_Todo_h
#define Classix_Todo_h

#if TODOS

#include <stdio.h>

inline void TodoMessage(int* guard, size_t line, const char* function, const char* message)
{
	if (*guard == 0)
	{
		*guard = 1;
		printf("%s(%lu): %s", function, line, message);
	}
}

# define TODO(msg) do { static int warn_##__LINE__ = 0; TodoMessage(&warn_##__LINE__, __LINE__, __func__, (msg)); } while (0)

#else

#define TODO(x)

#endif

#endif
