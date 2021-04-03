/*   
 * Copyright (C) 2008 Eirik Stople
 *
 * OpenTitus is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 3  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "sprites.h"
#include "sqz.h"
#include "settings.h"
#include "original.h"
#include "error.h"

SDL_Surface * copysurface(SDL_Surface * original, char flip){
    int i, j;

    SDL_Surface *surface = SDL_ConvertSurface(original, original->format, original->flags);
    if (surface == NULL)
        return NULL;

    SDL_SetColorKey(surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0); //Set transparent colour

    char *orig_pixels = (char*) original->pixels;
    char *dest_pixels = (char*) surface->pixels;

    if (flip) {
        for (i = 0; i < original->pitch; i++)
            for (j = 0; j < original->h; j++)
                dest_pixels[j * original->pitch + i] = orig_pixels[j * original->pitch + (original->pitch - i - 1)];
    } else {
        for (i = 0; i < original->pitch * original->h; i++)
                dest_pixels[i] = orig_pixels[i];
    }

    return(surface);
}


SDL_Surface * SDL_LoadSprite(unsigned char * first, char width, char height, unsigned int offset, SDL_PixelFormat * pixelformat){
    SDL_Surface *surface = NULL;
    char *tmpchar;
    unsigned int groupsize = ((width * height) >> 3);
    unsigned int i;
    int j;

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, pixelformat->BitsPerPixel, pixelformat->Rmask, pixelformat->Gmask, pixelformat->Bmask, pixelformat->Amask);
    copypixelformat(surface->format, pixelformat);

    tmpchar = (char *)surface->pixels;
    if (surface->format->BitsPerPixel == 8) {
        for (i = offset; i < offset + groupsize; i++) {
            for (j = 7; j >= 0; j--) {
                *tmpchar = (first[i] >> j) & 0x01;
                *tmpchar += (first[i + groupsize] >> j << 1) & 0x02;
                *tmpchar += (first[i + groupsize * 2] >> j << 2) & 0x04;
                *tmpchar += (first[i + groupsize * 3] >> j << 3) & 0x08;
                tmpchar++;
            }
        }
    }

    return(surface);
}

int loadsprites(SDL_Surface ***sprites, char * spritedata, int spritedatasize, SDL_PixelFormat * pixelformat) {
    int i;
    unsigned int offset = 0;

    *sprites = (SDL_Surface **)SDL_malloc(sizeof(SDL_Surface *) * SPRITECOUNT);
    if (sprites == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load sprites!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }

    for (i = 0; i < SPRITECOUNT; i++) {
        (*sprites)[i] = SDL_LoadSprite(spritedata, spritewidth[i], spriteheight[i], offset, pixelformat);
        offset += ((unsigned int)spritewidth[i] * (unsigned int)spriteheight[i]) >> 1;
    }

    return SPRITECOUNT;
}

int freesprites(SDL_Surface ***sprites) {
    int i;

    for (i = 0; i < SPRITECOUNT; i++)
        free ((*sprites)[i]);

    free (*sprites);

    return 0;
}

SDL_Surface * SDL_LoadTile(unsigned char * first, int i, SDL_PixelFormat * pixelformat){
    SDL_Surface *surface = NULL;
    char *tmpchar;
    int j, k;
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 16, 8, 0, 0, 0, 0);

    copypixelformat(surface->format, pixelformat);

    tmpchar = (char *)surface->pixels;
    for (j = i; j < i + 0x20; j++) {
        for (k = 7; k >= 0; k--) {
            *tmpchar = (first[j] >> k) & 0x01;
            *tmpchar += (first[j + 0x20] >> k << 1) & 0x02;
            *tmpchar += (first[j + 0x40] >> k << 2) & 0x04;
            *tmpchar += (first[j + 0x60] >> k << 3) & 0x08;
            tmpchar++;
        }
    }
done:
    return(surface);
}

int copypixelformat(SDL_PixelFormat * destformat, SDL_PixelFormat * srcformat) {
    int i;

    if (srcformat->palette != NULL) {
        destformat->palette->ncolors = srcformat->palette->ncolors;
        for (i = 0; i < destformat->palette->ncolors; i++) {
            destformat->palette->colors[i].r = srcformat->palette->colors[i].r;
            destformat->palette->colors[i].g = srcformat->palette->colors[i].g;
            destformat->palette->colors[i].b = srcformat->palette->colors[i].b;
            destformat->palette->colors[i].unused = srcformat->palette->colors[i].unused;
        }
    }

    destformat->BitsPerPixel = srcformat->BitsPerPixel;
    destformat->BytesPerPixel = srcformat->BytesPerPixel;

    destformat->Rloss = srcformat->Rloss;
    destformat->Gloss = srcformat->Gloss;
    destformat->Bloss = srcformat->Bloss;
    destformat->Aloss = srcformat->Aloss;

    destformat->Rshift = srcformat->Rshift;
    destformat->Gshift = srcformat->Gshift;
    destformat->Bshift = srcformat->Bshift;
    destformat->Ashift = srcformat->Ashift;

    destformat->Rmask = srcformat->Rmask;
    destformat->Gmask = srcformat->Gmask;
    destformat->Bmask = srcformat->Bmask;
    destformat->Amask = srcformat->Amask;

    destformat->colorkey = srcformat->colorkey;
    destformat->alpha = srcformat->alpha;

    return 0;
}

