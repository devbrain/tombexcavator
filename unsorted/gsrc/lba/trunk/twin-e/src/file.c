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

int checkIfFileExist(char *fileName) {
	FILE *fileHandle;

	fileHandle = fopen(fileName, "rb");

	if (fileHandle) {
		fclose(fileHandle);
		return(1);
	}

	return (0);
}

FILE *OpenWrite(char *fileName) {
	FILE *fileHandle;

	if (!fileName)
		return (NULL);

	fileHandle = fopen(fileName, "wb+");

	if (!fileHandle) {
		printf("%s can't be found !\n", fileName);
	}

	return (fileHandle);
}

FILE *OpenRead(char *fileName) {
	FILE *fileHandle;

	if (!fileName)
		return (NULL);

	fileHandle = fopen(fileName, "rb");

	if (!fileHandle) {
		printf("%s can't be found !\n", fileName);
	}

	return (fileHandle);
}

int Read(FILE * resourceFile, char *ptr, int length) {
	if (!resourceFile)
		return (1);

	fread((char *) ptr, length, 1, resourceFile);

	return (0);
}

void Close(FILE * resourceFile) {
	fclose(resourceFile);
}

void dumpFile(char *name, char *ptr, int size) {
	FILE *file;

	file = fopen(name, "wb+");
	fwrite(ptr, size, 1, file);

	fclose(file);
}

void Mkdir (const char *thePath) {
#ifdef UNIX
	mkdir(thePath, 0777);
#else
	mkdir(thePath);
#endif
}
