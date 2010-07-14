/*
Copyright (C) 2002-2004 The TwinE team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "stdlib.h"
#include <assert.h>

#ifdef GAME_DEBUG
#define todo(string) printf("TODO %s:%d %s\n", __FILE__, __LINE__, string)
#else // _DEBUG
#define todo(string)
#endif // _DEBUG

#ifdef GAME_DEBUG
#define assert_ptr(ptr) assert(((int)ptr!=0xCDCDCDCD) && (ptr!=NULL))
#else // _DEBUG
#define assert_ptr(ptr)
#endif // _DEBUG

int HQM_Init_Memory(int size);
int HQM_Alloc(int dataSize, unsigned char ** ptr);

void HQM_Free_All(void);
void HQM_Shrink_Last(unsigned char *bufferBrick2, int size);

