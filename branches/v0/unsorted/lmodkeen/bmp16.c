/* BMP16.C - 1-bit and 4-bit BMP manipulation routines.
**
** Copyright (c)2002-2004 Andrew Durdin. (andy@durdin.net)
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
#include <memory.h>

#include "utils.h"
#include "bmp16.h"

#include "linconio.h"

#define PACKED  __attribute__((packed))

#define BMP_SIG   0x4D42
#define BI_RGB    0

typedef PACKED struct  BITMAPFILEHEADER {
    unsigned short bfType PACKED;            /* Must be 0x4D42 = "BM" (spec) */
    unsigned long  bfSize PACKED;            /* Size in bytes of BMP file */
    unsigned short bfReserved1 PACKED;       /* Must be 0 (spec) */
    unsigned short bfReserved2 PACKED;       /* Must be 0 (spec) */
    unsigned long  bfOffBits PACKED;         /* Offset in bytes to data */
} BITMAPFILEHEADER;

typedef PACKED struct BITMAPINFOHEADER {
    unsigned long biSize PACKED;             /* Size of structure: 40 */
    long biWidth PACKED;                     /* Width of bitmap in pixels */
    long biHeight PACKED;                    /* Negative heights not supported */
    unsigned short biPlanes PACKED;          /* Must be 1 (spec) */
    unsigned short biBitCount PACKED;        /* Only 4 and 1 supported */
    unsigned long biCompression PACKED;      /* Only BI_RGB supported */
    unsigned long biSizeImage PACKED;        /* Size in bytes of image */
    long biXPelsPerMeter PACKED;             /* Ignored - 0 */
    long biYPelsPerMeter PACKED;             /* Ignored - 0 */
    unsigned long biClrUsed PACKED;          /* Ignored - 0*/
    unsigned long biClrImportant PACKED;     /* Ignored - 0 */
} BITMAPINFOHEADER;

typedef PACKED struct RGBQUAD {
    unsigned char rgbBlue PACKED;
    unsigned char rgbGreen PACKED;
    unsigned char rgbRed PACKED;
    unsigned char rgbReserved PACKED;
} RGBQUAD;

void bmp_free(BITMAP16 *bmp) {
    if(bmp) {
        if(bmp->lines)
            free(bmp->lines);
        if(bmp->bits)
            free(bmp->bits);
        free(bmp);
    }
}

BITMAP16 *bmp_load(char *fname) {
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    BITMAP16 *bmp;
    FILE *fin;
    int y;

    /* Open the input picture */
    fin = fopen(fname, "rb");
    if(!fin) {
        return NULL;
    }

    /* Read the header and make sure it's a real BMP */
    fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fin);
    if(bfh.bfType != BMP_SIG) {
        fclose(fin);
        return NULL;
    }

    /* Make sure it's a format we can handle */
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, fin);
    if(bih.biPlanes != 1 || (bih.biBitCount != 4 && bih.biBitCount != 1) ||
            bih.biCompression != BI_RGB || bih.biHeight < 0) {
        fclose(fin);
        return NULL;
    }

    /* Create a memory bitmap */
    bmp = bmp_create(bih.biWidth, bih.biHeight, bih.biBitCount);
    if(!bmp) {
        fclose(fin);
        return NULL;
    }

    /* Now load the data into the buffer, one line at a time */
    fseek(fin, bfh.bfOffBits, SEEK_SET);
    for(y = bmp->height - 1; y >= 0; y--)
        fread(bmp->lines[y], bmp->linewidth, 1, fin);

    /* Close the input file and return the bitmap pointer */
    fclose(fin);
    return bmp;
}

int bmp_save(BITMAP16 *bmp, char *fname, int backup) {
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    RGBQUAD palette[16];
    FILE *fout;
    int i, y;
    int colors;

    if(!bmp)
        return 0;
    colors = (1 << bmp->bpp);

    /* Open the output picture */
    fout = openfile(fname, "wb", backup);
    if(!fout) {
        return 0;
    }

    /* Initialise the BITMAPFILEHEADER */
    bfh.bfType = BMP_SIG;
    bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
        colors * sizeof(RGBQUAD) + bmp->linewidth * bmp->height;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
        colors * sizeof(RGBQUAD);

    /* Initialise the BITMAPINFOHEADER */
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = bmp->width;
    bih.biHeight = bmp->height;
    bih.biPlanes = 1;
    bih.biBitCount = bmp->bpp;
    bih.biCompression = BI_RGB;
    bih.biSizeImage = bmp->linewidth * bmp->height;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;

    /* Initialise the palette */
    if(bmp->bpp == 4)
    {
        for(i = 0; i < colors; i++) {
            palette[i].rgbRed = ((i & 4) != 0) * 168 + ((i & 8) != 0) * 84;
            palette[i].rgbGreen = ((i & 2) != 0) * 168 + ((i & 8) != 0) * 84;
            palette[i].rgbBlue = ((i & 1) != 0) * 168 + ((i & 8) != 0) * 84;
            palette[i].rgbReserved = 0;
        }
        /* Make palette entry 6 brown, not spew */
        palette[6].rgbGreen = 84;
    }
    else if(bmp->bpp == 1)
    {
         palette[0].rgbRed = palette[0].rgbGreen = palette[0].rgbBlue = 0;
         palette[1].rgbRed = palette[1].rgbGreen = palette[1].rgbBlue = 255;
         palette[0].rgbReserved = palette[1].rgbReserved = 0;
    }

    /* Write the headers and palette */
    fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fout);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fout);
    fwrite(palette, sizeof(RGBQUAD), colors, fout);

    /* Now save the data from the buffer, one line at a time */
    for(y = bmp->height - 1; y >= 0; y--)
        fwrite(bmp->lines[y], bmp->linewidth, 1, fout);

    /* Close the output file and return success */
    fclose(fout);
    return 1;
}

BITMAP16 *bmp_create(int width, int height, int bpp) {
    BITMAP16 *bmp;
    int y;

    /* Allocate the BITMAP structure */
    bmp = (BITMAP16 *)malloc(sizeof(BITMAP16));
    if(!bmp) {
        return NULL;
    }
    bmp->width = width;
    bmp->height = height;

    /* Allow only 1bpp and 4bpp bitmaps */
    if(bpp != 1 && bpp != 4)
        bpp = 4;
    bmp->bpp = bpp;

    /* Calculate line width in bytes, with dword padding */
    bmp->linewidth = (bmp->width * bmp->bpp + 31) >> 3 & ~3;
    bmp->lines = NULL;
    bmp->bits = NULL;

    /* Allocate the pixel buffer */
    bmp->bits = (unsigned char *)malloc(bmp->height * bmp->linewidth *
        sizeof(unsigned char));
    if(!bmp->bits) {
        bmp_free(bmp);
        return NULL;
    }

    /* Allocate the lines array */
    bmp->lines = (unsigned char **)malloc(bmp->height *
        sizeof(unsigned char *));
    if(!bmp->lines) {
        bmp_free(bmp);
        return NULL;
    }
    for(y = 0; y < bmp->height; y++)
        bmp->lines[y] = bmp->bits + bmp->linewidth * y;

    /* Clear the bitmap */
    memset(bmp->bits, 0, bmp->linewidth * bmp->height);

    return bmp;
}

int bmp_getpixel(BITMAP16 *bmp, int x, int y) {
    register int shift;
    if(x < 0 || x >= bmp->width || y < 0 || y >= bmp->height)
        return -1;

    if(bmp->bpp == 1)
    {
        shift = (~x & 7);
        return (bmp->lines[y][x / 8] >> shift) & 0x01;
    }
    else if(bmp->bpp == 4)
    {
        shift = (x & 1) ? 0 : 4;
        return (bmp->lines[y][x / 2] >> shift) & 0x0F;
    }
    else
    {
    	return 0;
    }
}

int bmp_putpixel(BITMAP16 *bmp, int x, int y, int c) {
    register int mask, shift;
    if(x < 0 || x >= bmp->width || y < 0 || y >= bmp->height)
        return -1;

    if(bmp->bpp == 1)
    {
        shift = (~x & 7);
        mask = 0x01 << shift;
        bmp->lines[y][x / 8] &= ~mask;
        bmp->lines[y][x / 8] |= ((c & 1) << shift);
    }
    else if(bmp->bpp == 4)
    {
        shift = (x & 1) ? 0 : 4;
        mask = 0x0F << shift;
        bmp->lines[y][x / 2] &= ~mask;
        bmp->lines[y][x / 2] |= (c << shift) & mask;
    }

    return c;
}

void bmp_rect(BITMAP16 *bmp, int x1, int y1, int x2, int y2, int c) {
    int x, y;

    for(y = y1; y <= y2; y++)
       for(x = x1; x <= x2; x++)
          bmp_putpixel(bmp, x, y, c);
}

BITMAP16 *bmp_duplicate(BITMAP16 *bmp)
{
    BITMAP16 *bmp2 = bmp_create(bmp->width, bmp->height, bmp->bpp);
    memcpy(bmp2->bits, bmp->bits, bmp->linewidth * bmp->height);
    
    return bmp2;
}

int bmp_split(BITMAP16 *bmp, BITMAP16 **red, BITMAP16 **green, BITMAP16 **blue, BITMAP16 **bright)
{
    if(bmp->bpp == 4)
    {
        BITMAP16 *planes[4];
        int p, y, x, c;

        for(p = 0; p < 4; p++)
        {
            planes[p] = bmp_create(bmp->width, bmp->height, 1);
            
            for(y = 0; y < bmp->height; y++)
            {
                for(x = 0; x < bmp->width; x++)
                {
                    c = bmp_getpixel(bmp, x, y);
                    c = (c & (1 << p)) ? 1 : 0;
                    bmp_putpixel(planes[p], x, y, c);
                }
            }
        }
        *blue = planes[0];
        *green = planes[1];
        *red = planes[2];
        *bright = planes[3];
        return 1;
    }
    else
    {
        return 0;
    }
}

BITMAP16 *bmp_merge(BITMAP16 *red, BITMAP16 *green, BITMAP16 *blue, BITMAP16 *bright)
{
    BITMAP16 *bmp;
    int p, y, x, c;
    BITMAP16 *planes[4];
    
    planes[0] = blue;
    planes[1] = green;
    planes[2] = red;
    planes[3] = bright;
    
    for(p = 0; p < 4; p++)
        if(!planes[p] || planes[p]->bpp != 1)
            return NULL;

    bmp = bmp_create(planes[0]->width, planes[1]->height, 4);
    if(!bmp)
    	return NULL;

    for(y = 0; y < bmp->height; y++)
    {
        for(x = 0; x < bmp->width; x++)
        {
            c = bmp_getpixel(planes[0], x, y) & 1;
            c |= (bmp_getpixel(planes[1], x, y) & 1) << 1;
            c |= (bmp_getpixel(planes[2], x, y) & 1) << 2;
            c |= (bmp_getpixel(planes[3], x, y) & 1) << 3;
            bmp_putpixel(bmp, x, y, c);
        }
    }

	return bmp;
}

void bmp_blit(BITMAP16 *src, unsigned int srcx, unsigned int srcy, BITMAP16 *dest, unsigned int destx, unsigned int desty, unsigned int width, unsigned int height)
{
	int x, y;
	int c;

	/* Sanitise arguments */
	if(srcx > src->width) srcx = src->width - 1;
	if(srcx + width > src->width) width = src->width - srcx;
	if(srcy > src->height) srcy = src->height - 1;
	if(srcy + height > src->height) height = src->height - srcy;
	
	if(destx > dest->width) destx = dest->width - 1;
	if(destx + width > dest->width) width = dest->width - destx;
	if(desty > dest->height) desty = dest->height - 1;
	if(desty + height > dest->height) height = dest->height - desty;
	
	/* Copy the data */
	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			/* Read a pixel, and convert it between bpp */
			c = bmp_getpixel(src, srcx + x, srcy + y);
			if(src->bpp == 1 && dest->bpp == 4)
				c = (c == 1 ? 15 : 0);
			if(src->bpp == 4 && dest->bpp == 1)
				c = (c > 7 ? 1 : 0);
			bmp_putpixel(dest, destx + x, desty + y, c);
		}
	}
}
