/* KEEN123.C - Keen 1, 2, and 3 import and export functions.
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

// SM: Lines that were removed/added by Shadow Master <shadowm2006@gmail.com> for Linux compatibility
// SM: have a comment starting with "SM:", and entire sections start with "SM: MOD BEGIN" and end with
// SM: "SM: MOD END"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// SM: Used by this port:
#include <memory.h>
#include "linconio.h"

#include "utils.h"
#include "lz.h"
#include "bmp16.h"
#include "switches.h"

#pragma pack(1)

/* Define the structures for EGAHEAD */
typedef struct
{
   unsigned long LatchPlaneSize;    // Size of one plane of latch data
   unsigned long SpritePlaneSize;   // Size of one plane of sprite data
   unsigned long OffBitmapTable;    // Offset in EGAHEAD to bitmap table
   unsigned long OffSpriteTable;    // Offset in EGAHEAD to sprite table
   unsigned short Num8Tiles;        // Number of 8x8 tiles
   unsigned long Off8Tiles;         // Offset of 8x8 tiles (relative to plane data)
   unsigned short Num32Tiles;       // Number of 32x32 tiles (always 0)
   unsigned long Off32Tiles;        // Offset of 32x32 tiles (relative to plane data)
   unsigned short Num16Tiles;       // Number of 16x16 tiles
   unsigned long Off16Tiles;        // Offset of 16x16 tiles (relative to plane data)
   unsigned short NumBitmaps;       // Number of bitmaps in table
   unsigned long OffBitmaps;        // Offset of bitmaps (relative to plane data)
   unsigned short NumSprites;       // Number of sprites
   unsigned long OffSprites;        // Offset of sprites (relative to plane data)
   unsigned short Compressed;       // (Keen 1 only) Nonzero: LZ compressed data
} EgaHeadStruct;

typedef struct
{
   unsigned short Width;            // Width of the bitmap
   unsigned short Height;           // Height of the bitmap
   unsigned long Offset;            // Offset of bitmap in EGALATCH
   char Name[8];           			// Name of the bitmap
} BitmapHeadStruct;

typedef struct
{
   unsigned short Width;            // Width of the sprite
   unsigned short Height;           // Height of the bitmap
   unsigned short OffsetDelta;      // Remaining offset to the bitmap in bytes
   unsigned short OffsetParas;      // Offset to the bitmap in paragraphs
   unsigned short Rx1, Ry1;         // Top-left corner of clipping rectangle
   unsigned short Rx2, Ry2;         // Bottom-right corner of clipping rectangle
   char Name[16];          // Name of the sprite
} SpriteHeadStruct;

#pragma pack()

typedef struct
{
	unsigned int NumTiles;
	unsigned int NumFonts;
	unsigned int NumBitmaps;
	unsigned int NumSprites;
} EpisodeInfoStruct;


static EpisodeInfoStruct EpisodeInfo[] = {
	{	/* Dummy item 0 */
		0, 0, 0, 0
	},
	{	/* Episode 1 */
		611, 1, 18, 119
	},
	{	/* Episode 2 */
		689, 1, 18, 134
	},
	{	/* Episode 3 */
		624, 1, 14, 148
	}
};


static int ExportInitialised = 0;
static int ImportInitialised = 0;
static EgaHeadStruct *EgaHead = NULL;
static BitmapHeadStruct *BmpHead = NULL;
static SpriteHeadStruct *SprHead = NULL;
static unsigned char *LatchData = NULL;
static unsigned char *SpriteData = NULL;
static BITMAP16 *FontBmp = NULL;
static BITMAP16 *TileBmp = NULL;
static BITMAP16 **SpriteBmp = NULL;
static BITMAP16 **BitmapBmp = NULL;
static SwitchStruct *Switches = NULL;

/************************************************************************************************************/
/** KEEN 1, 2, 3 EXPORTING ROUTINES *************************************************************************/
/************************************************************************************************************/

void k123_export_begin(SwitchStruct *switches)
{	char filename[PATH_MAX];
	FILE *headfile = NULL;
	FILE *latchfile = NULL;
	FILE *spritefile = NULL;

	/* Never allow the export start to occur more than once */
	if(ExportInitialised)
		quit("Tried to initialise Keen 1 files a second time!");
	
	/* Save the switches */
	Switches = switches;
	
	/* Open EGAHEAD */
	sprintf(filename, "%s/EGAHEAD.CK%c", Switches->InputPath, '0' + Switches->Episode);
	if(!(headfile = openfile(filename, "rb", Switches->Backup)))
		quit("Can't open %s!\n", filename);

	/* Read all the header data */
	EgaHead = (EgaHeadStruct *)malloc(sizeof(EgaHeadStruct));
	if(!EgaHead)
		quit("Not enough memory to read header");
		
	fread(EgaHead, sizeof(EgaHeadStruct), 1, headfile);
	
	/* Now check that the egahead appears correct */
	if((EgaHead->Num8Tiles / 256) != EpisodeInfo[Switches->Episode].NumFonts)
		quit("EgaHead should have only %d font! Check your version!", EpisodeInfo[Switches->Episode].NumFonts );
	if(EgaHead->Num16Tiles != EpisodeInfo[Switches->Episode].NumTiles)
		quit("EgaHead should have only %d tiles! Check your version!", EpisodeInfo[Switches->Episode].NumTiles );
	if(EgaHead->NumBitmaps != EpisodeInfo[Switches->Episode].NumBitmaps)
		quit("EgaHead should have only %d bitmaps! Check your version!", EpisodeInfo[Switches->Episode].NumBitmaps );
	if(EgaHead->NumSprites != EpisodeInfo[Switches->Episode].NumSprites)
		quit("EgaHead should have only %d sprites! Check your version!", EpisodeInfo[Switches->Episode].NumSprites );

	/* Allocate space for bitmap and sprite tables */
	BmpHead = (BitmapHeadStruct *)malloc(EgaHead->NumBitmaps * sizeof(BitmapHeadStruct));
	SprHead = (SpriteHeadStruct *)malloc(EgaHead->NumSprites * 4 * sizeof(SpriteHeadStruct));
	if(!BmpHead || !SprHead)
		quit("Not enough memory to create tables!\n");
	
	/* Read the bitmap and sprite tables */
	fseek(headfile, EgaHead->OffBitmapTable, SEEK_SET);
	fread(BmpHead, sizeof(BitmapHeadStruct), EgaHead->NumBitmaps, headfile);
	fseek(headfile, EgaHead->OffSpriteTable, SEEK_SET);
	fread(SprHead, 4 * sizeof(SpriteHeadStruct), EgaHead->NumSprites, headfile);
	
	/* Open the latch and sprite files and read them into memory */
	sprintf(filename, "%s/EGALATCH.CK%c", Switches->InputPath, '0' + Switches->Episode);
	latchfile = openfile(filename, "rb", Switches->Backup);
	if(!latchfile)
		quit("Cannot open %s!\n", filename);
	sprintf(filename, "%s/EGASPRIT.CK%c", Switches->InputPath, '0' + Switches->Episode);
	spritefile = openfile(filename, "rb", Switches->Backup);
	if(!spritefile)
		quit("Cannot open %s!\n", filename);
	LatchData = (unsigned char *)malloc(EgaHead->LatchPlaneSize * 4);
	SpriteData = (unsigned char *)malloc(EgaHead->SpritePlaneSize * 5);
	if(!LatchData || !SpriteData)
		quit("Not enough memory to load latch and sprite data!\n");

	/* Read the latch and sprite data, decompressing it if necessary for Keen 1 */	
	if(Switches->Episode == 1 && EgaHead->Compressed)
	{
		fseek(latchfile, 6, SEEK_SET);
		fseek(spritefile, 6, SEEK_SET);
		lz_decompress(spritefile, SpriteData);
		lz_decompress(latchfile, LatchData);
	} else {
		fread(LatchData, EgaHead->LatchPlaneSize * 4, 1, latchfile);
		fread(SpriteData, EgaHead->SpritePlaneSize * 5, 1, spritefile);
	}

	fclose(headfile);
	fclose(latchfile);
	fclose(spritefile);

	ExportInitialised = 1;	
}


void k123_export_end()
{
	/* Clean up */
	if(!ExportInitialised)
		quit("Tried to end export before beginning it!");

	free(LatchData);
	free(SpriteData);
	free(BmpHead);
	free(SprHead);
	free(EgaHead);

	ExportInitialised = 0;
}



void k123_export_bitmaps()
{
	BITMAP16 *bmp, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	
	if(!ExportInitialised)
		quit("Trying to export bitmaps before initialisation!");

	/* Export all the bitmaps */
	printf("Exporting bitmaps");
   
	for(i = 0; i < EgaHead->NumBitmaps; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EgaHead->NumBitmaps);
		
		/* Decode the bitmap data */
		for(p = 0; p < 4; p++)
		{
			/* Create a 1bpp bitmap for each plane */
			planes[p] = bmp_create(BmpHead[i].Width * 8, BmpHead[i].Height, 1);
			
			/* Decode the lines of the bitmap data */
			pointer = LatchData + EgaHead->OffBitmaps + BmpHead[i].Offset + p * EgaHead->LatchPlaneSize;
			for(y = 0; y < BmpHead[i].Height; y++)
				memcpy(planes[p]->lines[y], pointer + y * BmpHead[i].Width, BmpHead[i].Width);
		}			

		/* Create the bitmap file */
		sprintf(filename, "%s/%cBMP%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
		bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
		if(!bmp_save(bmp, filename, Switches->Backup))
			quit("Can't open bitmap file %s!", filename);
		
		/* Free the memory used */
		for(p = 0; p < 4; p++)
			bmp_free(planes[p]);
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k123_export_sprites()
{
	BITMAP16 *spr, *bmp, *planes[5];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	SpriteHeadStruct *sprhead;
	
	if(!ExportInitialised)
		quit("Trying to export sprites before initialisation!");

	/* Export all the sprites */
	printf("Exporting sprites");
   
	for(i = 0; i < EgaHead->NumSprites; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EgaHead->NumSprites);

		/* Construct the sprite bitmap */
		sprhead = &SprHead[i * 4];
		spr = bmp_create(sprhead->Width * 8 * 3, sprhead->Height, 4);
		
		/* Decode the sprite image data */
		for(p = 0; p < 4; p++)
		{
			/* Create a 1bpp bitmap for each plane */
			planes[p] = bmp_create(sprhead->Width * 8, sprhead->Height, 1);
			
			/* Decode the lines of the image data */
			pointer = SpriteData + EgaHead->OffSprites + sprhead->OffsetParas * 16 + sprhead->OffsetDelta + p * EgaHead->SpritePlaneSize;
			for(y = 0; y < sprhead->Height; y++)
				memcpy(planes[p]->lines[y], pointer + y * sprhead->Width, sprhead->Width);
		}			
		bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
		bmp_blit(bmp, 0, 0, spr, 0, 0, bmp->width, bmp->height);

		/* Decode the sprite mask */
		planes[4] = bmp_create(sprhead->Width * 8, sprhead->Height, 1);
		pointer = SpriteData + EgaHead->OffSprites + sprhead->OffsetParas * 16 + sprhead->OffsetDelta + 4 * EgaHead->SpritePlaneSize;
		for(y = 0; y < sprhead->Height; y++)
			memcpy(planes[4]->lines[y], pointer + y * sprhead->Width, sprhead->Width);
		bmp_blit(planes[4], 0, 0, spr, bmp->width, 0, bmp->width, bmp->height);
			
		/* Draw the clipping rectangle, red within dark grey */
		bmp_rect(spr, bmp->width * 2, 0, bmp->width * 3 - 1, bmp->height - 1, 8);
		bmp_rect(spr, bmp->width * 2 + (sprhead->Rx1 >> 8), (sprhead->Ry1 >> 8),
			bmp->width * 2 + (sprhead->Rx2 >> 8), (sprhead->Ry2 >> 8), 12);

		/* Create the bitmap file */
		sprintf(filename, "%s/%cSPR%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
		if(!bmp_save(spr, filename, Switches->Backup))
			quit("Can't open bitmap file %s!", filename);
		
		/* Free the memory used */
		for(p = 0; p < 5; p++)
			bmp_free(planes[p]);
		bmp_free(bmp);
		bmp_free(spr);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k123_export_tiles()
{
	BITMAP16 *bmp, *tiles, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	
	if(!ExportInitialised)
		quit("Trying to export tiles before initialisation!");

	/* Export all the 16x16 tiles into one bitmap */
	printf("Exporting tiles");

	/* Create a bitmap large enough to hold all the tiles */
	tiles = bmp_create(13 * 16, (EgaHead->Num16Tiles + 12) / 13 * 16, 4);

	for(i = 0; i < EgaHead->Num16Tiles; i++)
	{
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EgaHead->Num16Tiles);

		for(p = 0; p < 4; p++)
		{
			/* Create a 1bpp bitmap for each plane */
			planes[p] = bmp_create(16, 16, 1);

			pointer = LatchData + EgaHead->Off16Tiles + i * 32 + p * EgaHead->LatchPlaneSize;
			for(y = 0; y < 16; y++)
				memcpy(planes[p]->lines[y], pointer + y * 2, 2);
		}

		/* Merge the tile and put it in the large bitmap */
		bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
		bmp_blit(bmp, 0, 0, tiles, (i % 13) * 16, (i / 13) * 16, 16, 16);

		/* Free the memory used */
		for(p = 0; p < 4; p++)
			bmp_free(planes[p]);
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");

	/* Save the bitmap */	
	sprintf(filename, "%s/%cTIL0000.bmp", Switches->OutputPath, '0' + Switches->Episode);
	if(!bmp_save(tiles, filename, Switches->Backup))
		quit("Can't open bitmap file %s!", filename);
	bmp_free(tiles);
}

void k123_export_fonts()
{
	BITMAP16 *bmp, *font, *planes[4];
	char filename[PATH_MAX];
	int i, p, y;
	unsigned char *pointer;
	
	if(!ExportInitialised)
		quit("Trying to export font before initialisation!");

	/* Export all the 8x8 tiles into one bitmap */
	printf("Exporting font");

	/* Create a bitmap large enough to hold all the tiles */
	font = bmp_create(16 * 8, (EgaHead->Num8Tiles + 15) / 16 * 8, 4);

	for(i = 0; i < EgaHead->Num8Tiles; i++)
	{
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EgaHead->Num8Tiles);
		
		for(p = 0; p < 4; p++)
		{
			/* Create a 1bpp bitmap for each plane */
			planes[p] = bmp_create(8, 8, 1);

			pointer = LatchData + EgaHead->Off8Tiles + i * 8 + p * EgaHead->LatchPlaneSize;
			for(y = 0; y < 8; y++)
				memcpy(planes[p]->lines[y], pointer + y, 1);
		}

		/* Merge the tile and put it in the large bitmap */
		bmp = bmp_merge(planes[2], planes[1], planes[0], planes[3]);
		bmp_blit(bmp, 0, 0, font, (i % 16) * 8, (i / 16) * 8, 8, 8);

		/* Free the memory used */
		for(p = 0; p < 4; p++)
			bmp_free(planes[p]);
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");

	/* Save the bitmap */	
	sprintf(filename, "%s/%cFON0000.bmp", Switches->OutputPath, '0' + Switches->Episode);
	if(!bmp_save(font, filename, Switches->Backup))
		quit("Can't open bitmap file %s!", filename);
	bmp_free(font);
}


/************************************************************************************************************/
/** KEEN 1, 2, 3 IMPORTING ROUTINES *************************************************************************/
/************************************************************************************************************/

void k123_import_begin(SwitchStruct *switches)
{
	char filename[PATH_MAX];
	int i;
	unsigned long size;

	/* Never allow the import start to occur more than once */
	if(ImportInitialised)
		quit("Tried to initialise Keen 1 files a second time!");
	
	/* Save the switches */
	Switches = switches;

	/* Create the header structure */
	EgaHead = (EgaHeadStruct *)malloc(sizeof(EgaHeadStruct));
	if(!EgaHead)
		quit("Not enough memory to create header");

	/* Read the font bitmap */
	sprintf(filename, "%s/%cFON0000.bmp", Switches->OutputPath, '0' + Switches->Episode);
	FontBmp = bmp_load(filename);
	if(!FontBmp)
		quit("Can't open font bitmap %s!", filename);
	if(FontBmp->width != 128)
		quit("Font bitmap %s is not 128 pixels wide!", filename);

	/* Read the tile bitmap */
	sprintf(filename, "%s/%cTIL0000.bmp", Switches->OutputPath, '0' + Switches->Episode);
	TileBmp = bmp_load(filename);
	if(!TileBmp)
		quit("Can't open tile bitmap %s!", filename);
	if(TileBmp->width != 13 * 16)
		quit("Tile bitmap %s is not 208 pixels wide!", filename);

	/* Read the bitmaps */
	BitmapBmp = (BITMAP16 **)malloc(EpisodeInfo[Switches->Episode].NumBitmaps * sizeof(BITMAP16 *));
	if(!BitmapBmp)
		quit("Not enough memory to create bitmaps!");
	for(i = 0; i < EpisodeInfo[Switches->Episode].NumBitmaps; i++)
	{
		sprintf(filename, "%s/%cBMP%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
		BitmapBmp[i] = bmp_load(filename);
		if(!BitmapBmp[i])
			quit("Can't open bitmap %s!", filename);
		if(BitmapBmp[i]->width % 8 != 0)
			quit("Bitmap %s is not a multiple of 8 pixels wide!", filename);
	}
		
	/* Read the sprites */
	SpriteBmp = (BITMAP16 **)malloc(EpisodeInfo[Switches->Episode].NumSprites * sizeof(BITMAP16 *));
	if(!SpriteBmp)
		quit("Not enough memory to create sprites!");
	for(i = 0; i < EpisodeInfo[Switches->Episode].NumSprites; i++)
	{
		sprintf(filename, "%s/%cSPR%04d.bmp", Switches->OutputPath, '0' + Switches->Episode, i);
		SpriteBmp[i] = bmp_load(filename);
		if(!SpriteBmp[i])
			quit("Can't open sprite bitmap %s!", filename);
		if(SpriteBmp[i]->width % 24 != 0)
			quit("Sprite bitmap %s is not a multiple of 24 pixels wide!", filename);
	}

	/* Now allocate the bmphead and sprhead */
	BmpHead = (BitmapHeadStruct *)malloc(EpisodeInfo[Switches->Episode].NumBitmaps * sizeof(BitmapHeadStruct));
	SprHead = (SpriteHeadStruct *)malloc(EpisodeInfo[Switches->Episode].NumSprites * 4 * sizeof(SpriteHeadStruct));
	if(!BmpHead || !SprHead)
		quit("Not enough memory to create tables!\n");

	/* Calculate the latch and sprite plane sizes */
	size = 0;
	EgaHead->OffBitmaps = size;
	for(i = 0; i < EpisodeInfo[Switches->Episode].NumBitmaps; i++)
		size += BitmapBmp[i]->width * BitmapBmp[i]->height / 8;
	size += (size % 16 == 0) ? 0 : (16 - size % 16);		/* Convert size to paragraph multiple */
	EgaHead->Off8Tiles = size;
	size += EpisodeInfo[Switches->Episode].NumFonts * 256 * 8;
	size += (size % 16 == 0) ? 0 : (16 - size % 16);		/* Convert size to paragraph multiple */
	EgaHead->Off16Tiles = size;
	size += EpisodeInfo[Switches->Episode].NumTiles * 32;
	size += (size % 16 == 0) ? 0 : (16 - size % 16);		/* Convert size to paragraph multiple */
	EgaHead->LatchPlaneSize = size;
	
	size = 0;
	EgaHead->OffSprites = size;
	for(i = 0; i < EpisodeInfo[Switches->Episode].NumSprites; i++)
		size += SpriteBmp[i]->width * SpriteBmp[i]->height / 3 / 8;
	/* The final size must be padded to a paragraph */
	EgaHead->SpritePlaneSize = size + (16 - size % 16);	

	/* Set up the EgaHead structure */
	EgaHead->Num8Tiles = EpisodeInfo[Switches->Episode].NumFonts * 256;
	EgaHead->Num32Tiles = 0;
	EgaHead->Off32Tiles = 0;
	EgaHead->Num16Tiles = EpisodeInfo[Switches->Episode].NumTiles;
	EgaHead->NumBitmaps = EpisodeInfo[Switches->Episode].NumBitmaps;
	EgaHead->OffBitmapTable = sizeof(EgaHeadStruct);
	EgaHead->NumSprites = EpisodeInfo[Switches->Episode].NumSprites;
	EgaHead->OffSpriteTable = EgaHead->OffBitmapTable + EgaHead->NumBitmaps * sizeof(BitmapHeadStruct);
	EgaHead->Compressed = 0;
	
	LatchData = (unsigned char *)malloc(EgaHead->LatchPlaneSize * 4);
	SpriteData = (unsigned char *)malloc(EgaHead->SpritePlaneSize * 5);
	if(!LatchData || !SpriteData)
		quit("Not enough memory to load latch and sprite data!\n");

	ImportInitialised = 1;
}


void k123_import_bitmaps()
{
	BITMAP16 *planes[4];
	int i, p, y;
	unsigned char *pointer;
	unsigned long offset;

	if(!ImportInitialised)
		quit("Tried to import bitmaps without initialising!");

	/* Import all the bitmaps */
	printf("Importing bitmaps");
   
   	offset = 0;
	for(i = 0; i < EgaHead->NumBitmaps; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EgaHead->NumBitmaps);
	
		/* Set up the BmpHead for this bitmap */
		BmpHead[i].Width = BitmapBmp[i]->width / 8;
		BmpHead[i].Height = BitmapBmp[i]->height;
		BmpHead[i].Offset = offset;
		strncpy(BmpHead[i].Name, "", 8);
		offset += BmpHead[i].Width * BmpHead[i].Height;
		
		/* Split the bitmap up into planes */
		bmp_split(BitmapBmp[i], &planes[2], &planes[1], &planes[0], &planes[3]);
		
		for(p = 0; p < 4; p++)
		{
			/* Copy the lines of the bitmap data */
			pointer = LatchData + EgaHead->OffBitmaps + BmpHead[i].Offset + p * EgaHead->LatchPlaneSize;
			for(y = 0; y < BmpHead[i].Height; y++)
				memcpy(pointer + y * BmpHead[i].Width, planes[p]->lines[y], BmpHead[i].Width);
		}

		/* Free the memory used */
		for(p = 0; p < 4; p++)
			bmp_free(planes[p]);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k123_import_sprites()
{
	BITMAP16 *bmp, *planes[5];
	int i, j, p, y, x;
	unsigned char *pointer;
	unsigned long offset;
	SpriteHeadStruct *sprhead;

	if(!ImportInitialised)
		quit("Tried to import sprites without initialising!");
		
	/* Import all the sprites */
	printf("Importing sprites");
   
    offset = 0;
	for(i = 0; i < EgaHead->NumSprites; i++)
	{
		/* Show that something is happening */
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EgaHead->NumSprites);

		/* Set up the SprHead for this bitmap */
		sprhead = &SprHead[i * 4];
		sprhead->Width = SpriteBmp[i]->width / 3 / 8;
		sprhead->Height = SpriteBmp[i]->height;
		sprhead->OffsetDelta = offset % 16;
		sprhead->OffsetParas = offset / 16;

		/* Work out top-left corner of the clipping rectangle */
		x = y = 0;
		for( y = 0; y < SpriteBmp[i]->height; y++ )
			for( x = 0; x < SpriteBmp[i]->width / 3; x++ )
				if(bmp_getpixel(SpriteBmp[i], SpriteBmp[i]->width / 3 * 2 + x, y) == 12)
					goto foundtl;
		foundtl:
		sprhead->Rx1 = x << 8;
		sprhead->Ry1 = y << 8;
		
		/* Work out bottom-right corner of the clipping rectangle */
		x = y = 0;
		for( y =  SpriteBmp[i]->height - 1; y >= 0; y-- )
			for( x = SpriteBmp[i]->width / 3 - 1; x >= 0; x-- )
				if(bmp_getpixel(SpriteBmp[i], SpriteBmp[i]->width / 3 * 2 + x, y) == 12)
					goto foundbr;
		foundbr:
		sprhead->Rx2 = x << 8;
		sprhead->Ry2 = y << 8;

		strncpy(sprhead->Name, "", 16);
		
		/* Copy this into the other three SprHead structures for this sprite */
		for(j = 1; j < 4; j++)
		{
			/* The extra width allows for shifts */
			SprHead[i * 4 + j].Width = sprhead->Width + 1;
			SprHead[i * 4 + j].Height = sprhead->Height;
			SprHead[i * 4 + j].OffsetDelta = sprhead->OffsetDelta;
			SprHead[i * 4 + j].OffsetParas = sprhead->OffsetParas;
			SprHead[i * 4 + j].Rx1 = sprhead->Rx1;
			SprHead[i * 4 + j].Ry1 = sprhead->Ry1;
			SprHead[i * 4 + j].Rx2 = sprhead->Rx2;
			SprHead[i * 4 + j].Ry2 = sprhead->Ry2;
			strncpy(SprHead[i * 4 + j].Name, sprhead->Name, 16);
		}
		offset += sprhead->Width * sprhead->Height;

		/* Copy the sprite image and split it up into planes */
		bmp = bmp_create(SpriteBmp[i]->width / 3, SpriteBmp[i]->height, 4);
		bmp_blit(SpriteBmp[i], 0, 0, bmp, 0, 0, bmp->width, bmp->height);
		bmp_split(bmp, &planes[2], &planes[1], &planes[0], &planes[3]);
		/* Decode the sprite image data */
		for(p = 0; p < 4; p++)
		{
			/* Copy the lines of the image data */
			pointer = SpriteData + EgaHead->OffSprites + sprhead->OffsetParas * 16 + sprhead->OffsetDelta + p * EgaHead->SpritePlaneSize;
			for(y = 0; y < sprhead->Height; y++)
				memcpy(pointer + y * sprhead->Width, planes[p]->lines[y], sprhead->Width);
		}			

		/* Encode the sprite mask */
		planes[4] = bmp_create(bmp->width, bmp->height, 1);
		bmp_blit(SpriteBmp[i], bmp->width, 0, planes[4], 0, 0, bmp->width, bmp->height);
		pointer = SpriteData + EgaHead->OffSprites + sprhead->OffsetParas * 16 + sprhead->OffsetDelta + 4 * EgaHead->SpritePlaneSize;

		for(y = 0; y < sprhead->Height; y++)
			memcpy(pointer + y * sprhead->Width, planes[4]->lines[y], sprhead->Width);
			
		/* Free the memory used */
		for(p = 0; p < 5; p++)
			bmp_free(planes[p]);
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k123_import_tiles()
{
	BITMAP16 *bmp, *planes[4];
	int i, p, y;
	unsigned char *pointer;

	if(!ImportInitialised)
		quit("Tried to import tiles without initialising!");

	/* Import all the 16x16 tiles from one bitmap */
	printf("Importing tiles");

	for(i = 0; i < EgaHead->Num16Tiles; i++)
	{
		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EgaHead->Num16Tiles);

		/* Copy the tile into the small bitmap and split it up into planes */
		bmp = bmp_create(16, 16, 4);
		bmp_blit(TileBmp, (i % 13) * 16, (i / 13) * 16, bmp, 0, 0, 16, 16);
		bmp_split(bmp, &planes[2], &planes[1], &planes[0], &planes[3]);

		for(p = 0; p < 4; p++)
		{
			pointer = LatchData + EgaHead->Off16Tiles + i * 32 + p * EgaHead->LatchPlaneSize;
			for(y = 0; y < 16; y++)
				memcpy(pointer + y * 2, planes[p]->lines[y], 2);
		}

		/* Free the memory used */
		for(p = 0; p < 4; p++)
			bmp_free(planes[p]);
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k123_import_fonts()
{
	BITMAP16 *bmp, *planes[4];
	int i, p, y;
	unsigned char *offset;

	if(!ImportInitialised)
		quit("Tried to import fonts without initialising!");
	
	/* Import all the 8x8 tiles from one bitmap */
	printf("Importing font");

	for(i = 0; i < EgaHead->Num8Tiles; i++)
	{
		/* Copy the character into the small bitmap and split it up into planes */
		bmp = bmp_create(8, 8, 4);
		bmp_blit(FontBmp, (i % 16) * 8, (i / 16) * 8, bmp, 0, 0, 8, 8);
		bmp_split(bmp, &planes[2], &planes[1], &planes[0], &planes[3]);

		gotoxy(30, wherey());
		printf("%3d%%", (i * 100) / EgaHead->Num8Tiles);
	
		for(p = 0; p < 4; p++)
		{
			offset = LatchData + EgaHead->Off8Tiles + i * 8 + p * EgaHead->LatchPlaneSize;
			for(y = 0; y < 8; y++)
				memcpy(offset + y, planes[p]->lines[y], 1);
		}

		/* Free the memory used */
		for(p = 0; p < 4; p++)
			bmp_free(planes[p]);
		bmp_free(bmp);

		//printf("\x8\x8\x8\x8");
	}
	gotoxy(30, wherey());
	printf("100%%\n");
}

void k123_import_end()
{
	int i;
	FILE *headfile, *latchfile, *spritefile;
	char filename[PATH_MAX];
	
	if(!ImportInitialised)
		quit("Tried to end import without initialising!");

	/* Save EGAHEAD data */
	sprintf(filename, "%s/EGAHEAD.CK%c", Switches->InputPath, '0' + Switches->Episode);
	headfile = openfile(filename, "wb", Switches->Backup);
	if(!headfile)
		quit("Can't open %s for writing!", filename);
	
	/* Write the egahead, bitmap and sprite tables */
	fwrite(EgaHead, sizeof(EgaHeadStruct), 1, headfile);
	fseek(headfile, EgaHead->OffBitmapTable, SEEK_SET);
	fwrite(BmpHead, sizeof(BitmapHeadStruct), EgaHead->NumBitmaps, headfile);
	fseek(headfile, EgaHead->OffSpriteTable, SEEK_SET);
	fwrite(SprHead, sizeof(SpriteHeadStruct), EgaHead->NumSprites * 4, headfile);
	
	/* Close the file */
	fclose(headfile);	


	/* Write the latch file */
	sprintf(filename, "%s/EGALATCH.CK%c", Switches->InputPath, '0' + Switches->Episode);
	latchfile = openfile(filename, "wb", Switches->Backup);
	if(!latchfile)
		quit("Can't open %s for writing!", filename);
	fwrite(LatchData, EgaHead->LatchPlaneSize * 4, 1, latchfile);
	fclose(latchfile);
	
	/* Write the sprite file */
	sprintf(filename, "%s/EGASPRIT.CK%c", Switches->InputPath, '0' + Switches->Episode);
	spritefile = openfile(filename, "wb", Switches->Backup);
	if(!spritefile)
		quit("Can't open %s for writing!", filename);
	fwrite(SpriteData, EgaHead->SpritePlaneSize * 5, 1, spritefile);
	fclose(spritefile);
		

	/* Free all the memory used */
	free(SpriteData);
	free(LatchData);
	for(i = EpisodeInfo[Switches->Episode].NumSprites - 1; i >= 0; i--)
		bmp_free(SpriteBmp[i]);
	for(i = EpisodeInfo[Switches->Episode].NumBitmaps - 1; i >= 0; i--)
		bmp_free(BitmapBmp[i]);
	bmp_free(TileBmp);
	bmp_free(FontBmp);
	free(EgaHead);

	ImportInitialised = 0;
}


