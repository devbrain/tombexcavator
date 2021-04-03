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
#include "original.h"
#include "settings.h"

int initoriginal() {

    int i;

    char spritewidth_titus[] = {32, 32, 24, 24, 32, 32, 32, 32, 32, 32, 24, 24, 24, 32, 32, 32, 24,
                                24, 24, 32, 24, 24, 24, 32, 32, 32, 32, 32, 32, 32, 24, 24, 16, 16,
                                24, 16, 16, 8, 16, 16, 16, 24, 16, 24, 8, 24, 16, 16, 16, 32, 48,
                                40, 40, 32, 16, 16, 40, 32, 24, 8, 16, 32, 24, 24, 32, 24, 32, 32,
                                16, 24, 16, 32, 32, 16, 16, 32, 32, 32, 32, 24, 16, 24, 32, 32, 40,
                                16, 16, 16, 16, 24, 16, 16, 32, 32, 32, 16, 32, 48, 16, 16, 16, 40,
                                32, 24, 32, 40, 48, 48, 48, 48, 32, 32, 24, 24, 40, 40, 40, 40, 40,
                                40, 40, 24, 24, 56, 32, 56, 24, 24, 24, 32, 24, 32, 32, 32, 40, 40,
                                40, 40, 32, 32, 40, 40, 32, 40, 32, 32, 32, 32, 40, 40, 40, 32, 24,
                                24, 24, 24, 24, 40, 32, 32, 32, 32, 32, 32, 56, 32, 40, 32, 16, 16,
                                16, 16, 32, 32, 32, 32, 32, 32, 32, 32, 40, 32, 40, 40, 40, 32, 32,
                                32, 32, 32, 32, 32, 32, 56, 32, 40, 48, 32, 32, 24, 32, 32, 32, 32,
                                40, 40, 40, 24, 24, 32, 32, 40, 32, 32, 24, 24, 24, 32, 40, 48, 48,
                                32, 32, 40, 40, 48, 40, 32, 32, 40, 40, 40, 32, 32, 40, 32, 24, 40,
                                32, 40, 40, 32, 16, 16, 24, 40, 40, 32, 40, 40, 64, 64, 64, 48, 48,
                                48, 48, 56, 48, 72, 48, 56, 24, 56, 56, 56, 56, 48, 32, 32, 32, 32,
                                48, 48, 40, 40, 40, 48, 56, 48, 16, 16, 40, 48, 40, 40, 40, 40, 40,
                                32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 24, 32, 24, 24, 40, 24, 32,
                                24, 24, 32, 40, 40, 40, 32, 40, 24, 24, 32, 24, 16, 16, 16, 24, 24,
                                24, 16, 24, 16, 8, 32, 56, 64, 88, 64, 64, 80, 72, 80, 32, 40, 40,
                                40, 40, 40, 32, 24, 32, 40, 16, 24, 24, 24, 32, 32, 40, 32, 16};

    char spriteheight_titus[] = {31, 32, 32, 32, 31, 26, 15, 16, 32, 32, 31, 32, 32, 29, 28, 32, 31,
                                 32, 32, 32, 31, 32, 29, 32, 32, 32, 32, 32, 32, 31, 30, 23, 16, 16,
                                 20, 16, 14, 16, 16, 15, 21, 10, 9, 4, 6, 17, 5, 7, 5, 9, 8,
                                 5, 5, 17, 11, 16, 32, 32, 12, 8, 9, 21, 11, 17, 16, 20, 25, 22,
                                 23, 17, 9, 13, 24, 11, 21, 19, 16, 15, 15, 30, 25, 21, 17, 22, 8,
                                 29, 8, 7, 7, 29, 9, 15, 9, 23, 8, 23, 21, 33, 12, 12, 13, 32,
                                 33, 32, 33, 30, 33, 33, 40, 33, 33, 32, 33, 32, 33, 33, 33, 31, 35,
                                 32, 40, 25, 27, 35, 61, 29, 19, 16, 20, 20, 20, 27, 28, 32, 32, 19,
                                 21, 19, 25, 25, 28, 36, 35, 36, 35, 32, 32, 40, 32, 16, 37, 30, 29,
                                 30, 18, 17, 19, 21, 21, 41, 42, 41, 41, 41, 41, 50, 40, 36, 15, 9,
                                 14, 11, 7, 9, 10, 18, 27, 27, 27, 27, 14, 22, 27, 33, 24, 18, 35,
                                 38, 37, 38, 37, 38, 31, 23, 31, 31, 28, 49, 49, 49, 34, 35, 35, 29,
                                 31, 33, 31, 33, 31, 33, 39, 26, 41, 34, 11, 16, 30, 32, 35, 28, 19,
                                 11, 26, 26, 35, 26, 27, 17, 17, 26, 25, 20, 26, 26, 26, 26, 39, 38,
                                 38, 37, 37, 37, 24, 40, 30, 38, 30, 24, 50, 54, 59, 49, 55, 50, 61,
                                 48, 42, 52, 46, 47, 52, 52, 33, 61, 59, 60, 60, 58, 35, 35, 35, 39,
                                 45, 43, 23, 40, 41, 40, 52, 31, 16, 16, 51, 53, 55, 55, 56, 55, 56,
                                 39, 36, 38, 38, 37, 38, 38, 39, 36, 54, 54, 53, 54, 53, 32, 32, 31,
                                 32, 31, 27, 37, 38, 38, 15, 49, 38, 41, 37, 41, 15, 16, 16, 21, 24,
                                 21, 21, 30, 25, 9, 21, 101, 99, 78, 69, 45, 41, 55, 41, 33, 30, 33,
                                 35, 34, 33, 34, 34, 34, 34, 10, 14, 19, 24, 26, 28, 32, 33, 9};

    char spritewidth_moktar[] = {24, 32, 24, 32, 32, 32, 32, 32, 32, 32, 32, 24, 24, 32, 32, 32, 24,
                                 32, 24, 32, 24, 24, 32, 32, 32, 40, 40, 40, 32, 48, 24, 24, 16, 16,
                                 24, 16, 16, 8, 16, 16, 16, 24, 16, 24, 8, 24, 16, 16, 16, 32, 48,
                                 40, 40, 32, 16, 16, 40, 32, 24, 8, 16, 32, 24, 24, 32, 24, 32, 32,
                                 16, 24, 16, 32, 32, 16, 16, 32, 32, 32, 32, 24, 16, 24, 32, 32, 40,
                                 16, 16, 16, 16, 24, 16, 16, 32, 32, 32, 16, 32, 48, 16, 16, 16, 40,
                                 32, 24, 32, 40, 48, 48, 48, 48, 32, 32, 24, 24, 40, 40, 40, 40, 40,
                                 40, 40, 24, 24, 56, 32, 56, 24, 24, 24, 32, 24, 32, 32, 32, 40, 40,
                                 40, 40, 32, 32, 40, 40, 32, 40, 32, 32, 32, 32, 40, 40, 40, 32, 24,
                                 24, 24, 24, 24, 40, 32, 32, 32, 32, 32, 32, 56, 32, 40, 32, 16, 16,
                                 16, 16, 32, 32, 32, 32, 32, 32, 32, 32, 40, 32, 40, 40, 40, 32, 32,
                                 32, 32, 32, 32, 32, 32, 56, 32, 40, 48, 32, 32, 24, 32, 32, 32, 32,
                                 40, 40, 40, 24, 24, 32, 32, 40, 32, 32, 24, 24, 24, 32, 40, 48, 48,
                                 32, 32, 40, 40, 48, 40, 32, 32, 40, 40, 40, 32, 32, 40, 32, 24, 40,
                                 32, 40, 40, 32, 16, 16, 24, 40, 40, 32, 40, 40, 64, 64, 64, 48, 48,
                                 48, 48, 56, 48, 72, 48, 56, 24, 56, 56, 56, 56, 48, 32, 32, 32, 32,
                                 48, 48, 40, 40, 40, 48, 56, 48, 16, 16, 40, 48, 40, 40, 40, 40, 40,
                                 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 24, 32, 24, 24, 40, 24, 32,
                                 24, 24, 32, 40, 40, 40, 32, 40, 24, 24, 32, 24, 16, 16, 16, 24, 24,
                                 24, 16, 24, 16, 8, 32, 56, 64, 88, 64, 64, 80, 72, 80, 32, 40, 48,
                                 40, 40, 40, 32, 32, 40, 40, 16, 24, 24, 24, 32, 32, 40, 32, 16};

    char spriteheight_moktar[] = {32, 31, 30, 31, 31, 21, 16, 16, 32, 32, 29, 32, 32, 28, 24, 32, 32,
                                  32, 32, 32, 32, 32, 27, 32, 32, 34, 32, 32, 33, 33, 30, 23, 16, 16,
                                  20, 16, 14, 16, 16, 15, 21, 10, 9, 4, 6, 17, 5, 7, 5, 9, 8,
                                  5, 5, 17, 11, 16, 32, 32, 12, 8, 9, 21, 11, 17, 16, 20, 25, 22,
                                  23, 17, 9, 13, 24, 11, 21, 19, 16, 15, 15, 30, 25, 21, 17, 22, 8,
                                  29, 8, 7, 7, 29, 9, 15, 9, 23, 8, 23, 21, 33, 12, 12, 13, 32,
                                  33, 32, 33, 30, 33, 33, 40, 33, 33, 32, 33, 32, 33, 33, 33, 31, 35,
                                  32, 40, 25, 27, 35, 61, 29, 19, 16, 20, 20, 20, 27, 28, 32, 32, 19,
                                  21, 19, 27, 27, 31, 32, 31, 34, 31, 32, 32, 40, 32, 16, 37, 30, 29,
                                  30, 18, 17, 19, 21, 21, 41, 42, 41, 41, 41, 41, 50, 41, 36, 15, 9,
                                  14, 11, 7, 9, 10, 18, 27, 27, 27, 27, 14, 22, 27, 33, 24, 18, 35,
                                  38, 37, 38, 37, 38, 31, 23, 31, 31, 28, 49, 49, 49, 34, 35, 35, 29,
                                  31, 33, 31, 33, 31, 33, 39, 26, 41, 34, 11, 16, 30, 32, 35, 28, 19,
                                  11, 26, 26, 35, 26, 27, 17, 17, 26, 25, 20, 26, 26, 26, 26, 39, 38,
                                  38, 37, 37, 37, 24, 40, 30, 38, 30, 24, 50, 54, 59, 49, 55, 50, 61,
                                  48, 42, 52, 46, 47, 52, 52, 33, 61, 59, 60, 60, 58, 35, 35, 35, 39,
                                  45, 43, 23, 40, 41, 40, 52, 31, 16, 16, 51, 53, 55, 55, 56, 55, 56,
                                  39, 36, 38, 38, 37, 38, 38, 39, 36, 54, 54, 53, 54, 53, 32, 32, 31,
                                  32, 31, 27, 37, 38, 38, 15, 49, 38, 41, 37, 41, 15, 16, 16, 21, 24,
                                  21, 21, 30, 25, 9, 21, 101, 99, 78, 69, 45, 41, 55, 41, 33, 30, 33,
                                  35, 34, 33, 29, 30, 30, 31, 10, 14, 19, 24, 26, 28, 32, 33, 9};

    if (game == 0) { //Titus
        for (i = 0; i < SPRITECOUNT; i++) {
            spritewidth[i] = spritewidth_titus[i];
            spriteheight[i] = spriteheight_titus[i];
        }
    } else if (game == 1) { //Moktar
        for (i = 0; i < SPRITECOUNT; i++) {
            spritewidth[i] = spritewidth_moktar[i];
            spriteheight[i] = spriteheight_moktar[i];
        }
    }

    orig_palette_colour[0].r = 1 * 4; // Transparent color, needs to be different from the others
    orig_palette_colour[0].g = 1 * 4;
    orig_palette_colour[0].b = 1 * 4;
    orig_palette_colour[0].unused = 0 * 4; 

    orig_palette_colour[1].r = 60 * 4;
    orig_palette_colour[1].g = 60 * 4;
    orig_palette_colour[1].b = 60 * 4;
    orig_palette_colour[1].unused = 0 * 4; 

    orig_palette_colour[2].r = 0 * 4;
    orig_palette_colour[2].g = 0 * 4;
    orig_palette_colour[2].b = 0 * 4;
    orig_palette_colour[2].unused = 0 * 4; 

    orig_palette_colour[3].r = 24 * 4;
    orig_palette_colour[3].g = 8 * 4;
    orig_palette_colour[3].b = 0 * 4;
    orig_palette_colour[3].unused = 0 * 4; 

    orig_palette_colour[4].r = 28 * 4;
    orig_palette_colour[4].g = 16 * 4;
    orig_palette_colour[4].b = 0 * 4;
    orig_palette_colour[4].unused = 0 * 4; 

    orig_palette_colour[5].r = 40 * 4;
    orig_palette_colour[5].g = 24 * 4;
    orig_palette_colour[5].b = 16 * 4;
    orig_palette_colour[5].unused = 0 * 4; 

    orig_palette_colour[6].r = 48 * 4;
    orig_palette_colour[6].g = 40 * 4;
    orig_palette_colour[6].b = 24 * 4;
    orig_palette_colour[6].unused = 0 * 4; 

    orig_palette_colour[7].r = 60 * 4;
    orig_palette_colour[7].g = 48 * 4;
    orig_palette_colour[7].b = 32 * 4;
    orig_palette_colour[7].unused = 0 * 4; 

    orig_palette_colour[8].r = 16 * 4;
    orig_palette_colour[8].g = 8 * 4;
    orig_palette_colour[8].b = 0 * 4;
    orig_palette_colour[8].unused = 0 * 4; 

    orig_palette_colour[9].r = 28 * 4;
    orig_palette_colour[9].g = 20 * 4;
    orig_palette_colour[9].b = 20 * 4;
    orig_palette_colour[9].unused = 0 * 4; 

    orig_palette_colour[10].r = 40 * 4;
    orig_palette_colour[10].g = 32 * 4;
    orig_palette_colour[10].b = 32 * 4;
    orig_palette_colour[10].unused = 0 * 4; 

    orig_palette_colour[11].r = 12 * 4;
    orig_palette_colour[11].g = 12 * 4;
    orig_palette_colour[11].b = 28 * 4;
    orig_palette_colour[11].unused = 0 * 4; 

    orig_palette_colour[12].r = 24 * 4;
    orig_palette_colour[12].g = 24 * 4;
    orig_palette_colour[12].b = 40 * 4;
    orig_palette_colour[12].unused = 0 * 4; 

    orig_palette_colour[13].r = 32 * 4;
    orig_palette_colour[13].g = 32 * 4;
    orig_palette_colour[13].b = 48 * 4;
    orig_palette_colour[13].unused = 0 * 4; 

    orig_palette_colour[14].r = 0 * 4; //I think this is only used by 16 bit colour images (TITREEGA.SQZ and MENUEGA.SQZ), on levels this color is replaced with orig_palette_level_colour[levelnr]
    orig_palette_colour[14].g = 16 * 4;
    orig_palette_colour[14].b = 0 * 4;
    orig_palette_colour[14].unused = 0 * 4; 

    orig_palette_colour[15].r = 8 * 4;
    orig_palette_colour[15].g = 8 * 4;
    orig_palette_colour[15].b = 24 * 4;
    orig_palette_colour[15].unused = 0 * 4; 


    orig_palette_level_colour[0].r = 0 * 4;
    orig_palette_level_colour[0].g = 16 * 4;
    orig_palette_level_colour[0].b = 0 * 4;
    orig_palette_level_colour[0].unused = 0 * 4; 

    orig_palette_level_colour[1].r = 0 * 4;
    orig_palette_level_colour[1].g = 16 * 4;
    orig_palette_level_colour[1].b = 0 * 4;
    orig_palette_level_colour[1].unused = 0 * 4; 

    orig_palette_level_colour[2].r = 0 * 4;
    orig_palette_level_colour[2].g = 16 * 4;
    orig_palette_level_colour[2].b = 0 * 4;
    orig_palette_level_colour[2].unused = 0 * 4; 

    orig_palette_level_colour[3].r = 20 * 4;
    orig_palette_level_colour[3].g = 12 * 4;
    orig_palette_level_colour[3].b = 12 * 4;
    orig_palette_level_colour[3].unused = 0 * 4; 

    orig_palette_level_colour[4].r = 40 * 4;
    orig_palette_level_colour[4].g = 12 * 4;
    orig_palette_level_colour[4].b = 4 * 4;
    orig_palette_level_colour[4].unused = 0 * 4; 

    orig_palette_level_colour[5].r = 20 * 4;
    orig_palette_level_colour[5].g = 12 * 4;
    orig_palette_level_colour[5].b = 12 * 4;
    orig_palette_level_colour[5].unused = 0 * 4; 

    orig_palette_level_colour[6].r = 20 * 4;
    orig_palette_level_colour[6].g = 12 * 4;
    orig_palette_level_colour[6].b = 12 * 4;
    orig_palette_level_colour[6].unused = 0 * 4; 

    orig_palette_level_colour[7].r = 0 * 4;
    orig_palette_level_colour[7].g = 16 * 4;
    orig_palette_level_colour[7].b = 0 * 4;
    orig_palette_level_colour[7].unused = 0 * 4; 

    orig_palette_level_colour[8].r = 40 * 4;
    orig_palette_level_colour[8].g = 28 * 4;
    orig_palette_level_colour[8].b = 12 * 4;
    orig_palette_level_colour[8].unused = 0 * 4; 

    orig_palette_level_colour[9].r = 0 * 4;
    orig_palette_level_colour[9].g = 20 * 4;
    orig_palette_level_colour[9].b = 16 * 4;
    orig_palette_level_colour[9].unused = 0 * 4; 

    orig_palette_level_colour[10].r = 40 * 4;
    orig_palette_level_colour[10].g = 28 * 4;
    orig_palette_level_colour[10].b = 12 * 4;
    orig_palette_level_colour[10].unused = 0 * 4; 

    orig_palette_level_colour[11].r = 40 * 4;
    orig_palette_level_colour[11].g = 28 * 4;
    orig_palette_level_colour[11].b = 12 * 4;
    orig_palette_level_colour[11].unused = 0 * 4; 

    orig_palette_level_colour[12].r = 40 * 4;
    orig_palette_level_colour[12].g = 28 * 4;
    orig_palette_level_colour[12].b = 12 * 4;
    orig_palette_level_colour[12].unused = 0 * 4; 

    orig_palette_level_colour[13].r = 40 * 4;
    orig_palette_level_colour[13].g = 28 * 4;
    orig_palette_level_colour[13].b = 12 * 4;
    orig_palette_level_colour[13].unused = 0 * 4; 

    if (game == 0) { //Titus
        orig_palette_level_colour[14].r = 48 * 4;
        orig_palette_level_colour[14].g = 8 * 4;
        orig_palette_level_colour[14].b = 0 * 4;
        orig_palette_level_colour[14].unused = 0 * 4; 
    } else if (game == 1) { //Moktar
        orig_palette_level_colour[14].r = 40 * 4;
        orig_palette_level_colour[14].g = 28 * 4;
        orig_palette_level_colour[14].b = 12 * 4;
        orig_palette_level_colour[14].unused = 0 * 4; 
    }

    orig_palette_level_colour[15].r = 48 * 4;
    orig_palette_level_colour[15].g = 8 * 4;
    orig_palette_level_colour[15].b = 0 * 4;
    orig_palette_level_colour[15].unused = 0 * 4; 

    return 0;
}
