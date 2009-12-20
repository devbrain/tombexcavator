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

#include "HQRlib.h"

#include "type.h"
#include "streamReader.h"

#ifdef DREAMCAST
#include <shinobi.h>
#endif

#ifdef UNIX
#define USE_IFOPEN
#endif

#ifdef _EE
#undef USE_IFOPEN
#endif

streamReader fileReader;

int HQR_File_checkIfFileExist(char *fileName) {
#ifndef DREAMCAST
	FILE *fileHandle;

	fileHandle = fopen(fileName, "rb");

	if (fileHandle) {
		fclose(fileHandle);
		return(1);
	}

	return (0);
#else
	GDFS fileHandle;

	fileHandle = gdFsOpen(fileName, NULL);

	if (fileHandle) {
		gdFsClose(fileHandle);
		return(1);
	}

	return (0);
#endif
}

#ifdef USE_IFOPEN
FILE * ifopen(const char *, const char *);
#endif

#ifndef DREAMCAST
FILE* HQR_File_OpenRead(char *fileName) {
	FILE *fileHandle;

	if (!fileName)
		return (NULL);

#ifdef USE_IFOPEN
	fileHandle = ifopen(fileName, "rb");
#else
	fileHandle = fopen(fileName, "rb");
#endif

	if (!fileHandle) {
		printf("%s can't be found !\n", fileName);
	}

	return (fileHandle);
}
#else
// DC version
GDFS HQR_File_OpenRead(char *fileName) {
	/*GDFS fs;
	  FILE *fileHandle;

	  if (!fileName)
	{
	  return(NULL);
	}

	fs = gdFsOpen(fileName, NULL);

	return(fs);*/
	return(NULL);
}
#endif

#ifndef DREAMCAST
int HQR_File_Read(FILE * resourceFile, char *ptr, int length) {
	if (!resourceFile)
		return (1);

	fread((char *) ptr, length, 1, resourceFile);

	return (0);
}
#else
//DC
int HQR_File_Read(GDFS resourceFile, char *ptr, int length) {
	unsigned char buffer[2048];
	int numSectors;
	int lastSectorLength;

	numSectors = length / 2048;
	lastSectorLength = length % 2048;

	if (numSectors) {
		gdFsRead(resourceFile, numSectors, ptr);

		ptr += 2048 * numSectors;
	}

	// read last sector

	gdFsRead(resourceFile, 1, buffer);
	memcpy(ptr, buffer, lastSectorLength);

	return (0);
}
#endif

#ifndef DREAMCAST
void HQR_File_Close(FILE * resourceFile) {
	fclose(resourceFile);
}
#else
//DC
void HQR_File_Close(GDFS resourceFile) {
	gdFsClose(resourceFile);
}
#endif

void HQR_Destroy_Ressource(hqr_entry* resource) {
	MemFree(resource->ptr);
	MemFree(resource);
}

hqr_entry* HQR_Init_Ressource(char *fileName, int sizeOfBuffer, int numOfEntriesInBuffer) {
	hqr_entry *hqr_ptr;
	unsigned char *dataPtr;

	if (!HQR_File_checkIfFileExist(fileName))
		return (NULL);

	hqr_ptr = Malloc(numOfEntriesInBuffer * sizeof(subHqr) + sizeof(hqr_entry));

	if (!hqr_ptr)
		return (NULL);

	dataPtr = Malloc(sizeOfBuffer + 500);

	if (!dataPtr)
		return (NULL);

	strcpy(hqr_ptr->fileName, fileName);
	hqr_ptr->preloadedResource = false;

	hqr_ptr->size1 = sizeOfBuffer;
	hqr_ptr->remainingSize = sizeOfBuffer;
	hqr_ptr->numEntriesMax = numOfEntriesInBuffer;
	hqr_ptr->numCurrentlyUsedEntries = 0;
	hqr_ptr->ptr = dataPtr;

	return (hqr_ptr);
}

static long int getFileSize(const char* resourceName) {
#ifndef DREAMCAST
	FILE* fHandle;
	int size;

	fHandle = fopen(resourceName, "rb");
	fseek(fHandle, 0L, SEEK_END);
	size = ftell(fHandle);
	fclose(fHandle);
	return size;
#else
	GDFS fs;
	signed long int length;
	fs = gdFsOpen(resourceName, NULL);
	gdFsGetFileSize(fs, &length);
	gdFsClose(fs);
	return length;
#endif
}

static void fileRead(const char* resourceName, long int size, unsigned char* buffer) {
#ifndef DREAMCAST
	FILE* fHandle;
	fHandle = fopen(resourceName, "rb");
	fread(buffer, size, 1, fHandle);
	fclose(fHandle);
#else
	if (!streamReader_open(&fileReader, resourceName))
		return;

	streamReader_get(&fileReader, buffer, size);

	streamReader_close(&fileReader);

#endif
}

hqr_entry* HQR_Init_RessourcePreload(char *resourceName) {
	hqr_entry *hqr_ptr;
	unsigned char **ptrTable;
	int fileSize;
	unsigned char* buffer;
	int numEntries;
	int i;

	if (!HQR_File_checkIfFileExist(resourceName))
		return (NULL);

	hqr_ptr = Malloc(sizeof(hqr_entry));

	if (!hqr_ptr)
		return (NULL);

	fileSize = getFileSize(resourceName);
	buffer = Malloc(fileSize);
	fileRead(resourceName, fileSize, buffer);

	numEntries = (*(unsigned int*)buffer) / 4; // always alligned

	ptrTable = Malloc(numEntries * sizeof(unsigned char*));
	hqr_ptr->sizeArray = Malloc(numEntries * sizeof(int));

	for (i = 0;i < numEntries - 1;i++) {
		int offsetToData;
		int dataSize;
		int compressedSize;
		short int mode;

		offsetToData = *(unsigned int*)(buffer + 4 * i); // always alligned

		dataSize = READ_LE_U32(buffer + offsetToData);
		compressedSize = READ_LE_U32(buffer + offsetToData + 4);
		mode = READ_LE_U16(buffer + offsetToData + 8);

		ptrTable[i] = Malloc(dataSize);
		hqr_ptr->sizeArray[i] = dataSize;

		if (mode == 0) { // uncompressed data
			memcpy(ptrTable[i], buffer + offsetToData + 10, dataSize);
		} else
			if (mode == 1) { // compressed data
				HQR_Expand(dataSize, ptrTable[i], buffer + offsetToData + 10);
			}
	}

	free(buffer);

	strcpy(hqr_ptr->fileName, resourceName);
	hqr_ptr->preloadedResource = true;

	hqr_ptr->ptrArray = ptrTable;

	return (hqr_ptr);
}

int Load_HQR(char *resourceName, unsigned char* ptr, int imageNumber) {
	unsigned int headerSize;
	unsigned int offToImage;
	unsigned int dataSize;
	unsigned int compressedSize;
	unsigned short int mode;

	if (!streamReader_open(&fileReader, resourceName, 1))
		return 0;

	streamReader_get(&fileReader, &headerSize, 4);
	headerSize = convertDWFromLE(headerSize);

	if (imageNumber >= headerSize / 4) {
		streamReader_close(&fileReader);
		return 0;
	}

	streamReader_seek(&fileReader, imageNumber * 4);
	streamReader_get(&fileReader, &offToImage, 4);
	offToImage = convertDWFromLE(offToImage);

	streamReader_seek(&fileReader, offToImage);
	streamReader_get(&fileReader, &dataSize , 4);
	dataSize = convertDWFromLE(dataSize);
	streamReader_get(&fileReader, &compressedSize , 4);
	compressedSize = convertDWFromLE(compressedSize);
	streamReader_get(&fileReader, &mode , 2);
	mode = convertWFromLE(mode);

	if (mode <= 0) {
		streamReader_get(&fileReader, ptr, dataSize);
	} else
		if (mode == 1) {
			unsigned char* compressedDataPtr;

//    compressedDataPtr = ptr + dataSize - compressedSize + 500;
			compressedDataPtr = malloc(compressedSize + 500);

			streamReader_get(&fileReader, compressedDataPtr, compressedSize);
			HQR_Expand(dataSize, (unsigned char*)ptr, compressedDataPtr);

			free(compressedDataPtr);
		}

	streamReader_close(&fileReader);

	return (dataSize);
}

int HQR_GetNumEntry(char* fileName) {
#ifndef DREAMCAST
	FILE *resourceFile = NULL;
#else
	//DC
	GDFS resourceFile = 0;
#endif
//   resourceFile = HQR_File_OpenRead(fileName);
	int headerSize;

	if (!resourceFile)
		return (0);

	HQR_File_Read(resourceFile, (char *) &headerSize, 4);

	return(headerSize / 4);
}

unsigned char *LoadMalloc_HQR(char *fileName, short int imageNumber) {
	unsigned char* ptr;
	HQRM_Load(fileName, imageNumber, &ptr);

	return (ptr);
}

subHqr *findSubHqr(int arg_0, int arg_4, subHqr * arg_8) {
	subHqr *temp;
	int i;

	if (arg_4 == 0)
		return (0);

	temp = arg_8;

	for (i = 0; i < arg_4; i++) {
		if (temp->index == arg_0)
			return (temp);

		temp++;
	}

	return (0);

}

unsigned char *HQR_Get(hqr_entry * hqrPtr, short int arg_4) {
	unsigned int headerSize;
	unsigned int offToData;
	unsigned int dataSize;
	unsigned int compressedSize;
	short int mode;

	short int var_4;
	int ltime;
	int temp2;
	unsigned char *ptr;

	int i;

	int dataSize2;
	subHqr *hqrdata;
	subHqr *hqrdataPtr;

	if (arg_4 < 0)
		return (0);

	if (hqrPtr->preloadedResource) {
		return hqrPtr->ptrArray[arg_4];
	}

	hqrdata = (subHqr *)((unsigned char *) hqrPtr + sizeof(hqr_entry));

	hqrdataPtr = findSubHqr(arg_4, hqrPtr->numCurrentlyUsedEntries, hqrdata);

	if (hqrdataPtr) {
		hqrdataPtr->lastAccessedTime = lba_time;
		HQR_Flag = 0;
		return (hqrdataPtr->offFromPtr + hqrPtr->ptr);
	}
	// printf ("Need to load: %s-%d\n", hqrPtr->fileName, arg_4);

	Size_HQR(hqrPtr->fileName, arg_4);

	if (!streamReader_open(&fileReader, hqrPtr->fileName, 1))
		return 0;

	streamReader_get(&fileReader, &headerSize, 4);
	headerSize = convertDWFromLE(headerSize);

	if (arg_4 >= headerSize / 4) {
		streamReader_close(&fileReader);
		return 0;
	}

	streamReader_seek(&fileReader, arg_4 * 4);
	streamReader_get(&fileReader, &offToData, 4);
	offToData = convertDWFromLE(offToData);

	streamReader_seek(&fileReader, offToData);
	streamReader_get(&fileReader, &dataSize, 4) ;
	dataSize = convertDWFromLE(dataSize);
	streamReader_get(&fileReader, &compressedSize, 4) ;
	compressedSize = convertDWFromLE(compressedSize);
	streamReader_get(&fileReader, &mode, 2) ;
	mode = convertWFromLE(mode);

	dataSize2 = dataSize;

// ici, test sur la taille de dataSize

	ltime = lba_time;

	while (dataSize2 >= hqrPtr->remainingSize || hqrPtr->numCurrentlyUsedEntries >= hqrPtr->numEntriesMax) { // pour retirer les elements les plus vieux jusqu'a ce qu'on ai de la place
		var_4 = 0;
		temp2 = 0;

		for (i = 0; i < hqrPtr->numCurrentlyUsedEntries; i++) {
			if (temp2 <= ltime - hqrdata[i].lastAccessedTime) {
				temp2 = ltime - hqrdata[var_4].lastAccessedTime;
				var_4 = i;
			}
		}

		HQR_RemoveEntryFromHQR(hqrPtr, var_4);

	}

	ptr = hqrPtr->ptr + hqrPtr->size1 - hqrPtr->remainingSize;

	if (mode <= 0) {  // uncompressed
		streamReader_get(&fileReader, ptr, dataSize);
	} else
		if (mode == 1) { // compressed
			streamReader_get(&fileReader, ptr + dataSize - compressedSize + 500, compressedSize);
			HQR_Expand(dataSize, ptr, (ptr + dataSize - compressedSize + 500));
		}

	streamReader_close(&fileReader);

	hqrdataPtr = &hqrdata[hqrPtr->numCurrentlyUsedEntries];

	hqrdataPtr->index = arg_4;
	HQR_Flag = 1;
	hqrdataPtr->lastAccessedTime = lba_time;
	hqrdataPtr->offFromPtr = hqrPtr->size1 - hqrPtr->remainingSize;
	hqrdataPtr->size = dataSize2;

	hqrPtr->numCurrentlyUsedEntries++;
	hqrPtr->remainingSize -= dataSize2;

	return (ptr);
}

unsigned char *HQR_GetCopy(hqr_entry * hqrPtr, short int arg_4) {
	unsigned char* newPtr;

	if (hqrPtr->preloadedResource) {
		newPtr = Malloc(hqrPtr->sizeArray[arg_4]);
		memcpy(newPtr, hqrPtr->ptrArray[arg_4], hqrPtr->sizeArray[arg_4]);
		return newPtr;
	}

	return NULL;
}

void HQR_Reset_Ressource(hqr_entry * ptr) {
	ptr->remainingSize = ptr->size1;
	ptr->numCurrentlyUsedEntries = 0;
}

int HQR_RemoveEntryFromHQR(hqr_entry * hqrPtr, int var) {
	subHqr *subPtr;

	int lvar;

	unsigned char *source;
	unsigned char *dest;
	int size;
	int retVal;

	todo("implement HQR_RemoveEntryFromHqr");
	HQR_Reset_Ressource(hqrPtr);
	return(var);

	lvar = var;

	subPtr = (subHqr *)(hqrPtr + sizeof(hqr_entry) + var * sizeof(subHqr));

	retVal = subPtr->size;

	if (hqrPtr->numCurrentlyUsedEntries - 1 > lvar) {
		source = hqrPtr->ptr + subPtr->offFromPtr;

		dest = source + retVal;
		size = hqrPtr->ptr + hqrPtr->size1 - dest;

		memmove(source, dest, size);

		printf("HQR_RemoveEntryFromHQR\n");
		exit(1);
		// implementer la suite
	}

	hqrPtr->numCurrentlyUsedEntries--;

	hqrPtr->remainingSize = retVal;

	return (retVal);
}

int HQRM_Load(char *fileName, short int arg_4, unsigned char ** ptr) { // recheck
	unsigned int headerSize;
	unsigned int offToData;
	unsigned int dataSize;
	unsigned int compressedSize;
	unsigned short int mode;
	unsigned char *temp;

	if (!streamReader_open(&fileReader, fileName, 1))
		return(-1);

	streamReader_get(&fileReader, &headerSize, 4);
	headerSize = convertDWFromLE(headerSize);

	if (arg_4 >= headerSize / 4) {
		streamReader_close(&fileReader);
		return (-1);
	}

	streamReader_seek(&fileReader, arg_4 * 4);
	streamReader_get(&fileReader, &offToData, 4);
	offToData = convertDWFromLE(offToData);

	streamReader_seek(&fileReader, offToData);
	streamReader_get(&fileReader, &dataSize, 4);
	dataSize = convertDWFromLE(dataSize);
	streamReader_get(&fileReader, &compressedSize, 4);
	compressedSize = convertDWFromLE(compressedSize);
	streamReader_get(&fileReader, &mode , 2);
	mode = convertWFromLE(mode);

	(*ptr) = Malloc(dataSize);

	if (!(*ptr)) {
		streamReader_close(&fileReader);
		return(-1);
	}

	if (mode == 0) {
		streamReader_get(&fileReader, (*ptr), dataSize);
	} else
		if (mode == 1) {
			temp = Malloc(compressedSize);

			streamReader_get(&fileReader, temp, compressedSize);
			HQR_Expand(dataSize, *ptr, temp);
			MemFree(temp);
		}

	streamReader_close(&fileReader);

	return (dataSize);
}

int Size_HQR(char *fileName, int index) {
	unsigned int headerSize;
	unsigned int dataSize;
	unsigned int offToData;

	if (!streamReader_open(&fileReader, fileName, 1)) {
		return 0;
	}

	streamReader_get(&fileReader, &headerSize, 4);
	headerSize = convertDWFromLE(headerSize);

	if (index >= headerSize / 4) {
		streamReader_close(&fileReader);
		return 0;
	}

	streamReader_seek(&fileReader, index * 4);
	streamReader_get(&fileReader, &offToData, 4);
	offToData = convertDWFromLE(offToData);
	streamReader_seek(&fileReader, offToData);
	streamReader_get(&fileReader, &dataSize, 4);
	dataSize = convertDWFromLE(dataSize);
	streamReader_close(&fileReader);

	return (dataSize);
}
