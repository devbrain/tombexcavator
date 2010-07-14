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

#include "lba.h"
#include <stdlib.h>

#ifdef MEM_DEBUG

#define NUNMEMORYBLOCKS 10000

int currentAllocatedMemory = 0;
int staticMemoryUsage = 0;

struct memoryBlockStruct {
	void* ptr;
	int size;
	boolean used;
};

memoryBlockStruct memoryBlockArray[NUNMEMORYBLOCKS];

void initMemorySystem() {
	int i;

	for (i = 0;i < NUNMEMORYBLOCKS;i++) {
		memoryBlockArray[i].used = false;
	}
}

void *Malloc(size_t size) {
	int i;
	void *ptr;

	for (i = 0;i < NUNMEMORYBLOCKS;i++) {
		if (memoryBlockArray[i].used == false)
			break;
	}

	if (i == NUNMEMORYBLOCKS && memoryBlockArray[NUNMEMORYBLOCKS].used) {
		printf("All blocks are used !\n");
		assert(true);
	}

	ptr = malloc(size);

	if (!ptr) {
		printf("Can't alloc %d!\n", size);
		exit(1);
	}

	memoryBlockArray[i].ptr = ptr;
	memoryBlockArray[i].size = size;
	memoryBlockArray[i].used = true;

	currentAllocatedMemory += size;

	return (ptr);
}

void MemFree(void* ptr) {
	int i;

	for (i = 0;i < NUNMEMORYBLOCKS;i++) {
		if (memoryBlockArray[i].used == true && memoryBlockArray[i].ptr == ptr) {
			free(memoryBlockArray[i].ptr);
			currentAllocatedMemory -= memoryBlockArray[i].size;
			memoryBlockArray[i].used = false;
			break;
		}
	}
}

#endif

