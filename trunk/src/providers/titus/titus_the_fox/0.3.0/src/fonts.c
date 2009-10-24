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
#include <string.h>
#include "SDL/SDL.h"
#include "sqz.h"
#include "settings.h"
#include "fonts.h"
#include "backbuffer.h"
#include "error.h"
#include "keyboard.h"
#include <time.h>

SDL_Surface * SDL_Text(char *text, int inv_back){
    SDL_Surface *surface = NULL;
    SDL_Surface *charsurface = NULL;
    SDL_Palette *palette;
    char *tmpchar;
    //int fontsize = 1968; //41 characters 8*12, 16 color
    unsigned char *fontdata;
    int retval;
    int currentchar;
    int offset;
    unsigned int i;
    int j;
    SDL_Rect src, dest;

    retval = unSQZ(fontfile, &fontdata);

    if (retval < 0) {
        lasterrornr = retval;
        free (fontdata);
        return (NULL);
    }

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 8 * strlen(text), 12, 8, 0, 0, 0, 0);
    charsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 12, 8, 0, 0, 0, 0);
    palette = (charsurface->format)->palette;
    if (palette) {
        palette->colors[0].r = 0 * 4; // Unused
        palette->colors[0].g = 63 * 4;
        palette->colors[0].b = 63 * 4;
        palette->colors[0].unused = 0 * 4; 

        palette->colors[1].r = 0 * 4; // Black
        palette->colors[1].g = 0 * 4;
        palette->colors[1].b = 0 * 4;
        palette->colors[1].unused = 0 * 4; 

        palette->colors[2].r = 63 * 4; // Unused
        palette->colors[2].g = 0 * 4;
        palette->colors[2].b = 0 * 4;
        palette->colors[2].unused = 0 * 4; 

        palette->colors[3].r = 0 * 4; // Unused
        palette->colors[3].g = 63 * 4;
        palette->colors[3].b = 0 * 4;
        palette->colors[3].unused = 0 * 4; 

        palette->colors[4].r = 0 * 4; // Unused
        palette->colors[4].g = 0 * 4;
        palette->colors[4].b = 63 * 4;
        palette->colors[4].unused = 0 * 4; 

        palette->colors[5].r = 60 * 4; // Brightest yellow
        palette->colors[5].g = 60 * 4;
        palette->colors[5].b = 28 * 4;
        palette->colors[5].unused = 0 * 4; 

        palette->colors[6].r = 60 * 4;
        palette->colors[6].g = 60 * 4;
        palette->colors[6].b = 12 * 4;
        palette->colors[6].unused = 0 * 4; 

        palette->colors[7].r = 60 * 4;
        palette->colors[7].g = 56 * 4;
        palette->colors[7].b = 0 * 4;
        palette->colors[7].unused = 0 * 4; 

        palette->colors[8].r = 56 * 4;
        palette->colors[8].g = 48 * 4;
        palette->colors[8].b = 0 * 4;
        palette->colors[8].unused = 0 * 4; 

        palette->colors[9].r = 52 * 4;
        palette->colors[9].g = 44 * 4;
        palette->colors[9].b = 0 * 4;
        palette->colors[9].unused = 0 * 4; 

        palette->colors[10].r = 52 * 4;
        palette->colors[10].g = 36 * 4;
        palette->colors[10].b = 0 * 4;
        palette->colors[10].unused = 0 * 4; 

        palette->colors[11].r = 44 * 4;
        palette->colors[11].g = 36 * 4;
        palette->colors[11].b = 0 * 4;
        palette->colors[11].unused = 0 * 4; 

        palette->colors[12].r = 36 * 4;
        palette->colors[12].g = 28 * 4;
        palette->colors[12].b = 0 * 4;
        palette->colors[12].unused = 0 * 4; 

        palette->colors[13].r = 32 * 4;
        palette->colors[13].g = 24 * 4;
        palette->colors[13].b = 0 * 4;
        palette->colors[13].unused = 0 * 4; 

        palette->colors[14].r = 24 * 4;
        palette->colors[14].g = 20 * 4;
        palette->colors[14].b = 0 * 4;
        palette->colors[14].unused = 0 * 4; 

        palette->colors[15].r = 20 * 4; // Darkest yellow
        palette->colors[15].g = 16 * 4;
        palette->colors[15].b = 0 * 4;
        palette->colors[15].unused = 0 * 4; 

        palette->ncolors = 16;
    }	

    if (inv_back) //Invisible (transparent) background
        SDL_SetColorKey(surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, 1);

    SDL_SetColors(surface, palette->colors, 0, 16);

    src.x = 0;
    src.y = 0;
    src.w = charsurface->w;
    src.h = charsurface->h;

    dest.x = 0;
    dest.y = 0;
    dest.w = charsurface->w;
    dest.h = charsurface->h;

    tmpchar = (char *)charsurface->pixels;

    for (currentchar = 0; currentchar < strlen(text); currentchar++) {
        if (((text[currentchar] & 0xFF) >= CHAR_a) && ((text[currentchar] & 0xFF) <= CHAR_z)) // "a-z"
            offset = (text[currentchar] & 0xFF) - CHAR_a + 15;
        else if (((text[currentchar] & 0xFF) >= CHAR_A) && ((text[currentchar] & 0xFF) <= CHAR_Z)) // "A-Z"
            offset = (text[currentchar] & 0xFF) - CHAR_A + 15;
        else if (((text[currentchar] & 0xFF) >= CHAR_0) && ((text[currentchar] & 0xFF) <= CHAR_9)) // "0-9"
            offset = (text[currentchar] & 0xFF) - CHAR_0;
        else if ((text[currentchar] & 0xFF) == CHAR_EXCLAMATION) // "!"
            offset = 10;
        else if ((text[currentchar] & 0xFF) == CHAR_QUESTION) // "?"
            offset = 11;
        else if ((text[currentchar] & 0xFF) == CHAR_DOT) // "."
            offset = 12;
        else if ((text[currentchar] & 0xFF) == CHAR_DOLLAR) // "$"
            offset = 13;
        else if ((text[currentchar] & 0xFF) == CHAR_UNDERSCORE) // "_"
            offset = 14;
        else {
            for (i = 0; i < 96; i++) {
                *tmpchar = 0x01;
                tmpchar++;
            }
            tmpchar -= 96;
            SDL_BlitSurface(charsurface, &src, surface, &dest);
            dest.x += 8;
            continue;
        }

        for (i = offset * 48; i < offset * 48 + 12; i++) {//12
            for (j = 7; j >= 0; j--) {
                *tmpchar = (fontdata[i] >> j) & 0x01;
                *tmpchar += (fontdata[i + 12] >> j << 1) & 0x02;
                *tmpchar += (fontdata[i + 12 * 2] >> j << 2) & 0x04;
                *tmpchar += (fontdata[i + 12 * 3] >> j << 3) & 0x08;
                tmpchar++;
            }
        }

        tmpchar -= 96;

        SDL_BlitSurface(charsurface, &src, surface, &dest);
        dest.x += 8;

    }

done:
    free (fontdata);
    SDL_FreeSurface(charsurface);
    return(surface);
}

int SDL_Print_Text(char *text, int x, int y, int inv_back){
    SDL_Rect src, dest;
    SDL_Surface *surface = SDL_Text(text, inv_back);

    if (surface == NULL)
        return (lasterrornr);

    SDL_Surface *image = SDL_DisplayFormat(surface);

    src.x = 0;
    src.y = 0;
    src.w = image->w;
    src.h = image->h;
	
    dest.x = x;
    dest.y = y;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, &src, screen, &dest);
    SDL_FreeSurface(image);
    return (0);
}

int viewintrotext(){
    int retval;
    char tmpstring[41];
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    SDL_Print_Text("     YEAAA . . .", 0, 5 * 12, 0);
    sprintf(tmpstring, "YOU ARE STILL PLAYING MOKTAR IN %d !!", timeinfo->tm_year + 1900);
    SDL_Print_Text(tmpstring, 0, 6 * 12, 0);
    SDL_Print_Text(" PROGRAMMED IN 1991 ON AT .286 12MHZ.", 0, 12 * 12, 0);
    SDL_Print_Text("   . . . ENJOY MOKTAR ADVENTURE !!", 0, 13 * 12, 0);

    SDL_Flip(screen);

    retval = waitforbutton();
    if (retval < 0)
        return retval;

    SDL_Print_Text("     YEAAA . . .", 0, 5 * 12, 0);
    sprintf(tmpstring, "YOU ARE STILL PLAYING MOKTAR IN %d !!", timeinfo->tm_year + 1900);
    SDL_Print_Text(tmpstring, 0, 6 * 12, 0);
    SDL_Print_Text("REPROGRAMMED IN 2008 ON A .686 1.83 GHZ.", 0, 12 * 12, 0);
    SDL_Print_Text("   . . . ENJOY MOKTAR ADVENTURE !!", 0, 13 * 12, 0);

    SDL_Flip(screen);

    retval = waitforbutton();
    if (retval < 0)
        return retval;

    return (0);
}
