/* MODKEEN.C - source for a tile importer/exporter for Keen 1 - 6.
** Greetings and thanks to Anders Gavare and Daniel Olson for their assistance.
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
**
**
** Revision history:
** 25 March 2002: ModLatch implicit Version 1.0
**      Initial release, DJGPP+Allegro
** 27 March 2002: Version 1.1
**      Added LZ routines and Keen 1 support, and BMP i/o routines so it
**      compiles under plain DJGPP.
**  1 April 2002:   Renamed to MODKEEN version 1.0
**      Full support for editing sprites and bitmaps in all three.!!!
** 22 February 2004: Version 2.0-build
**      Began adding support for Keen 4.
**  5 March 2004: Version 2.0 Beta 1
**		Supports exporting from Keen 4, 5, 6
**  7 March 2004: Version 2.0 Beta 2
**		Supports importing from Keen 4, 5, 6
**  8 March 2004: Version 2.0 Beta 3
**		Fixed bugs with too many chunks and with % displaying
** 13 March 2004: Version 2.0 Beta 4
**		Fixed bug with EGAHEAD reading, made extraction automatic, and added
**      more sanity checks.
** 14 March 2004: Version 2.0.0
**		Initial public release
**  1 April 2004: Version 2.0.1
**		Fixed bug in Keen 1-3 importing where different data types were not
**		paragraph-aligned.
*/

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "utils.h"
#include "switches.h"
#include "keen123.h"
#include "keen456.h"

#include "linconio.h"

int main(int argc, char *argv[]) {
	SwitchStruct *switches;

	/* Setup terminal */
	if (linconio_init()) {
		printf("CONIO: failed to setup terminal.");
		return -1;
	}

	/* Display the signature */
	printf("LMODKEEN I - ModKeen 2.0.1 Linux Port, Copyright (c) 2007 by Shadow Master\n"
			"ModKeen 2.0.1 source code is Copyright (c) 2002-2004 Andrew Durdin\n"
			"\n");
	
	/* Get the options */
	switches = getswitches(argc, argv);

	if(switches->Export)
	{
		/* Export all data */
		if(switches->Episode >= 1 && switches->Episode <= 3)
		{
			k123_export_begin(switches);
			k123_export_bitmaps();
			k123_export_sprites();
			k123_export_tiles();
			k123_export_fonts();
			k123_export_end();
		}
		else if(switches->Episode >= 4 && switches->Episode <= 6)
		{
			k456_export_begin(switches);
			k456_export_fonts();
			k456_export_bitmaps();
			k456_export_masked_bitmaps();
			k456_export_tiles();
			k456_export_masked_tiles();
			k456_export_8_tiles();
			k456_export_8_masked_tiles();
			k456_export_sprites();
			k456_export_texts();
			k456_export_misc();
			k456_export_demos();
			k456_export_end();
		}
	}
 	else if(switches->Import)
	{
		/* Export all data */
		if(switches->Episode >= 1 && switches->Episode <= 3)
		{
			k123_import_begin(switches);
			k123_import_bitmaps();
			k123_import_sprites();
			k123_import_tiles();
			k123_import_fonts();
			k123_import_end();
		}
		else if(switches->Episode >= 4 && switches->Episode <= 6)
		{
			k456_import_begin(switches);
			k456_import_fonts();
			k456_import_bitmaps();
			k456_import_masked_bitmaps();
			k456_import_tiles();
			k456_import_masked_tiles();
			k456_import_8_tiles();
			k456_import_8_masked_tiles();
			k456_import_sprites();
			k456_import_texts();
			k456_import_misc();
			k456_import_demos();
			k456_import_end();
		}
	}
	
	printf("Done!\n\n");
	
	/* Quit, indicating success */
	return 0;
}

#if 0
void import( EgaHead *head, BitmapHead *bmphead, SpriteHead *sprhead,
      BITMAP16 *fontbmp, BITMAP16 *tilebmp, BITMAP16 **bitmaps, BITMAP16 **sprites,
      FILE *headfile, FILE *latchfile, FILE *spritefile ) {
   int p, i, x, y, lx, ly;
   int latchpad, spritepad;

   /* Pad the planes to paragraphs */
   latchpad = (16 - (head->LatchPlaneSize % 16)) % 16;
   head->LatchPlaneSize += latchpad;

   /* Output the latch data */
   for( p = 0; p < 4; p++ ) {
      for( i = 0; i < head->Num8Tiles; i++ )
         decode_plane( 1 << p, latchfile, fontbmp, i%(fontbmp->w/8) * 8, i/(fontbmp->w/8) * 8, 8, 8 );
      for( i = 0; i < head->Num16Tiles; i++ )
         decode_plane( 1 << p, latchfile, tilebmp, i%(tilebmp->w/16) * 16, i/(tilebmp->w/16) * 16, 16, 16 );
      for( i = 0; i < head->NumBitmaps; i++ )
         decode_plane( 1 << p, latchfile, bitmaps[i], 0, 0, bitmaps[i]->w, bitmaps[i]->h );
      for( i = 0; i < latchpad; i++ )
         fputc( 0, latchfile );
   }

   /* Update the sprite header with clipping rectangle info etc */
   for( i = 0; i < head->NumSprites * 4; i+=4 ) {
      sprhead[i+3].Width = sprhead[i+2].Width = sprhead[i+1].Width = sprhead[i].Width + 1;
      sprhead[i+3].Height = sprhead[i+2].Height = sprhead[i+1].Height = sprhead[i].Height;
      sprhead[i+3].OffsetParas = sprhead[i+2].OffsetParas = sprhead[i+1].OffsetParas =   0;
      sprhead[i+3].OffsetDelta = sprhead[i+2].OffsetDelta = sprhead[i+1].OffsetDelta =   0;
      strncpy( sprhead[i+3].Name, sprhead[i].Name, 16 );
      strncpy( sprhead[i+2].Name, sprhead[i].Name, 16 );
      strncpy( sprhead[i+1].Name, sprhead[i].Name, 16 );
      /* Work out top-left corner of the clipping rectangle */
      lx = ly = 0;
      for( y = 0; y < sprites[i/4]->h; y++ ) {
         for( x = 0; x < sprites[i/4]->w / 3; x++ ) {
            p = bmp_getpixel( sprites[i/4], sprites[i/4]->w / 3 * 2 + x, y );
            if( p == 12 ) {
               lx = x;
               ly = y;
               goto foundtl;
            }
         }
      }
foundtl:
      sprhead[i+3].Rx1 = sprhead[i+2].Rx1 = sprhead[i+1].Rx1 = sprhead[i].Rx1 = lx << 8;
      sprhead[i+3].Ry1 = sprhead[i+2].Ry1 = sprhead[i+1].Ry1 = sprhead[i].Ry1 = ly << 8;

      /* Work out bottom-right corner of the clipping rectangle */
      lx = sprites[i/4]->w - 1;
      ly = sprites[i/4]->h - 1;
      for( y = 0; y < sprites[i/4]->h; y++ ) {
         for( x = 0; x < sprites[i/4]->w/3; x++ ) {
            p = bmp_getpixel( sprites[i/4], sprites[i/4]->w/3 * 2 + x, y );
            if( p == 12 ) {
               lx = x;
               ly = y;
            }
         }
      }
      sprhead[i+3].Rx2 = sprhead[i+2].Rx2 = sprhead[i+1].Rx2 = sprhead[i].Rx2 = lx << 8;
      sprhead[i+3].Ry2 = sprhead[i+2].Ry2 = sprhead[i+1].Ry2 = sprhead[i].Ry2 = ly << 8;
   }

   /* Pad the planes to paragraphs */
   spritepad = (16 - (head->SpritePlaneSize % 16)) % 16;
   head->SpritePlaneSize += spritepad;

   /* Output the sprite data */
   for( p = 0; p < 4; p++ ) {
      for( i = 0; i < head->NumSprites; i++ )
         decode_plane( 1 << p, spritefile, sprites[i], 0, 0, sprites[i]->w / 3, sprites[i]->h );
      for( i = 0;   i < spritepad; i++ )
         fputc( 0, spritefile );
   }
   for( i = 0; i < head->NumSprites; i++ )
      decode_plane( 15, spritefile, sprites[i], sprites[i]->w / 3, 0, sprites[i]->w / 3, sprites[i]->h );
   for( i = 0;   i < spritepad; i++ )
      fputc( 0, spritefile );

   /* Output the header */
   fwrite( head, sizeof( EgaHead ), 1, headfile );
   fwrite( bmphead, head->NumBitmaps * sizeof( BitmapHead ), 1, headfile );
   fwrite( sprhead, head->NumSprites * 4 * sizeof( SpriteHead ), 1, headfile );
}

void encode_plane( int cmask, char *pbits, BITMAP16 *bmp, int bx, int by, int w, int h ) {
   int tx, ty, x, y, c, p, pixel;
   int pmask;
   int b;

   /* Decode the data */
   for( y = 0; y < h; y++ ) {
      for( x = 0; x < w/8; x++ ) {
         b = *pbits++;

         /* Decode the byte */
         pmask = 128;
         for( p = 0; p < 8; p++ ) {
            if( b & pmask ) {
               /* Shove a bit in the correct plane */
               pixel = bmp_getpixel( bmp, bx+x*8+p, by+y );
               pixel |= cmask;
               bmp_putpixel( bmp, bx+x*8+p, by+y, pixel );
            }
            pmask >>= 1;
         }
      }
   }
}


void decode_plane( int cmask, FILE *fout, BITMAP16 *bmp, int bx, int by,
   int w, int h ) {
   int c, x, y, b, p, pmask;

   /* There are four bitplanes */
   for( y = 0; y < h; y++ ) {
      for( x = 0; x < w/8; x++ ) {
         b = 0;

         /* Decode the byte */
         pmask = 128;
         for( p = 0; p < 8; p++ ) {
            if( bmp_getpixel( bmp, bx+x*8+p, by+y ) & cmask ) {
                 /* Shove a bit in the correct plane */
               b |= pmask;
            }
            pmask >>= 1;
           }

         fputc( b, fout );
      }
   }
}



#endif
