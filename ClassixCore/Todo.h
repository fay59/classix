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

#ifdef __cplusplus
extern "C"
#endif
inline void TodoMessage(char* guard, const char* filePath, size_t line, const char* function, const char* message)
{
	if (*guard == 0)
	{
		*guard = 1;
		const char* fileName = filePath;
		for (const char* iter = filePath; *iter != 0; iter++)
		{
			if (*iter == '/')
				fileName = iter + 1;
		}
		
		printf("<TODO> %s:%lu: %s: %s\n", fileName, line, function, message);
	}
}

#define TODO(msg) do { static char warn_##__LINE__ = 0; TodoMessage(&warn_##__LINE__, __FILE__, __LINE__, __func__, (msg)); } while (0)

#else

#define TODO(x)

#endif

#endif
