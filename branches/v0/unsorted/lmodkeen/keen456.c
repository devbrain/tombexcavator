/* KEEN456.C - Keen 4, 5, and 6 import and export routines.
**
** Original code Copyright (c)2002-2004 Andrew Durdin. (andy@durdin.net)
** Linux port code by Ignacio R. Morelle "Shadow Master". (shadowm2006@gmail.com)
** March 11th 2007
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
** Permission is granted to anyone to use this software for any purpose, including
** commercial applications, and to alter it and redistribute it freely, subject
** to the following restrictions:
**    1. The origin of this software must not be misrepresented; you must not
**       claim that you wrote the original software. If you use this software in
**       a product, an acknowledgment in the product documentation would be
**       appreciated but is not required.
**    2. Altered source versions must be plainly marked as such, and must not be
**       misrepresented as being the original software.
**    3. This notice may not be removed or altered from any source distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// SM: Used by this port:
#include <memory.h>
#include "linconio.h"

#include "utils.h"
#include "huff.h"
#include "bmp16.h"
#include "switches.h"

#pragma pack(1)

typedef struct
{
	unsigned short Width;
	unsigned short Height;
} BitmapHeadStruct;

typedef struct
{
	unsigned short Width;
	unsigned short Height;
	signed short OrgX;
	signed short OrgY;
	signed short Rx1, Ry1;
	signed short Rx2, Ry2;
	unsigned short Shifts;
} SpriteHeadStruct;

typedef struct
{
	unsigned short Height;
	unsigned short Offset[256];
	unsigned char Width[256];
} FontHeadStruct;

#pragma pack()



#define NUM_VERSIONS	9


typedef struct
{
	unsigned long ExeImageSize;
	unsigned long OffEgaHead;
	unsigned long OffEgaDict;
	unsigned long NumChunks;
	unsigned int NumFonts, IndexFonts;
	unsigned int NumBitmaps, IndexBitmaps;
	unsigned int NumMaskedBitmaps, IndexMaskedBitmaps;
	unsigned int NumSprites, IndexSprites;
	unsigned int Num8Tiles, Index8Tiles;
	unsigned int Num8MaskedTiles, Index8MaskedTiles;
	unsigned int Num16Tiles, Index16Tiles;
	unsigned int Num16MaskedTiles, Index16MaskedTiles;
	unsigned int NumMisc, IndexMisc;	/* Includes ANSI images */
	unsigned int NumTexts, IndexTexts;
	unsigned int NumDemos, IndexDemos;
} EpisodeInfoStruct;


static EpisodeInfoStruct EpisodeInfo[] = {
	{	/* Episode 4 */
		0x3D740,	/* ExeImageSize */
		0x21080,	/* OffEgaHead */
		0x358F6,	/* OffEgaDict */
		4751,		/* NumChunks */
		3, 3,		/* NumFonts, IndexFonts */
		115, 6,		/* NumBitmaps, IndexBitmaps */
		3, 121,		/* NumMaskedBitmaps, IndexMasked Bitmaps */
		397, 124,	/* NumSprites, IndexSprites */
		104, 521,	/* Num8Tiles, Index8Tiles */
		20, 522,	/* Num8MaskedTiles, Index8MaskedTiles */
		1296, 523,	/* Num16Tiles, Index16Tiles */
		2916, 1819,	/* Num16MaskedTiles, Index16MaskedTiles */
		4, 4735,	/* NumMisc, IndexMisc */
		7, 4739,	/* NumTexts, IndexTexts */
		5, 4746		/* NumDemos, IndexDemos */
	},
	{	/* Episode 5 */
		0x3E370,	/* ExeImageSize */
		0x21FC0,	/* OffEgaHead */
		0x362C4,	/* OffEgaDict */
		4931,		/* NumChunks */
		3, 3,		/* NumFonts, IndexFonts */
		93, 6,		/* NumBitmaps, IndexBitmaps */
		3, 99,		/* NumMaskedBitmaps, IndexMasked Bitmaps */
		346, 102,	/* NumSprites, IndexSprites */
		104, 448,	/* Num8Tiles, Index8Tiles */
		20, 449,	/* Num8MaskedTiles, Index8MaskedTiles */
		1512, 450,	/* Num16Tiles, Index16Tiles */
		2952, 1962,	/* Num16MaskedTiles, Index16MaskedTiles */
		5, 4921,	/* NumMisc, IndexMisc */
		7, 4914,	/* NumTexts, IndexTexts */
		5, 4926		/* NumDemos, IndexDemos */
	},
	{	/* Episode 6 */
		0x3F950,	/* ExeImageSize */
		0x20F50,	/* OffEgaHead */
		0x372EE,	/* OffEgaDict */
		5560,		/* NumChunks */
		3, 3,		/* NumFonts, IndexFonts */
		37, 6,		/* NumBitmaps, IndexBitmaps */
		3, 43,		/* NumMaskedBitmaps, IndexMasked Bitmaps */
		390, 46,	/* NumSprites, IndexSprites */
		104, 436,	/* Num8Tiles, Index8Tiles */
		12, 437,	/* Num8MaskedTiles, Index8MaskedTiles */
		2376, 438,	/* Num16Tiles, Index16Tiles */
		2736, 2814,	/* Num16MaskedTiles, Index16MaskedTiles */
		4, 5551,	/* NumMisc, IndexMisc */
		1, 5550,	/* NumTexts, IndexTexts */
		5, 5555		/* NumDemos, IndexDemos */
	}
};


typedef struct {
	unsigned long len;
	unsigned char *data;
} ChunkStruct;

static int ExportInitialised = 0;
static int ImportInitialised = 0;
static ChunkStruct *EgaGraph = NULL;
static BitmapHeadStruct *BmpHead = NULL;
static BitmapHeadStruct *BmpMaskedHead = NULL;
static SpriteHeadStruct *SprHead = NULL;
static SwitchStruct *Switches = NULL;

/************************************************************************************************************/
/** KEEN 4, 5, 6 EXPORTING ROUTINES *************************************************************************/
/************************************************************************************************************/

void k456_export_begin(SwitchStruct *switches)
{	char filename[PATH_MAX];
	FILE *exefile = NULL;
	FILE *graphfile = NULL;
	FILE *headfile = NULL;
	unsigned long exeimglen, exeheaderlen;
	unsigned long offset;
	unsigned char *CompEgaGraphData;
	unsigned long *EgaHead = NULL;
	unsigned long egagraphlen, inlen, outlen;
	int i, j;
	int ep;

	/* Never allow the export start to occur more than once */
	if(ExportInitialised)
		quit("Tried to initialise Keen 1 files a second time!");
	
	/* Save the switches */
	Switches = switches;
	ep = Switches->Episode - 4;


	/* Open the EXE */
	if(Switches->Episode == 6)
		sprintf(filename, "%s/KEEN%c.EXE", Switches->InputPath, '0' + Switches->Episode);
	else
		sprintf(filename, "%s/KEEN%cE.EXE", Switches->InputPath, '0' + Switches->Episode);
	exefile = fopen(filename, "rb");
	if(!exefile)
		quit("Can't open %s for reading!", filename);
		
	/* Check that it's the right version (and it's unpacked) */
	if(!get_exe_image_size(exefile, &exeimglen, &exeheaderlen))
		quit("%s is not a valid exe file!", filename);
	
	if(exeimglen != EpisodeInfo[ep].ExeImageSize)
		quit("%s must be version 1.4, and must be uncompressed!", filename);

	/* Read the EGADICT */
	huff_read_dictionary(exefile, exeheaderlen + EpisodeInfo[ep].OffEgaDict);
	
	/* Get the EGAHEAD Data */
	EgaHead = malloc(EpisodeInfo[ep].NumChunks * sizeof(unsigned long));
	if(!EgaHead)
		quit("Not enough memory to read EGAHEAD!");

	/* Get the EGAHEAD data from the EGAHEAD file, or from the exe if that doesn't exist */
	sprintf(filename, "%s/EGAHEAD.CK%c", Switches->InputPath, '0' + Switches->Episode);
	headfile = fopen(filename, "rb");
	if(!headfile)
	{
		printf("Cannot find %s, Extracting EGAHEAD data from the exe.\n", filename);
		/* Get EGAHEAD from the EXE */
		headfile = exefile;
		fseek(headfile, exeheaderlen + EpisodeInfo[ep].OffEgaHead, SEEK_SET);
	}
	/* Read the EGAHEAD */
	for(i = 0; i < EpisodeInfo[ep].NumChunks; i++)
	{
		fread(&offset, 3, 1, headfile);
		offset &= 0x00FFFFFF;
		EgaHead[i] = offset;
	}
	
	/* Close the files */
	if(headfile != exefile)
		fclose(headfile);
	fclose(exefile);
	
	/* Now read the EGAGRAPH */
	sprintf(filename, "%s/EGAGRAPH.CK%c", Switches->InputPath, '0' + Switches->Episode);
	graphfile = fopen(filename, "rb");
	if(!graphfile)
		quit("Can't open %s!", filename);
	fseek(graphfile, 1, SEEK_END);
	egagraphlen = ftell(graphfile) - 1;
	fseek(graphfile, 0, SEEK_SET);
	
	CompEgaGraphData = (unsigned char *)malloc(egagraphlen);
	if(!CompEgaGraphData)
		quit("Not enough memory to read EGAGRAPH!");
		
	fread(CompEgaGraphData, egagraphlen, 1, graphfile);
	
	fclose(graphfile);
	
	/* Now decompress the EGAGRAPH */
	printf("Decompressing: ");
	EgaGraph = (ChunkStruct *)malloc(EpisodeInfo[ep].NumChunks * sizeof(ChunkStruct));
	if(!EgaGraph)
		quit("Not enough memory to decompress EGAGRAPH!");
	for(i = 0; i < EpisodeInfo[ep].NumChunks; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (int)((i * 100) / EpisodeInfo[ep].NumChunks));

		offset = EgaHead[i];

		/* Make sure the chunk is valid */
		if(offset != 0xFFFFFF)
		{
			/* Get the expanded length of the chunk */
			if(i >= EpisodeInfo[ep].Index8Tiles && i < EpisodeInfo[ep].Index16MaskedTiles + EpisodeInfo[ep].Num16MaskedTiles)
			{
				/* Expanded sizes of 8, 16,and 32 tiles are implicit */
				if(i >= EpisodeInfo[ep].Index16MaskedTiles) /* 16x16 tiles are one/chunk */
					outlen = 2 * 16 * 5;
				else if(i >= EpisodeInfo[ep].Index16Tiles)
					outlen = 2 * 16 * 4;
				else if(i >= EpisodeInfo[ep].Index8MaskedTiles)	/* 8x8 tiles are all in one chunk! */
					outlen = EpisodeInfo[ep].Num8MaskedTiles * 8 * 5;
				else if(i >= EpisodeInfo[ep].Index8Tiles)
					outlen = EpisodeInfo[ep].Num8Tiles * 8 * 4;
			}
			else
			{
				memcpy(&outlen, CompEgaGraphData + offset, sizeof(unsigned long));
				offset += sizeof(unsigned long);
			}


			/* Allocate memory and decompress the chunk */
			EgaGraph[i].len = outlen;
			EgaGraph[i].data = (unsigned char *)malloc(outlen);
			if(!EgaGraph[i].data)
				quit("Not enough memory to decompress EGAGRAPH chunk %d (size %X at %X)!", i, outlen, offset);

			inlen = 0;
			/* Find out the input length */			
			for(j = i + 1; j < EpisodeInfo[ep].NumChunks; j++)
			{
				if(EgaHead[j] != 0xFFFFFF)
				{
					inlen = EgaHead[j] - offset;
					break;
				}
			}
			if(j == EpisodeInfo[ep].NumChunks)
				inlen = egagraphlen - offset;
			
			huff_expand(CompEgaGraphData + offset, EgaGraph[i].data, inlen, outlen);
		}
		else
		{
			EgaGraph[i].len = 0;
			EgaGraph[i].data = NULL;
		}

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
	
	/* Set up pointers to bitmap and sprite tables */
	BmpHead = (BitmapHeadStruct *)EgaGraph[0].data;
	BmpMaskedHead = (BitmapHeadStruct *)EgaGraph[1].data;
	SprHead = (SpriteHeadStruct *)EgaGraph[2].data;

	free(EgaHead);
	free(CompEgaGraphData);

	ExportInitialised = 1;
}

void k456_export_end()
{
	int i;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Tried to end export before beginning!");
		
	for(i = 0; i < EpisodeInfo[ep].NumChunks; i++)
		if(EgaGraph[i].data)
			free(EgaGraph[i].data);

	free(EgaGraph);
	
	ExportInitialised = 0;
}

void k456_export_bitmaps()
{
	BITMAP16 *bmp, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export bitmaps before initialisation!");

	/* Export all the bitmaps */
	printf("Exporting bitmaps: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumBitmaps; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumBitmaps);
		
		if(EgaGraph[EpisodeInfo[ep].IndexBitmaps + i].data)
		{
			/* Decode the bitmap data */
			for(p = 0; p < 4; p++)
			{
				/* Create a 1bpp bitmap for each plane */
				planes[p] = bmp_create(BmpHead[i].Width * 8, BmpHead[i].Height, 1);
				if(!planes[p])
					quit("Not enough memory to create bitmaps!");
				
				/* Decode the lines of the bitmap data */
				pointer = EgaGraph[EpisodeInfo[ep].IndexBitmaps + i].data + p * BmpHead[i].Width * BmpHead[i].Height;
				for(y = 0; y < BmpHead[i].Height; y++)
					memcpy(planes[p]->lines[y], pointer + y * BmpHead[i].Width, BmpHead[i].Width);
			}			
	
			/* Create the bitmap file */
			sprintf(filename, "%s/%cBMP%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
			bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
			if(!planes[p])
				quit("Not enough memory to create bitmap!");
			if(!bmp_save(bmp, filename, Switches->Backup))
				quit("Can't open bitmap file %s!", filename);
			
			/* Free the memory used */
			for(p = 0; p < 4; p++)
			{
				bmp_free(planes[p]);
			}
			bmp_free(bmp);
		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}


void k456_export_masked_bitmaps()
{
	BITMAP16 *bmp, *mbmp, *planes[5];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export masked bitmaps before initialisation!");

	/* Export all the bitmaps */
	printf("Exporting masked bitmaps: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumMaskedBitmaps; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumMaskedBitmaps);

		if(EgaGraph[EpisodeInfo[ep].IndexMaskedBitmaps + i].data)
		{
			mbmp = bmp_create(BmpMaskedHead[i].Width * 8 * 2, BmpMaskedHead[i].Height, 4);
			
			/* Decode the transparency mask */
			planes[4] = bmp_create(BmpMaskedHead[i].Width * 8, BmpMaskedHead[i].Height, 1);
			pointer = EgaGraph[EpisodeInfo[ep].IndexMaskedBitmaps + i].data;
			for(y = 0; y < BmpMaskedHead[i].Height; y++)
				memcpy(planes[4]->lines[y], pointer + y * BmpMaskedHead[i].Width, BmpMaskedHead[i].Width);
			bmp_blit(planes[4], 0, 0, mbmp, BmpMaskedHead[i].Width * 8, 0, BmpMaskedHead[i].Width * 8, BmpMaskedHead[i].Height);
			
			/* Decode the bitmap data */
			for(p = 0; p < 4; p++)
			{
				/* Create a 1bpp bitmap for each plane */
				planes[p] = bmp_create(BmpMaskedHead[i].Width * 8, BmpMaskedHead[i].Height, 1);
				
				/* Decode the lines of the bitmap data */
				pointer = EgaGraph[EpisodeInfo[ep].IndexMaskedBitmaps + i].data + (p + 1) * BmpMaskedHead[i].Width * BmpMaskedHead[i].Height;
				for(y = 0; y < BmpMaskedHead[i].Height; y++)
					memcpy(planes[p]->lines[y], pointer + y * BmpMaskedHead[i].Width, BmpMaskedHead[i].Width);
			}			
	
			/* Create the bitmap file */
			sprintf(filename, "%s/%cMBM%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
			bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
			bmp_blit(bmp, 0, 0, mbmp, 0, 0, BmpMaskedHead[i].Width * 8, BmpMaskedHead[i].Height);
			if(!bmp_save(mbmp, filename, Switches->Backup))
				quit("Can't open bitmap file %s!", filename);
			
			/* Free the memory used */
			for(p = 0; p < 5; p++)
				bmp_free(planes[p]);
			bmp_free(bmp);
			bmp_free(mbmp);
		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_export_tiles()
{
	BITMAP16 *tiles, *bmp, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export tiles before initialisation!");

	/* Export all the tiles into one bitmap*/
	printf("Exporting tiles: ");

	tiles = bmp_create(16 * 18, 16 * ((EpisodeInfo[ep].Num16Tiles + 17) / 18), 4);

	/* Create a 1bpp bitmap for each plane */
	for(p = 0; p < 4; p++)
		planes[p] = bmp_create(16, 16, 1);
   
	for(i = 0; i < EpisodeInfo[ep].Num16Tiles; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].Num16Tiles);

		if(EgaGraph[EpisodeInfo[ep].Index16Tiles + i].data)
		{
			/* Decode the image data */
			for(p = 0; p < 4; p++)
			{
				/* Decode the lines of the bitmap data */
				pointer = EgaGraph[EpisodeInfo[ep].Index16Tiles + i].data + p * 2 * 16;
				for(y = 0; y < 16; y++)
					memcpy(planes[p]->lines[y], pointer + y * 2, 2);
			}			
	
			bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
			bmp_blit(bmp, 0, 0, tiles, 16 * (i % 18), 16 * (i / 18), 16, 16);
			bmp_free(bmp);
		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");

	/* Create the bitmap file */
	sprintf(filename, "%s/%cTIL0000.bmp", Switches->OutputPath, '0' + Switches->Episode);
	if(!bmp_save(tiles, filename, Switches->Backup))
		quit("Can't open bitmap file %s!", filename);

	/* Free the memory used */
	for(p = 0; p < 4; p++)
		bmp_free(planes[p]);
	bmp_free(tiles);
}

void k456_export_masked_tiles()
{
	BITMAP16 *tiles, *bmp, *planes[5];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export masked tiles before initialisation!");

	/* Export all the masked tiles into one bitmap*/
	printf("Exporting masked tiles: ");

	tiles = bmp_create(16 * 18 * 2, 16 * ((EpisodeInfo[ep].Num16MaskedTiles + 17) / 18), 4);

	/* Create a 1bpp bitmap for each plane */
	for(p = 0; p < 5; p++)
		planes[p] = bmp_create(16, 16, 1);
   
	for(i = 0; i < EpisodeInfo[ep].Num16MaskedTiles; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].Num16MaskedTiles);
		
		if(EgaGraph[EpisodeInfo[ep].Index16MaskedTiles + i].data)
		{
			/* Decode the mask data */
			pointer = EgaGraph[EpisodeInfo[ep].Index16MaskedTiles + i].data;
			for(y = 0; y < 16; y++)
			{
				memcpy(planes[4]->lines[y], pointer + y * 2, 2);
			}
			bmp_blit(planes[4], 0, 0, tiles, 16 * 18 + 16 * (i % 18), 16 * (i / 18), 16, 16);
			
			/* Decode the image data */
			for(p = 0; p < 4; p++)
			{
				/* Decode the lines of the bitmap data */
				pointer = EgaGraph[EpisodeInfo[ep].Index16MaskedTiles + i].data + (p + 1) * 2 * 16;
				for(y = 0; y < 16; y++)
					memcpy(planes[p]->lines[y], pointer + y * 2, 2);
			}			
	
			bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
			bmp_blit(bmp, 0, 0, tiles, 16 * (i % 18), 16 * (i / 18), 16, 16);
			bmp_free(bmp);
		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");

	/* Create the bitmap file */
	sprintf(filename, "%s/%cTIL0001.bmp", Switches->OutputPath, '0' + Switches->Episode);
	if(!bmp_save(tiles, filename, Switches->Backup))
		quit("Can't open bitmap file %s!", filename);

	/* Free the memory used */
	for(p = 0; p < 5; p++)
		bmp_free(planes[p]);
	bmp_free(tiles);
}

void k456_export_8_tiles()
{
	BITMAP16 *tiles, *bmp, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export 8x8 tiles before initialisation!");

	/* Export all the 8x8 tiles into one bitmap*/
	printf("Exporting 8x8 tiles: ");

	tiles = bmp_create(8, 8 * EpisodeInfo[ep].Num8Tiles, 4);

	/* Create a 1bpp bitmap for each plane */
	for(p = 0; p < 4; p++)
		planes[p] = bmp_create(8, 8, 1);
   
	if(EgaGraph[EpisodeInfo[ep].Index8Tiles].data)
	{
		for(i = 0; i < EpisodeInfo[ep].Num8Tiles; i++)
		{
			/* Show that something is happening */
			gotoxy(30, wherey());
			printf("%3d%%", (i * 100) / EpisodeInfo[ep].Num8Tiles);
		
			/* Decode the image data */
			for(p = 0; p < 4; p++)
			{
				/* Decode the lines of the bitmap data */
				pointer = EgaGraph[EpisodeInfo[ep].Index8Tiles].data + (i * 4 * 8) + p * 8;
				for(y = 0; y < 8; y++)
					memcpy(planes[p]->lines[y], pointer + y, 1);
			}			
	
			bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
			bmp_blit(bmp, 0, 0, tiles, 0, 8 * i, 8, 8);
			bmp_free(bmp);
			//printf("\x8\x8\x8\x8");
		}
		gotoxy(30, wherey());
		printf("100%%\n");
	}

	/* Create the bitmap file */
	sprintf(filename, "%s/%cTIL0002.bmp", Switches->OutputPath, '0' + Switches->Episode);
	if(!bmp_save(tiles, filename, Switches->Backup))
		quit("Can't open bitmap file %s!", filename);

	/* Free the memory used */
	for(p = 0; p < 4; p++)
		bmp_free(planes[p]);
	bmp_free(tiles);
	}

void k456_export_8_masked_tiles()
{
	BITMAP16 *tiles, *bmp, *planes[5];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export 8x8 masked tiles before initialisation!");

	/* Export all the 8x8 masked tiles into one bitmap*/
	printf("Exporting 8x8 masked tiles: ");

	tiles = bmp_create(8 * 2, 8 * EpisodeInfo[ep].Num8MaskedTiles, 4);

	/* Create a 1bpp bitmap for each plane */
	for(p = 0; p < 5; p++)
		planes[p] = bmp_create(8, 8, 1);
   
	if(EgaGraph[EpisodeInfo[ep].Index8MaskedTiles].data)
	{
		for(i = 0; i < EpisodeInfo[ep].Num8MaskedTiles; i++)
		{
			/* Show that something is happening */
			gotoxy(30, wherey());
			printf("%3d%%", (i * 100) / EpisodeInfo[ep].Num8MaskedTiles);

			/* Decode the mask data */
			pointer = EgaGraph[EpisodeInfo[ep].Index8MaskedTiles].data + (i * 5 * 8);
			for(y = 0; y < 8; y++)
				memcpy(planes[4]->lines[y], pointer + y, 1);
			bmp_blit(planes[4], 0, 0, tiles, 8, 8 * i, 8, 8);
			
			/* Decode the image data */
			for(p = 0; p < 4; p++)
			{
				/* Decode the lines of the bitmap data */
				pointer = EgaGraph[EpisodeInfo[ep].Index8MaskedTiles].data + (i * 5 * 8) + (p + 1) * 8;
				for(y = 0; y < 8; y++)
					memcpy(planes[p]->lines[y], pointer + y, 1);
			}			
	
			bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
			bmp_blit(bmp, 0, 0, tiles, 0, 8 * i, 8, 8);
			bmp_free(bmp);
			
			//printf("\x8\x8\x8\x8");
		}
		gotoxy(30, wherey());
		printf("100%%\n");
	}
	
	/* Create the bitmap file */
	sprintf(filename, "%s/%cTIL0003.bmp", Switches->OutputPath, '0' + Switches->Episode);
	if(!bmp_save(tiles, filename, Switches->Backup))
		quit("Can't open bitmap file %s!", filename);

	/* Free the memory used */
	for(p = 0; p < 5; p++)
		bmp_free(planes[p]);
	bmp_free(tiles);
}

void k456_export_sprites()
{
	BITMAP16 *bmp, *spr, *planes[5];
	FILE *f;
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export sprites before initialisation!");

	/* Export all the sprites */
	printf("Exporting sprites: ");

	/* Open a text file for the clipping and origin info */
	sprintf(filename, "%s/%cSPRITES.txt", Switches->OutputPath, '0' + Switches->Episode);
	f = openfile(filename, "w", Switches->Backup);
   
	for(i = 0; i < EpisodeInfo[ep].NumSprites; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumSprites);
		
		if(EgaGraph[EpisodeInfo[ep].IndexSprites + i].data)
		{
			spr = bmp_create(SprHead[i].Width * 8 * 3, SprHead[i].Height, 4);
			
			/* Decode the transparency mask */
			planes[4] = bmp_create(SprHead[i].Width * 8, SprHead[i].Height, 1);
			pointer = EgaGraph[EpisodeInfo[ep].IndexSprites + i].data;
			for(y = 0; y < SprHead[i].Height; y++)
				memcpy(planes[4]->lines[y], pointer + y * SprHead[i].Width, SprHead[i].Width);
			bmp_blit(planes[4], 0, 0, spr, SprHead[i].Width * 8, 0, SprHead[i].Width * 8, SprHead[i].Height);
			
			/* Decode the sprite image data */
			for(p = 0; p < 4; p++)
			{
				/* Create a 1bpp bitmap for each plane */
				planes[p] = bmp_create(SprHead[i].Width * 8, SprHead[i].Height, 1);
				
				/* Decode the lines of the bitmap data */
				pointer = EgaGraph[EpisodeInfo[ep].IndexSprites + i].data + (p + 1) * SprHead[i].Width * SprHead[i].Height;
				for(y = 0; y < SprHead[i].Height; y++)
					memcpy(planes[p]->lines[y], pointer + y * SprHead[i].Width, SprHead[i].Width);
			}			
	
			/* Draw the collision rectangle */
			bmp_rect(spr, 2 * SprHead[i].Width * 8, 0, spr->width - 1, spr->height - 1, 8);
			bmp_rect(spr, 2 * SprHead[i].Width * 8 + ((SprHead[i].Rx1 - SprHead[i].OrgX) >> 4), ((SprHead[i].Ry1 - SprHead[i].OrgY) >> 4),
					2 * SprHead[i].Width * 8 + ((SprHead[i].Rx2 - SprHead[i].OrgX) >> 4), ((SprHead[i].Ry2 - SprHead[i].OrgY) >> 4), 12);
	
			/* Output the collision rectangle and origin information */
			fprintf(f, "%d: [%d, %d, %d, %d], [%d, %d], %d\n", i, (SprHead[i].Rx1 - SprHead[i].OrgX) >> 4,
				(SprHead[i].Ry1 - SprHead[i].OrgY) >> 4, (SprHead[i].Rx2 - SprHead[i].OrgX) >> 4,
				(SprHead[i].Ry2 - SprHead[i].OrgY) >> 4, SprHead[i].OrgX >> 4, SprHead[i].OrgY >> 4,
				SprHead[i].Shifts);
			
			/* Create the bitmap file */
			sprintf(filename, "%s/%cSPR%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
			bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
			bmp_blit(bmp, 0, 0, spr, 0, 0, SprHead[i].Width * 8, SprHead[i].Height);
			if(!bmp_save(spr, filename, Switches->Backup))
				quit("Can't open bitmap file %s!", filename);
			
			/* Free the memory used */
			for(p = 0; p < 5; p++)
				bmp_free(planes[p]);
			bmp_free(bmp);
			bmp_free(spr);

		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
	fclose(f);
}


void k456_export_texts()
{
	char filename[PATH_MAX];
	FILE *f;
	int i;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export texts before initialisation!");

	/* Export all the texts */
	printf("Exporting texts: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumTexts; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumTexts);
		
		if(EgaGraph[EpisodeInfo[ep].IndexTexts + i].data)
		{
			/* Create the text file */
			sprintf(filename, "%s/%cTXT%04d.txt", Switches->OutputPath, '0' + Switches->Episode, i);
			f = openfile(filename, "wb", Switches->Backup);
			if(!f)
				quit("Can't open text file %s!", filename);
			fwrite(EgaGraph[EpisodeInfo[ep].IndexTexts + i].data, EgaGraph[EpisodeInfo[ep].IndexTexts + i].len, 1, f);
			fclose(f);
		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_export_misc()
{
	char filename[PATH_MAX];
	FILE *f;
	int i;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export misc bits before initialisation!");

	/* Export all the misc bits */
	printf("Exporting misc bits: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumMisc; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumMisc);
		
		if(EgaGraph[EpisodeInfo[ep].IndexMisc + i].data)
		{
			/* Create the text file */
			sprintf(filename, "%s/%cMSC%04d.bin", Switches->OutputPath, '0' + Switches->Episode, i);
			f = openfile(filename, "wb", Switches->Backup);
			if(!f)
				quit("Can't open file %s!", filename);
			fwrite(EgaGraph[EpisodeInfo[ep].IndexMisc + i].data, EgaGraph[EpisodeInfo[ep].IndexMisc + i].len, 1, f);
			fclose(f);
		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_export_demos()
{
	char filename[PATH_MAX];
	FILE *f;
	int i;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export demos before initialisation!");

	/* Export all the demos */
	printf("Exporting demos: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumDemos; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumDemos);
		
		if(EgaGraph[EpisodeInfo[ep].IndexDemos + i].data)
		{
			/* Create the demo file */
			sprintf(filename, "%s/DEMO%d.CK%c", Switches->OutputPath, i, '0' + Switches->Episode);
			f = openfile(filename, "wb", Switches->Backup);
			if(!f)
				quit("Can't open file %s!", filename);
			fwrite(EgaGraph[EpisodeInfo[ep].IndexDemos + i].data, EgaGraph[EpisodeInfo[ep].IndexDemos + i].len, 1, f);
			fclose(f);
		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}


void k456_export_fonts()
{
	BITMAP16 *font, *bmp;
	FontHeadStruct *FontHead;
	char filename[PATH_MAX];
	int i, j, w, bw, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ExportInitialised)
		quit("Trying to export fonts before initialisation!");

	/* Export all the fonts into separate bitmaps*/
	printf("Exporting fonts: ");

	for(i = 0; i < EpisodeInfo[ep].NumFonts; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumFonts);
		
		if(EgaGraph[EpisodeInfo[ep].IndexFonts + i].data)
		{
			FontHead = (FontHeadStruct *)EgaGraph[EpisodeInfo[ep].IndexFonts + i].data;
			
			/* Find out the maximum character width */
			w = 0;
			for(j = 0; j < 256; j++)
				if(FontHead->Width[j] > w)
					w = FontHead->Width[j];
			
			font = bmp_create(w * 16, FontHead->Height * 16, 4);
		
			/* Create a 1bpp bitmap for the character */
			bmp = bmp_create(w, FontHead->Height, 1);
		
			/* Now decode the characters */
			pointer = EgaGraph[EpisodeInfo[ep].IndexFonts + i].data;
		   	for(j = 0; j < 256; j++)
		   	{
		   		/* Get the width of the character in bytes */
		   		bw = (FontHead->Width[j] + 7) / 8;
	
		   		/* Clear the bitmap */
	   			bmp_rect(bmp, 0, 0, bmp->width - 1, bmp->height - 1, 8);
		   		
				/* Decode the lines of the character data */
		   		if(FontHead->Width[j] > 0)
					for(y = 0; y < FontHead->Height; y++)
						memcpy(bmp->lines[y], pointer + FontHead->Offset[j] + (y * bw), bw);
	
				/* Copy the character into the grid */
				bmp_blit(bmp, 0, 0, font, (j % 16) * w, (j / 16) * FontHead->Height, w, FontHead->Height);
	
		   		/* Fill the remainder of the bitmap with Grey */
	   			bmp_rect(font, (j % 16) * w + FontHead->Width[j], (j / 16) * FontHead->Height,
	   				(j % 16) * w + w - 1, (j / 16) * FontHead->Height +  FontHead->Height - 1, 8);
			}
	
			/* Create the bitmap file */
			sprintf(filename, "%s/%cFON%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
			if(!bmp_save(font, filename, Switches->Backup))
				quit("Can't open bitmap file %s!", filename);
		
			/* Free the memory used */
			bmp_free(font);
			bmp_free(bmp);
		}
		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

/************************************************************************************************************/
/** KEEN 4, 5, 6 IMPORTING ROUTINES *************************************************************************/
/************************************************************************************************************/

void k456_import_begin(SwitchStruct *switches)
{
	char filename[PATH_MAX];
	FILE *exefile;
	unsigned long exeimglen, exeheaderlen;
	int i;
	int ep;

	/* Never allow the import start to occur more than once */
	if(ImportInitialised)
		quit("Tried to initialise Keen 4 files a second time!");
	
	/* Save the switches */
	Switches = switches;
	ep = Switches->Episode - 4;

	/* Open the EXE */
	if(Switches->Episode == 6)
		sprintf(filename, "%s/KEEN%c.EXE", Switches->InputPath, '0' + Switches->Episode);
	else
		sprintf(filename, "%s/KEEN%cE.EXE", Switches->InputPath, '0' + Switches->Episode);
	exefile = fopen(filename, "rb");
	if(!exefile)
		quit("Can't open %s for reading!", filename);
		
	/* Check that it's the right version (and it's unpacked) */
	if(!get_exe_image_size(exefile, &exeimglen, &exeheaderlen))
		quit("%s is not a valid exe file!", filename);
	
	if(exeimglen != EpisodeInfo[ep].ExeImageSize)
		quit("%s must be version 1.4, and must be uncompressed!", filename);

	/* Read the EGADICT and initialise compression */
	huff_read_dictionary(exefile, exeheaderlen + EpisodeInfo[ep].OffEgaDict);
	huff_setup_compression();


	/* Initialise the EGAGRAPH */
	EgaGraph = (ChunkStruct *)malloc(EpisodeInfo[ep].NumChunks * sizeof(ChunkStruct));
	if(!EgaGraph)
		quit("Not enough memory to initialise EGAGRAPH!");
	for(i = 0; i < EpisodeInfo[ep].NumChunks; i++)
	{
		EgaGraph[i].data = NULL;
		EgaGraph[i].len = 0;
	}

	/* Set up pointers to bitmap and sprite tables */
	BmpHead = (BitmapHeadStruct *)malloc(sizeof(BitmapHeadStruct) * EpisodeInfo[ep].NumBitmaps);
	BmpMaskedHead = (BitmapHeadStruct *)malloc(sizeof(BitmapHeadStruct) * EpisodeInfo[ep].NumMaskedBitmaps);
	SprHead = (SpriteHeadStruct *)malloc(sizeof(SpriteHeadStruct) * EpisodeInfo[ep].NumSprites);
	if(!BmpHead || !BmpMaskedHead || !SprHead)
		quit("Not enough memory to initialise structures!");

	EgaGraph[0].data = (unsigned char *)BmpHead;
	EgaGraph[0].len = sizeof(BitmapHeadStruct) * EpisodeInfo[ep].NumBitmaps;
	EgaGraph[1].data = (unsigned char *)BmpMaskedHead;
	EgaGraph[1].len = sizeof(BitmapHeadStruct) * EpisodeInfo[ep].NumMaskedBitmaps;
	EgaGraph[2].data = (unsigned char *)SprHead;
	EgaGraph[2].len = sizeof(SpriteHeadStruct) * EpisodeInfo[ep].NumSprites;

	ImportInitialised = 1;
}


void k456_import_end()
{
	char filename[PATH_MAX];
	int i;
	unsigned char *compdata;
	FILE *headfile, *graphfile, *patchfile;
	int ep = Switches->Episode - 4;
	unsigned long offset, len, ptr;
	
	if(!ImportInitialised)
		quit("Tried to end import without beginning it!");

	/* Open the EGAHEAD and EGAGRAPH files for writing */
	sprintf(filename, "%s/EGAHEAD.CK%c", Switches->InputPath, '0' + Switches->Episode);
	headfile = openfile(filename, "wb", Switches->Backup);
	if(!headfile)
		quit("Unable to open %s for writing!", filename);

	sprintf(filename, "%s/EGAGRAPH.CK%c", Switches->InputPath, '0' + Switches->Episode);
	graphfile = openfile(filename, "wb", Switches->Backup);
	if(!graphfile)
		quit("Unable to open %s for writing!", filename);


	/* Compress data and output the EGAHEAD and EGAGRAPH */
	printf("Compressing: ");
	offset = 0;
	for(i = 0; i < EpisodeInfo[ep].NumChunks; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (int)((i * 100) / EpisodeInfo[ep].NumChunks));

		if(EgaGraph[i].data && EgaGraph[i].len > 0)
		{
			/* Save the current offset */
			ptr = offset;

			/* Give some extra room for compressed data (as it's occasionally larger) */
			compdata = malloc(EgaGraph[i].len * 2);
			if(!compdata)
				quit("Not enough memory for compression buffer!");
	
			len = huff_compress(EgaGraph[i].data, compdata, EgaGraph[i].len, EgaGraph[i].len * 2);

			/* If the chunk is not a tile chunk then we need to output the length first */
			if(i < EpisodeInfo[ep].Index8Tiles || i >= EpisodeInfo[ep].Index16MaskedTiles + EpisodeInfo[ep].Num16MaskedTiles)
			{
				fwrite(&EgaGraph[i].len, sizeof(unsigned long), 1, graphfile);
				offset += sizeof(unsigned long);
			}
			fwrite(compdata, len, 1, graphfile);
			free(compdata);

			/* Calculate the next offset (taking t*/			
			offset += len;
		}
		else
		{
			ptr = 0xFFFFFF;
		}
		fwrite(&ptr, 3, 1, headfile);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");

	sprintf(filename, "%s/KEEN%c.PAT", Switches->InputPath, '0' + Switches->Episode);
	patchfile = openfile(filename, "w", Switches->Backup);
	
	fprintf(patchfile, "%%ext ck%c\n"
					  "%%version 1.4\n"
					  "# Load the modified graphics\n"
					  "%%egahead EGAHEAD.CK%c\n"
					  "%%end\n", '0' + Switches->Episode, '0' + Switches->Episode);
	
	/* Close files */
	fclose(patchfile);
	fclose(headfile);
	fclose(graphfile);

	/* Free the memory used */
	free(EgaGraph);
	free(BmpHead);
	free(BmpMaskedHead);
	free(SprHead);
}

void k456_import_bitmaps()
{
	BITMAP16 *bmp, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import bitmaps before initialisation!");

	/* Import all the bitmaps */
	printf("Importing bitmaps: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumBitmaps; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumBitmaps);
		
		/* Open the bitmap file */
		sprintf(filename, "%s/%cBMP%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
		bmp = bmp_load(filename);
		if(!bmp)
			quit("Can't open bitmap file %s!", filename);
		if(bmp->width % 8 != 0)
			quit("Bitmap %s is not a multiple of 8 pixels wide!", filename);

		/* Decode the bmp file */
		bmp_split(bmp, &planes[2], &planes[1], &planes[0], &planes[3]);

		/* Set up the BmpHead structures */
		BmpHead[i].Width = bmp->width / 8;
		BmpHead[i].Height = bmp->height;

		/* Allocate memory for the data */
		EgaGraph[EpisodeInfo[ep].IndexBitmaps + i].len = BmpHead[i].Width * BmpHead[i].Height * 4;
		pointer = malloc(BmpHead[i].Width * BmpHead[i].Height * 4);
		if(!pointer)
			quit("Not enough memory for bitmap %d!", i);
		EgaGraph[EpisodeInfo[ep].IndexBitmaps + i].data = pointer;

		/* Decode the bitmap data */
		for(p = 0; p < 4; p++)
		{
			if(!planes[p])
				quit("Not enough memory to create bitmap!");

			/* Decode the lines of the bitmap data */
			pointer = EgaGraph[EpisodeInfo[ep].IndexBitmaps + i].data + p * BmpHead[i].Width * BmpHead[i].Height;
			for(y = 0; y < BmpHead[i].Height; y++)
				memcpy(pointer + y * BmpHead[i].Width, planes[p]->lines[y], BmpHead[i].Width);
		}			
	
		/* Free the memory used */
		for(p = 0; p < 4; p++)
		{
			bmp_free(planes[p]);
		}
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_import_masked_bitmaps()
{
	BITMAP16 *bmp, *mbmp, *planes[5];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import masked bitmaps before initialisation!");

	/* Import all the masked bitmaps */
	printf("Importing masked bitmaps: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumMaskedBitmaps; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumMaskedBitmaps);
		
		/* Open the bitmap file */
		sprintf(filename, "%s/%cMBM%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
		mbmp = bmp_load(filename);
		if(!mbmp)
			quit("Can't open bitmap file %s!", filename);
		if(mbmp->width % 16 != 0)
			quit("Masked bitmap %s is not a multiple of 16 pixels wide!", filename);

		/* Create a copy of the image for splitting */
		bmp = bmp_create(mbmp->width / 2, mbmp->height, 4);
		if(!bmp)
			quit("Not enough memory to create bitmap!");
		bmp_blit(mbmp, 0, 0, bmp, 0, 0, bmp->width, bmp->height);
		
		/* Get the mask */
		planes[4] = bmp_create(mbmp->width / 2, mbmp->height, 1);
		if(!planes[4])
			quit("Not enough memory to create bitmap!");
		bmp_blit(mbmp, planes[4]->width, 0, planes[4], 0, 0, planes[4]->width, planes[4]->height);

		/* Decode the bmp file */
		bmp_split(bmp, &planes[2], &planes[1], &planes[0], &planes[3]);

		/* Set up the BmpHead structures */
		BmpMaskedHead[i].Width = bmp->width / 8;
		BmpMaskedHead[i].Height = bmp->height;

		/* Allocate memory for the data */
		EgaGraph[EpisodeInfo[ep].IndexMaskedBitmaps + i].len = BmpMaskedHead[i].Width * BmpMaskedHead[i].Height * 5;
		pointer = malloc(BmpMaskedHead[i].Width * BmpMaskedHead[i].Height * 5);
		if(!pointer)
			quit("Not enough memory for bitmap %d!", i);
		EgaGraph[EpisodeInfo[ep].IndexMaskedBitmaps + i].data = pointer;

		/* Decode the lines of the mask data */
		pointer = EgaGraph[EpisodeInfo[ep].IndexMaskedBitmaps + i].data;
		for(y = 0; y < BmpMaskedHead[i].Height; y++)
			memcpy(pointer + y * BmpMaskedHead[i].Width, planes[4]->lines[y], BmpMaskedHead[i].Width);

		/* Decode the bitmap data */
		for(p = 0; p < 4; p++)
		{
			if(!planes[p])
				quit("Not enough memory to create bitmap!");

			/* Decode the lines of the bitmap data */
			pointer = EgaGraph[EpisodeInfo[ep].IndexMaskedBitmaps + i].data + (p + 1) * BmpMaskedHead[i].Width * BmpMaskedHead[i].Height;
			for(y = 0; y < BmpMaskedHead[i].Height; y++)
				memcpy(pointer + y * BmpMaskedHead[i].Width, planes[p]->lines[y], BmpMaskedHead[i].Width);
		}			
	
		/* Free the memory used */
		for(p = 0; p < 5; p++)
		{
			bmp_free(planes[p]);
		}
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_import_tiles()
{
	BITMAP16 *bmp, *tile, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import tiles before initialisation!");

	/* Import all the tiles */
	printf("Importing tiles: ");

	/* Open the bitmap file */
	sprintf(filename, "%s/%cTIL0000.bmp", Switches->OutputPath, '0' + Switches->Episode);
	bmp = bmp_load(filename);
	if(!bmp)
		quit("Can't open bitmap file %s!", filename);
	if(bmp->width != 18 * 16)
		quit("Tile bitmap %s is not 288 pixels wide!", filename);
   
	for(i = 0; i < EpisodeInfo[ep].Num16Tiles; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].Num16Tiles);
		
		/* Extract the tile we want */
		tile = bmp_create(16, 16, 4);
		if(!tile)
			quit("Not enough memory to create bitmap!");
		bmp_blit(bmp, (i % 18) * 16, (i / 18) * 16, tile, 0, 0, 16, 16);
		
		/* Decode the tile */
		bmp_split(tile, &planes[2], &planes[1], &planes[0], &planes[3]);

		/* Allocate memory for the data */
		EgaGraph[EpisodeInfo[ep].Index16Tiles + i].len = 2 * 16 * 4;
		pointer = malloc(2 * 16 * 4);
		if(!pointer)
			quit("Not enough memory for tile %d!", i);
		EgaGraph[EpisodeInfo[ep].Index16Tiles + i].data = pointer;

		/* Decode the bitmap data */
		for(p = 0; p < 4; p++)
		{
			if(!planes[p])
				quit("Not enough memory to create bitmap!");

			/* Decode the lines of the image data */
			pointer = EgaGraph[EpisodeInfo[ep].Index16Tiles + i].data + p * 2 * 16;
			for(y = 0; y < 16; y++)
				memcpy(pointer + y * 2, planes[p]->lines[y], 2);
		}			
	
		/* Free the memory used */
		for(p = 0; p < 4; p++)
		{
			bmp_free(planes[p]);
		}
		bmp_free(tile);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
	
	bmp_free(bmp);
}

void k456_import_masked_tiles()
{
	BITMAP16 *bmp, *tile, *planes[5];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import masked tiles before initialisation!");

	/* Import all the masked tiles */
	printf("Importing masked tiles: ");

	/* Open the bitmap file */
	sprintf(filename, "%s/%cTIL0001.bmp", Switches->OutputPath, '0' + Switches->Episode);
	bmp = bmp_load(filename);
	if(!bmp)
		quit("Can't open bitmap file %s!", filename);
	if(bmp->width != 18 * 16 * 2)
		quit("Masked tile bitmap %s is not 576 pixels wide!", filename);
   
   	/* Skip the first tile, as it should always be transparent */
	for(i = 1; i < EpisodeInfo[ep].Num16MaskedTiles; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].Num16MaskedTiles);
		
		/* Extract the tile we want */
		tile = bmp_create(16, 16, 4);
		if(!tile)
			quit("Not enough memory to create bitmap!");
		bmp_blit(bmp, (i % 18) * 16, (i / 18) * 16, tile, 0, 0, 16, 16);
		
		/* Extract the tile mask*/
		planes[4] = bmp_create(16, 16, 1);
		if(!planes[4])
			quit("Not enough memory to create bitmap!");
		bmp_blit(bmp, 16 * 18 + (i % 18) * 16, (i / 18) * 16, planes[4], 0, 0, 16, 16);
		
		/* Decode the tile */
		bmp_split(tile, &planes[2], &planes[1], &planes[0], &planes[3]);

		/* Allocate memory for the data */
		EgaGraph[EpisodeInfo[ep].Index16MaskedTiles + i].len = 2 * 16 * 5;
		pointer = malloc(2 * 16 * 5);
		if(!pointer)
			quit("Not enough memory for masked tile %d!", i);
		EgaGraph[EpisodeInfo[ep].Index16MaskedTiles + i].data = pointer;

		/* Decode the lines of the mask data */
		pointer = EgaGraph[EpisodeInfo[ep].Index16MaskedTiles + i].data;
		for(y = 0; y < 16; y++)
			memcpy(pointer + y * 2, planes[4]->lines[y], 2);

		/* Decode the bitmap data */
		for(p = 0; p < 4; p++)
		{
			if(!planes[p])
				quit("Not enough memory to create bitmap!");

			/* Decode the lines of the image data */
			pointer = EgaGraph[EpisodeInfo[ep].Index16MaskedTiles + i].data + (p + 1) * 2 * 16;
			for(y = 0; y < 16; y++)
				memcpy(pointer + y * 2, planes[p]->lines[y], 2);
		}			
	
		/* Free the memory used */
		for(p = 0; p < 5; p++)
		{
			bmp_free(planes[p]);
		}
		bmp_free(tile);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
	
	bmp_free(bmp);
}

void k456_import_8_tiles()
{
	BITMAP16 *bmp, *tile, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import 8x8 tiles before initialisation!");

	/* Import all the 8x8 tiles */
	printf("Importing 8x8 tiles: ");

	/* Open the bitmap file */
	sprintf(filename, "%s/%cTIL0002.bmp", Switches->OutputPath, '0' + Switches->Episode);
	bmp = bmp_load(filename);
	if(!bmp)
		quit("Can't open bitmap file %s!", filename);
	if(bmp->width != 8)
		quit("8x8 Tile bitmap %s is not 8 pixels wide!", filename);

	/* Allocate memory for the all the tiles */
	EgaGraph[EpisodeInfo[ep].Index8Tiles].len = EpisodeInfo[ep].Num8Tiles * 8 * 4;
	pointer = malloc(EpisodeInfo[ep].Num8Tiles * 8 * 4);
	if(!pointer)
		quit("Not enough memory for 8x8 tiles!");
	EgaGraph[EpisodeInfo[ep].Index8Tiles].data = pointer;


	for(i = 0; i < EpisodeInfo[ep].Num8Tiles; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].Num8Tiles);
		
		/* Extract the tile we want */
		tile = bmp_create(8, 8, 4);
		if(!tile)
			quit("Not enough memory to create bitmap!");
		bmp_blit(bmp, 0, i * 8, tile, 0, 0, 8, 8);
		
		/* Decode the tile */
		bmp_split(tile, &planes[2], &planes[1], &planes[0], &planes[3]);


		/* Decode the bitmap data */
		for(p = 0; p < 4; p++)
		{
			if(!planes[p])
				quit("Not enough memory to create bitmap!");

			/* Decode the lines of the image data */
			pointer = EgaGraph[EpisodeInfo[ep].Index8Tiles].data + i * 8 * 4 + p * 8;
			for(y = 0; y < 8; y++)
				memcpy(pointer + y, planes[p]->lines[y], 1);
		}			
	
		/* Free the memory used */
		for(p = 0; p < 4; p++)
		{
			bmp_free(planes[p]);
		}
		bmp_free(tile);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
	
	bmp_free(bmp);
}

void k456_import_8_masked_tiles()
{
	BITMAP16 *bmp, *tile, *planes[5];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import 8x8 masked tiles before initialisation!");

	/* Import all the 8x8 masked tiles */
	printf("Importing 8x8 masked tiles: ");

	/* Open the bitmap file */
	sprintf(filename, "%s/%cTIL0003.bmp", Switches->OutputPath, '0' + Switches->Episode);
	bmp = bmp_load(filename);
	if(!bmp)
		quit("Can't open bitmap file %s!", filename);
	if(bmp->width != 16)
		quit("Masked 8x8 tile bitmap %s is not 16 pixels wide!", filename);

	/* Allocate memory for the data */
	EgaGraph[EpisodeInfo[ep].Index8MaskedTiles].len = EpisodeInfo[ep].Num8MaskedTiles * 8 * 5;
	pointer = malloc(EpisodeInfo[ep].Num8MaskedTiles * 8 * 5);
	if(!pointer)
		quit("Not enough memory for 8x8 masked tiles!");
	EgaGraph[EpisodeInfo[ep].Index8MaskedTiles].data = pointer;

   
	for(i = 0; i < EpisodeInfo[ep].Num8MaskedTiles; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].Num8MaskedTiles);
		
		/* Extract the tile we want */
		tile = bmp_create(8, 8, 4);
		if(!tile)
			quit("Not enough memory to create bitmap!");
		bmp_blit(bmp, 0, i * 8, tile, 0, 0, 8, 8);
		
		/* Extract the tile mask*/
		planes[4] = bmp_create(8, 8, 1);
		if(!planes[4])
			quit("Not enough memory to create bitmap!");
		bmp_blit(bmp, 8, i * 8, planes[4], 0, 0, 8, 8);
		
		/* Decode the tile */
		bmp_split(tile, &planes[2], &planes[1], &planes[0], &planes[3]);

		/* Decode the lines of the mask data */
		pointer = EgaGraph[EpisodeInfo[ep].Index8MaskedTiles].data + i * 8 * 5;
		for(y = 0; y < 8; y++)
			memcpy(pointer + y, planes[4]->lines[y], 1);

		/* Decode the bitmap data */
		for(p = 0; p < 4; p++)
		{
			if(!planes[p])
				quit("Not enough memory to create bitmap!");

			/* Decode the lines of the image data */
			pointer = EgaGraph[EpisodeInfo[ep].Index8MaskedTiles].data + i * 8 * 5 + (p + 1) * 8;
			for(y = 0; y < 8; y++)
				memcpy(pointer + y, planes[p]->lines[y], 1);
		}			
	
		/* Free the memory used */
		for(p = 0; p < 5; p++)
		{
			bmp_free(planes[p]);
		}
		bmp_free(tile);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
	
	bmp_free(bmp);
}

void k456_import_texts()
{
	char filename[PATH_MAX];
	int i;
	FILE *f;
	unsigned long len;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import texts before initialisation!");

	/* Import all the texts */
	printf("Importing texts chunks: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumTexts; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumTexts);
		
		/* Open the demo file */
		sprintf(filename, "%s/%cTXT%04d.txt", Switches->OutputPath, '0' + Switches->Episode, i);
		f = openfile(filename, "rb", Switches->Backup);
		if(!f)
			quit("Can't open %s!", filename);

		/* Get the file length */
		fseek(f, 1, SEEK_END);
		len = ftell(f) - 1;
		fseek(f, 0, SEEK_SET);
	
		/* Allocate memory */
		pointer = malloc(len);
		if(!pointer)
			quit("Not enough memory to import misc chunks!");
		EgaGraph[EpisodeInfo[ep].IndexTexts + i].len = len;
		EgaGraph[EpisodeInfo[ep].IndexTexts + i].data = pointer;

		/* Read the data and close the file */
		fread(pointer, len, 1, f);
		fclose(f);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_import_misc()
{
	char filename[PATH_MAX];
	int i;
	FILE *f;
	unsigned long len;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import misc chunks before initialisation!");

	/* Import all the misc chunks */
	printf("Importing misc chunks: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumMisc; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumMisc);
		
		/* Open the demo file */
		sprintf(filename, "%s/%cMSC%04d.bin", Switches->OutputPath, '0' + Switches->Episode, i);
		f = openfile(filename, "rb", Switches->Backup);
		if(!f)
			quit("Can't open %s!", filename);

		/* Get the file length */
		fseek(f, 1, SEEK_END);
		len = ftell(f) - 1;
		fseek(f, 0, SEEK_SET);
	
		/* Allocate memory */
		pointer = malloc(len);
		if(!pointer)
			quit("Not enough memory to import misc chunks!");
		EgaGraph[EpisodeInfo[ep].IndexMisc + i].len = len;
		EgaGraph[EpisodeInfo[ep].IndexMisc + i].data = pointer;

		/* Read the data and close the file */
		fread(pointer, len, 1, f);
		fclose(f);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_import_demos()
{
	char filename[PATH_MAX];
	int i;
	FILE *f;
	unsigned long len;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import demos before initialisation!");

	/* Import all the demos */
	printf("Importing demos: ");
   
	for(i = 0; i < EpisodeInfo[ep].NumDemos; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumDemos);
		
		/* Open the demo file */
		sprintf(filename, "%s/DEMO%d.CK%c", Switches->OutputPath, i, '0' + Switches->Episode);
		f = openfile(filename, "rb", Switches->Backup);
		if(!f)
			quit("Can't open %s!", filename);

		/* Get the file length */
		fseek(f, 1, SEEK_END);
		len = ftell(f) - 1;
		fseek(f, 0, SEEK_SET);
	
		/* Allocate memory */
		pointer = malloc(len);
		if(!pointer)
			quit("Not enough memory to import demos!");
		EgaGraph[EpisodeInfo[ep].IndexDemos + i].len = len;
		EgaGraph[EpisodeInfo[ep].IndexDemos + i].data = pointer;

		/* Read the data and close the file */
		fread(pointer, len, 1, f);
		fclose(f);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_import_fonts()
{
	BITMAP16 *font, *bmp;
	FontHeadStruct FontHead;
	char filename[PATH_MAX];
	int i, j, mw, w, bw, y, x;
	unsigned char *pointer;
	unsigned long offset;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import fonts before initialisation!");

	/* Import all the fonts from separate bitmaps*/
	printf("Importing fonts: ");

	for(i = 0; i < EpisodeInfo[ep].NumFonts; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumFonts);
		
		/* Open the bitmap */
		sprintf(filename, "%s/%cFON%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
		font = bmp_load(filename);
		if(!font)
			quit("Can't open bitmap file %s!", filename);
		if(font->width % 16 != 0)
			quit("Font bitmap %s is not a multiple of 16 pixels wide!", filename);

		/* Calculate the height and width of the cells */
		mw = font->width / 16;
		FontHead.Height = font->height / 16;
		
		/* Get the size and offsets of the characters */
		offset = sizeof(FontHeadStruct);
		for(j = 0; j < 256; j++)
		{
			/* Get the width of the character in pixels */
			for(x = 0; x < mw; x++)
				if(bmp_getpixel(font, mw * (j % 16) + x, FontHead.Height * (j / 16)) == 8)
					break;
			w = x;
			FontHead.Width[j] = w;
			
	   		/* Get the width of the character in bytes */
	   		if(FontHead.Width[j] > 0)
	   		{
	   			bw = (FontHead.Width[j] + 7) / 8;
				FontHead.Offset[j] = offset;
				offset += bw * FontHead.Height;
			}
			else
			{
				FontHead.Offset[j] = 0;
			}
		}

		/* Allocate memory */
		pointer = malloc(offset);
		if(!pointer)
			quit("Not enough memory to import fonts!");
		EgaGraph[EpisodeInfo[ep].IndexFonts + i].len = offset;
		EgaGraph[EpisodeInfo[ep].IndexFonts + i].data = pointer;
		
		/* Save the FontHead table */
		memcpy(pointer, &FontHead, sizeof(FontHeadStruct));
		pointer += sizeof(FontHeadStruct);
		
		/* Export the characters */	
		for(j = 0; j < 256; j++)
		{
	   		if(FontHead.Width[j] > 0)
			{
				/* Copy the character into a 1-bit bitmap */
				bmp = bmp_create(FontHead.Width[j], FontHead.Height, 1);
				if(!bmp)
					quit("Not enough memory to export font!");
				bmp_blit(font, mw * (j % 16), FontHead.Height * (j / 16), bmp, 0, 0, FontHead.Width[j], FontHead.Height);
	
				/* Now encode the lines of the character into the output */
	   			bw = (FontHead.Width[j] + 7) / 8;
				pointer = EgaGraph[EpisodeInfo[ep].IndexFonts + i].data + FontHead.Offset[j];
				for(y = 0; y < FontHead.Height; y++)
					memcpy(pointer + y * bw, bmp->lines[y], bw);
		
				/* And free the bitmap */
				bmp_free(bmp);
			}
		}
		
		/* Free memory */
		bmp_free(font);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k456_import_sprites()
{
	BITMAP16 *bmp, *spr, *planes[5];
	char filename[PATH_MAX];
	FILE *f;
	int i, p, y, j;
	int rx1, ry1, rx2, ry2, ox, oy, sh;
	unsigned char *pointer;
	int ep = Switches->Episode - 4;
	
	if(!ImportInitialised)
		quit("Trying to import sprites before initialisation!");

	/* Import all the sprites  */
	printf("Importing sprites: ");

	/* Open a text file for the clipping and origin info */
	sprintf(filename, "%s/%cSPRITES.txt", Switches->OutputPath, '0' + Switches->Episode);
	f = fopen(filename, "rb");
	if(!f)
		quit("Unable to open %s!");
   
	for(i = 0; i < EpisodeInfo[ep].NumSprites; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EpisodeInfo[ep].NumSprites);
		
		/* Output the collision rectangle and origin information */
		if(fscanf(f, "%d: [%d, %d, %d, %d], [%d, %d], %d\n", &j, &rx1, &ry1, &rx2, &ry2, &ox, &oy, &sh) != 8)
			quit("Error reading data for sprite %d from sprite text file!", i);
		if(j != i)
			quit("Sprite text file has entry for %d when %d expected!", j, i);
		if(sh != 1 && sh != 2 && sh != 4)
			quit("Sprite %d has an illegal shift value!", i);
			
		SprHead[i].OrgX = ox << 4;
		SprHead[i].OrgY = oy << 4;
		SprHead[i].Rx1 = (rx1 + ox) << 4;
		SprHead[i].Ry1 = (ry1 + oy) << 4;
		SprHead[i].Rx2 = (rx2 + ox) << 4;
		SprHead[i].Ry2 = (ry2 + oy) << 4;
		SprHead[i].Shifts = sh;

		/* Open the bitmap file */
		sprintf(filename, "%s/%cSPR%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
		spr = bmp_load(filename);
		if(!spr)
			quit("Can't open bitmap file %s!", filename);
		if(spr->width % 24 != 0)
			quit("Sprite bitmap %s is not a multiple of 24 pixels wide!", filename);

		/* Create a copy of the image for splitting */
		bmp = bmp_create(spr->width / 3, spr->height, 4);
		if(!bmp)
			quit("Not enough memory to create bitmap!");
		bmp_blit(spr, 0, 0, bmp, 0, 0, bmp->width, bmp->height);
		
		/* Get the mask */
		planes[4] = bmp_create(spr->width / 3, spr->height, 1);
		if(!planes[4])
			quit("Not enough memory to create bitmap!");
		bmp_blit(spr, planes[4]->width, 0, planes[4], 0, 0, planes[4]->width, planes[4]->height);

		/* Decode the bmp file */
		bmp_split(bmp, &planes[2], &planes[1], &planes[0], &planes[3]);

		/* Set up the BmpHead structures */
		SprHead[i].Width = bmp->width / 8;
		SprHead[i].Height = bmp->height;

		/* Allocate memory for the data */
		EgaGraph[EpisodeInfo[ep].IndexSprites + i].len = SprHead[i].Width * SprHead[i].Height * 5;
		pointer = malloc(SprHead[i].Width * SprHead[i].Height * 5);
		if(!pointer)
			quit("Not enough memory for bitmap %d!", i);
		EgaGraph[EpisodeInfo[ep].IndexSprites + i].data = pointer;

		/* Decode the lines of the mask data */
		pointer = EgaGraph[EpisodeInfo[ep].IndexSprites + i].data;
		for(y = 0; y < SprHead[i].Height; y++)
			memcpy(pointer + y * SprHead[i].Width, planes[4]->lines[y], SprHead[i].Width);

		/* Decode the bitmap data */
		for(p = 0; p < 4; p++)
		{
			if(!planes[p])
				quit("Not enough memory to create bitmap!");

			/* Decode the lines of the bitmap data */
			pointer = EgaGraph[EpisodeInfo[ep].IndexSprites + i].data + (p + 1) * SprHead[i].Width * SprHead[i].Height;
			for(y = 0; y < SprHead[i].Height; y++)
				memcpy(pointer + y * SprHead[i].Width, planes[p]->lines[y], SprHead[i].Width);
		}			
	
		/* Free the memory used */
		for(p = 0; p < 5; p++)
		{
			bmp_free(planes[p]);
		}
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");	
	
	fclose(f);
}
