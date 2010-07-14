#ifndef ORIGINAL_H
#define ORIGINAL_H

#include "SDL/SDL.h"

#define SPRITECOUNT 356

int initoriginal();

SDL_Color orig_palette_colour[16];
SDL_Color orig_palette_level_colour[16];

char spritewidth[SPRITECOUNT];
char spriteheight[SPRITECOUNT];

/*orig_palette_colour[0]->r = 1 * 4; // Transparent color, needs to be different from the others
orig_palette_colour[0].g = 1 * 4;
orig_palette_colour[0].b = 1 * 4;
orig_palette_colour[0].unused = 0 * 4; 

orig_palette_colour[1].r = 60 * 4;
orig_palette_colour[1].g = 60 * 4;
orig_palette_colour[1].b = 60 * 4;
orig_palette_colour[1].unused = 0 * 4; 

        palette->colors[2].r = 0 * 4;
        palette->colors[2].g = 0 * 4;
        palette->colors[2].b = 0 * 4;
        palette->colors[2].unused = 0 * 4; 

        palette->colors[3].r = 24 * 4;
        palette->colors[3].g = 8 * 4;
        palette->colors[3].b = 0 * 4;
        palette->colors[3].unused = 0 * 4; 

        palette->colors[4].r = 28 * 4;
        palette->colors[4].g = 16 * 4;
        palette->colors[4].b = 0 * 4;
        palette->colors[4].unused = 0 * 4; 

        palette->colors[5].r = 40 * 4;
        palette->colors[5].g = 24 * 4;
        palette->colors[5].b = 16 * 4;
        palette->colors[5].unused = 0 * 4; 

        palette->colors[6].r = 48 * 4;
        palette->colors[6].g = 40 * 4;
        palette->colors[6].b = 24 * 4;
        palette->colors[6].unused = 0 * 4; 

        palette->colors[7].r = 60 * 4;
        palette->colors[7].g = 48 * 4;
        palette->colors[7].b = 32 * 4;
        palette->colors[7].unused = 0 * 4; 

        palette->colors[8].r = 16 * 4;
        palette->colors[8].g = 8 * 4;
        palette->colors[8].b = 0 * 4;
        palette->colors[8].unused = 0 * 4; 

        palette->colors[9].r = 28 * 4;
        palette->colors[9].g = 20 * 4;
        palette->colors[9].b = 20 * 4;
        palette->colors[9].unused = 0 * 4; 

        palette->colors[10].r = 40 * 4;
        palette->colors[10].g = 32 * 4;
        palette->colors[10].b = 32 * 4;
        palette->colors[10].unused = 0 * 4; 

        palette->colors[11].r = 12 * 4;
        palette->colors[11].g = 12 * 4;
        palette->colors[11].b = 28 * 4;
        palette->colors[11].unused = 0 * 4; 

        palette->colors[12].r = 24 * 4;
        palette->colors[12].g = 24 * 4;
        palette->colors[12].b = 40 * 4;
        palette->colors[12].unused = 0 * 4; 

        palette->colors[13].r = 32 * 4;
        palette->colors[13].g = 32 * 4;
        palette->colors[13].b = 48 * 4;
        palette->colors[13].unused = 0 * 4; 

        switch (level) {
        case 1:
            palette->colors[14].r = 0 * 4;
            palette->colors[14].g = 16 * 4;
            palette->colors[14].b = 0 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 2:
            palette->colors[14].r = 0 * 4;
            palette->colors[14].g = 16 * 4;
            palette->colors[14].b = 0 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 3:
            palette->colors[14].r = 0 * 4;
            palette->colors[14].g = 16 * 4;
            palette->colors[14].b = 0 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 4:
            palette->colors[14].r = 20 * 4;
            palette->colors[14].g = 12 * 4;
            palette->colors[14].b = 12 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 5:
            palette->colors[14].r = 40 * 4;
            palette->colors[14].g = 12 * 4;
            palette->colors[14].b = 4 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 6:
            palette->colors[14].r = 20 * 4;
            palette->colors[14].g = 12 * 4;
            palette->colors[14].b = 12 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 7:
            palette->colors[14].r = 20 * 4;
            palette->colors[14].g = 12 * 4;
            palette->colors[14].b = 12 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 8:
            palette->colors[14].r = 0 * 4;
            palette->colors[14].g = 16 * 4;
            palette->colors[14].b = 0 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 9:
            palette->colors[14].r = 40 * 4;
            palette->colors[14].g = 28 * 4;
            palette->colors[14].b = 12 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 10:
            palette->colors[14].r = 0 * 4;
            palette->colors[14].g = 20 * 4;
            palette->colors[14].b = 16 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 11:
            palette->colors[14].r = 40 * 4;
            palette->colors[14].g = 28 * 4;
            palette->colors[14].b = 12 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 12:
            palette->colors[14].r = 40 * 4;
            palette->colors[14].g = 28 * 4;
            palette->colors[14].b = 12 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 13:
            palette->colors[14].r = 40 * 4;
            palette->colors[14].g = 28 * 4;
            palette->colors[14].b = 12 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 14:
            palette->colors[14].r = 40 * 4;
            palette->colors[14].g = 28 * 4;
            palette->colors[14].b = 12 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        case 15:
            if (game == 0) { //Titus
                palette->colors[14].r = 48 * 4;
                palette->colors[14].g = 8 * 4;
                palette->colors[14].b = 0 * 4;
                palette->colors[14].unused = 0 * 4; 
            } else if (game == 1) { //Moktar
                palette->colors[14].r = 40 * 4;
                palette->colors[14].g = 28 * 4;
                palette->colors[14].b = 12 * 4;
                palette->colors[14].unused = 0 * 4; 
            }
            break;
        case 16:
            palette->colors[14].r = 48 * 4;
            palette->colors[14].g = 8 * 4;
            palette->colors[14].b = 0 * 4;
            palette->colors[14].unused = 0 * 4; 
            break;
        }

        palette->colors[15].r = 8 * 4;
        palette->colors[15].g = 8 * 4;
        palette->colors[15].b = 24 * 4;
        palette->colors[15].unused = 0 * 4; 

        palette->ncolors = 16;
    }	
*/

#endif
