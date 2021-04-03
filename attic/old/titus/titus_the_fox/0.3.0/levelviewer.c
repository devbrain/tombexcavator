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
#include "malloc.h"
#include "sprites.h"

int dumplevel(char *leveldata, int leveldatasize, SDL_Surface *sprites[], int level);
int main(int argc, char *argv[]);

int dumplevel(char *leveldata, int leveldatasize, SDL_Surface *sprites[], int level){
    char *tmppointer = leveldata;
    int i, j;
    unsigned int k;
    short int x, y;
    int levelheight = (leveldatasize - 35828) >> 8;

    SDL_Surface *levelsprite[256];
    tmppointer += levelheight * 256;

    SDL_Surface *screen;	//This pointer will reference the backbuffer
    SDL_Surface *image;	//This pointer will reference our bitmap sprite
    SDL_Surface *temp;	//This pointer will temporarily reference our bitmap sprite
    SDL_Rect src, dest;	//These rectangles will describe the source and destination regions of our blit
	
    //We must first initialize the SDL video component, and check for success
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
	
    //When this program exits, SDL_Quit must be called
    atexit(SDL_Quit);
	
    switch (videomode) {
    case 0: //window mode
        screen = SDL_SetVideoMode(reswidth, resheight, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
        SDL_WM_SetCaption("OpenTitus", 0);
        break;
    case 1: //fullscreen
        screen = SDL_SetVideoMode(reswidth, resheight, bitdepth, SDL_DOUBLEBUF | SDL_FULLSCREEN);
        break;
    }

    if (screen == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return 1;
    }
    for (i = 0; i < 256; i++)
        levelsprite[i] = SDL_LoadMapSprite(tmppointer, i * 128, level);

    for (i = 0; i < levelheight; i++) { //level sprites (tiles)
        for (j = 0; j < 256; j++) {
            image = SDL_DisplayFormat(levelsprite[leveldata[i * 256 + j] & 0xFF]);

            //Construct the source rectangle for our blit
            src.x = 0;
            src.y = 0;
            src.w = image->w;	//Use image->w to display the entire width of the image
            src.h = image->h;	//Use image->h to display the entire height of the image
	
            //Construct the destination rectangle for our blit
            dest.x = j * 16;		//Display the image at the (X,Y) coordinates (100,100)
            dest.y = i * 16;
            dest.w = image->w;	//Ensure the destination is large enough for the image's entire width/height
            dest.h = image->h;
            //printf("dummy2.5\n");

            //Blit the image to the backbuffer
            SDL_BlitSurface(image, &src, screen, &dest);

            //Release the surface
            SDL_FreeSurface(image);
        }
    }	

    for (i = 0; i < 100; i++) { //bonuses

        if (((leveldata[levelheight * 256 + 35084 + i * 4] & 0xFF) != 0xFF) && ((leveldata[levelheight * 256 + 35085 + i * 4] & 0xFF) != 0xFF)) {
            image = SDL_DisplayFormat(levelsprite[leveldata[levelheight * 256 + 35082 + i * 4] & 0xFF]);

            //Construct the source rectangle for our blit
            src.x = 0;
            src.y = 0;
            src.w = image->w;	//Use image->w to display the entire width of the image
            src.h = image->h;	//Use image->h to display the entire height of the image

            //Construct the destination rectangle for our blit
            dest.x = (leveldata[levelheight * 256 + 35084 + i * 4] & 0xFF) * 16;
            dest.y = (leveldata[levelheight * 256 + 35085 + i * 4] & 0xFF) * 16;
            dest.w = image->w;
            dest.h = image->h;
            //printf("dummy2.5\n");

            //Blit the image to the backbuffer
            SDL_BlitSurface(image, &src, screen, &dest);

            //Release the surface
            SDL_FreeSurface(image);
        }
    }

    for (i = 9; i >= 0; i--) { //elevators

        x = (((short int)(leveldata[levelheight * 256 + 35637 + i * 20] & 0xFF) << 8) & 0xFF00) + (short int)(leveldata[levelheight * 256 + 35636 + i * 20] & 0xFF);
        y = (((short int)(leveldata[levelheight * 256 + 35639 + i * 20] & 0xFF) << 8) & 0xFF00) + (short int)(leveldata[levelheight * 256 + 35638 + i * 20] & 0xFF);
        k = (((unsigned int)(leveldata[levelheight * 256 + 35629 + i * 20] & 0xFF) * 256) & 0xFF00) + (unsigned int)(leveldata[levelheight * 256 + 35628 + i * 20] & 0xFF);
        if ((k != 0xFFFF) && ((leveldata[levelheight * 256 + 35631 + i * 20] & 0xFF) < 8) && (x >= -16) && (y >= 0)) {
            if ((k & 0x8000) == 0x8000) //horizontal flip
                image = SDL_DisplayFormat(SDL_FlipHoriz(sprites[(k & 0x1FFF) + 30]));
            else
                image = SDL_DisplayFormat(sprites[(k & 0x1FFF) + 30]);

            //Construct the source rectangle for our blit
            src.x = 0;
            src.y = 0;
            src.w = image->w;	//Use image->w to display the entire width of the image
            src.h = image->h;	//Use image->h to display the entire height of the image

            //Construct the destination rectangle for our blit
            dest.x = x;
            dest.y = y;
            dest.w = image->w;
            dest.h = image->h;
            //printf("dummy2.5\n");

            //Blit the image to the backbuffer
            SDL_BlitSurface(image, &src, screen, &dest);

            //Release the surface
            SDL_FreeSurface(image);
        }
    }

    for (i = 49; i >= 0; i--) { //enemies
        k = (((unsigned int)(leveldata[levelheight * 256 + 33787 + i * 26] & 0xFF) * 256) & 0xFF00) + (unsigned int)(leveldata[levelheight * 256 + 33786 + i * 26] & 0xFF);
        if (k != 0xFFFF) {
            if ((k & 0x8000) == 0x8000) //horizontal flip
                image = SDL_DisplayFormat(SDL_FlipHoriz(sprites[(k & 0x1FFF) + 101]));
            else
                image = SDL_DisplayFormat(sprites[(k & 0x1FFF) + 101]);

            //Construct the source rectangle for our blit
            src.x = 0;
            src.y = 0;
            src.w = image->w;	//Use image->w to display the entire width of the image
            src.h = image->h;	//Use image->h to display the entire height of the image

            //Construct the destination rectangle for our blit
            dest.x = ((leveldata[levelheight * 256 + 33783 + i * 26] & 0xFF) * 256) + (leveldata[levelheight * 256 + 33782 + i * 26] & 0xFF) - (image->w >> 1);
            dest.y = ((leveldata[levelheight * 256 + 33785 + i * 26] & 0xFF) * 256) + (leveldata[levelheight * 256 + 33784 + i * 26] & 0xFF) - image->h;
            dest.w = image->w;
            dest.h = image->h;
            //printf("dummy2.5\n");

            //Blit the image to the backbuffer
            SDL_BlitSurface(image, &src, screen, &dest);

            //Release the surface
            SDL_FreeSurface(image);
        }
    }

    for (i = 39; i >= 0; i--) { //objects
        k = (((unsigned int)(leveldata[levelheight * 256 + 33537 + i * 6] & 0xFF) * 256) & 0xFF00) + (unsigned int)(leveldata[levelheight * 256 + 33536 + i * 6] & 0xFF);
        if (k != 0xFFFF) {
            if ((k & 0x8000) == 0x8000) //horizontal flip
                image = SDL_DisplayFormat(SDL_FlipHoriz(sprites[k & 0x1FFF]));
            else
                image = SDL_DisplayFormat(sprites[k & 0x1FFF]);

            //Construct the source rectangle for our blit
            src.x = 0;
            src.y = 0;
            src.w = image->w;	//Use image->w to display the entire width of the image
            src.h = image->h;	//Use image->h to display the entire height of the image

            //Construct the destination rectangle for our blit
            dest.x = ((leveldata[levelheight * 256 + 33539 + i * 6] & 0xFF) * 256) + (leveldata[levelheight * 256 + 33538 + i * 6] & 0xFF) - (image->w >> 1);
            dest.y = ((leveldata[levelheight * 256 + 33541 + i * 6] & 0xFF) * 256) + (leveldata[levelheight * 256 + 33540 + i * 6] & 0xFF) - image->h;
            dest.w = image->w;
            dest.h = image->h;
            //printf("dummy2.5\n");

            //Blit the image to the backbuffer
            SDL_BlitSurface(image, &src, screen, &dest);

            //Release the surface
            SDL_FreeSurface(image);
        }
    }

    image = SDL_DisplayFormat(sprites[0]);

    src.x = 0;
    src.y = 0;
    src.w = image->w;
    src.h = image->h;
	
    //tmppointer = leveldata + levelheight * 256 + 33778;
    dest.x = ((int)(leveldata[levelheight * 256 + 33779] & 0xFF) * 256) + (int)(leveldata[levelheight * 256 + 33778] & 0xFF) - (image->w >> 1);
    dest.y = ((leveldata[levelheight * 256 + 33781] & 0xFF) * 256) + (leveldata[levelheight * 256 + 33780] & 0xFF) - image->h;
    dest.w = image->w;
    dest.h = image->h;
    //printf("dummy2.5\n");

    //Blit the image to the backbuffer
    SDL_BlitSurface(image, &src, screen, &dest);

    //Release the surface
    SDL_FreeSurface(image);

    //Flip the backbuffer to the primary
    SDL_Flip(screen);

    //Wait for 2500ms (2.5 seconds) so we can see the image
    SDL_Delay(5000);

    for (i = 0; i < 256; i++)
        SDL_FreeSurface(levelsprite[i]);
	
    return 0;
}

int main(int argc, char *argv[]){
    unsigned char *leveldata;
    int i, leveldatasize, retval, curlevel;
    SDL_Surface *sprites[356];
    sscanf (argv[1], "%d", &curlevel);
    curlevel--;
    if ((retval = readconfig(OPENTITUS_CONFIG_FILE)) < 0)
        return retval;
    printf("levelfiles[0] = %s\n", levelfiles[0]);
    printf("spritefile = %s\n", spritefile);
    retval = getsprites(sprites, spritefile, curlevel + 1);

    if (retval < 0) {
        for (i = 0; i < 356; i++)
            SDL_FreeSurface(sprites[i]);
        return (retval);
    }

    retval = unSQZ(levelfiles[curlevel], &leveldata);
    printf("retval = %x\n", retval);

    if (retval < 0) {
        free (leveldata);
        for (i = 0; i < 356; i++)
            SDL_FreeSurface(sprites[i]);
        return (retval);
    }

    if (retval == 0) {
        fprintf(stderr, "Reading error with file: %s!\n", levelfiles[curlevel]);
        free (leveldata);
        for (i = 0; i < 356; i++)
            SDL_FreeSurface(sprites[i]);
        return (-9);
    }

    leveldatasize = retval;

    retval = dumplevel(leveldata, leveldatasize, sprites, curlevel + 1);
    free (leveldata);
    for (i = 0; i < 356; i++)
        SDL_FreeSurface(sprites[i]);
    return 0;
}
