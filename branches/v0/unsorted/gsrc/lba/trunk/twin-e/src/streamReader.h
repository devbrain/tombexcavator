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

#ifndef _STREAMREADER_
#define _STREAMREADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lba.h"

#ifndef PCLIKE
#ifndef _EE
#include <shinobi.h>
#endif
#endif

#define NUM_SECTOR_IN_BUFFER (3)
#define BUFFER_SIZE (2048*NUM_SECTOR_IN_BUFFER)

struct streamReader {
#ifdef PCLIKE
	FILE* fileHandle;
#else
#ifndef _EE
	// DC
	GDFS fileHandle;
#endif
#ifdef _EE
	FILE* fileHandle;
#endif
#endif

	unsigned char buffer[BUFFER_SIZE];
	unsigned long int positionInBuffer;
	unsigned long int currentSector;
};

typedef struct streamReader streamReader;

boolean streamReader_open(streamReader* pThis, const char* fileName, int fatal);
void streamReader_close(streamReader* pThis);
void streamReader_feedBuffer(streamReader* pThis);

u8 streamReader_getU8(streamReader* pThis);
uint16 streamReader_getU16(streamReader* pThis);
uint32 streamReader_getU32(streamReader* pThis);

void streamReader_get(streamReader* pThis, void* destPtr, uint32 size);
void streamReader_seek(streamReader* pThis, uint32 seekPosition);

#endif // _STREAMREADER_

