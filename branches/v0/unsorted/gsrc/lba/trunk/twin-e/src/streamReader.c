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

#include "streamReader.h"

#ifdef UNIX
#define USE_IFOPEN
#endif

#ifdef _EE
#undef USE_IFOPEN
#endif

#ifdef USE_IFOPEN
#include <sys/types.h>
#include <dirent.h>

char ** split(char * s, char t) {
	static char * p[100];
	int i;

	for (i = 1, p[0] = s; *s; s++) {
		if (*s == t) {
			*s = 0;
			p[i++] = s + 1;
		}
	}
	p[i] = 0;

	return p;
}

FILE * ifopen(const char * path, const char * mode) {
	char * duppath = strdup(path), upath[1024], ** tab;
	FILE * f = 0;
	int opened = 0;

	tab = split(duppath, '/');

	upath[0] = 0;

	if (tab[0][0] == '/') {
		strcat(upath, "/");
		(*tab)++;
	} else {
		strcat(upath, "./");
	}

	while (1) {
		DIR * d;
		int found = 0;
		struct dirent * entry;

		if (!(d = opendir(upath))) {
			break;
		}

		while ((entry = readdir(d))) {
			if (strcasecmp(*tab, entry->d_name) == 0) {
				found = 1;
				strcat(upath, entry->d_name);
				break;
			}
		}

		closedir(d);

		if (found) {
			if (*(++tab)) {
				strcat(upath, "/");
			} else {
				f = fopen(upath, mode);
				opened = 1;
				break;
			}
		} else {
			break;
		}
	}
	free(duppath);

	if (!opened)
		f = fopen(path, mode);

	return f;
}
#endif

boolean streamReader_open(streamReader* pThis, const char* fileName, int fatal) {
	char fullPath[MAX_PATH];
	snprintf(fullPath, sizeof(fullPath) - 1, DATA_DIR"%s", fileName);
	fullPath[sizeof(fullPath) - 1] = '\0';
#ifndef DREAMCAST
#ifdef USE_IFOPEN
	pThis->fileHandle = ifopen(fullPath, "rb");
#else
	pThis->fileHandle = fopen(fullPath, "rb");
#endif
#else
	pThis->fileHandle = gdFsOpen(fullPath, NULL);
#endif

	if (pThis->fileHandle) {
		pThis->currentSector = 0;
		streamReader_feedBuffer(pThis);
		return true;
	} else {
		if (fatal) {
			printf("FATAL: Can't find %s\n", fullPath);
			exit(-1);
		}
		return false;
	}
}

void streamReader_feedBuffer(streamReader* pThis) {
#ifndef DREAMCAST
	fread(pThis->buffer, BUFFER_SIZE, 1, pThis->fileHandle);
#else
	gdFsRead(pThis->fileHandle, NUM_SECTOR_IN_BUFFER, pThis->buffer);
#endif
	pThis->positionInBuffer = 0;
}

/*u8 streamReader_getU8(streamReader* pThis)
{
  assert( pThis->fileHandle );
}

uint16 streamReader_getU16(streamReader* pThis)
{
  assert( pThis->fileHandle );
}

uint32 streamReader_getU32(streamReader* pThis)
{
  assert( pThis->fileHandle );
}*/

void streamReader_get(streamReader* pThis, void* destPtr, uint32 size) {
	if (BUFFER_SIZE - pThis->positionInBuffer >= size) {
		memcpy(destPtr, &pThis->buffer[pThis->positionInBuffer], size);
		pThis->positionInBuffer += size;
	} else {
		// buffer isn't filled enough...
		char* tempPtr = (char*)destPtr;

		// feed what we can:
		memcpy(tempPtr, &pThis->buffer[pThis->positionInBuffer], BUFFER_SIZE - pThis->positionInBuffer);
		tempPtr += BUFFER_SIZE - pThis->positionInBuffer;
		size -= BUFFER_SIZE - pThis->positionInBuffer;

		// feed the rest
		do {
			pThis->currentSector++;
			streamReader_feedBuffer(pThis);

			if (size >= BUFFER_SIZE) {
				memcpy(tempPtr, pThis->buffer, BUFFER_SIZE);
				tempPtr += BUFFER_SIZE;
				size -= BUFFER_SIZE;
			} else {
				memcpy(tempPtr, pThis->buffer, size);
				pThis->positionInBuffer += size;
				size = 0;
			}
		} while (size > 0);
	}
}

void streamReader_seek(streamReader* pThis, uint32 seekPosition) {
	uint32 sectorToSeek;

	sectorToSeek = seekPosition / 2048;

	/*  if((sectorToSeek >= pThis->currentSector) && (sectorToSeek < pThis->currentSector + NUM_SECTOR_IN_BUFFER ))// already at the good sector
	  {
	    pThis->positionInBuffer = (seekPosition - (sectorToSeek*2048));
	  }
	  else
	  {
	  } */

#ifndef DREAMCAST
	fseek(pThis->fileHandle, sectorToSeek * 2048, SEEK_SET);
#else
	gdFsSeek(pThis->fileHandle, sectorToSeek, GDD_SEEK_SET);
#endif

	pThis->currentSector = sectorToSeek;
	streamReader_feedBuffer(pThis);
	pThis->positionInBuffer = (seekPosition - (sectorToSeek * 2048));
}

void streamReader_close(streamReader* pThis) {
	if (pThis->fileHandle) {
#ifndef DREAMCAST
		fclose(pThis->fileHandle);
#else
		gdFsClose(pThis->fileHandle);
#endif
	}
}


