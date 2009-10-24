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
#include "backbuffer.h"
#include "menu.h"
#include "fonts.h"
#include "settings.h"

int viewmenu(char * menufile, int menuformat) {
    SDL_Surface *surface;
    SDL_Palette *palette;
    char *tmpchar;
    SDL_Surface *image;
    //int menusize = 66304; //for 256 color
    unsigned char *menudata;
    int retval;
    int i;
    int j;
    int menuloop = 1;
    int selection = 0;
    SDL_Event event;
    int curlevel = 1;

    unsigned int fade_time = 1000;
    unsigned int tick_start = 0;
    unsigned int image_alpha = 0;

    SDL_Rect src, dest;
    SDL_Rect sel[2];

    retval = unSQZ(menufile, &menudata);

    if (retval < 0) {
        free (menudata);
        return (retval);
    }

    switch (menuformat) {
    case 1: //Planar 16-color

        break;

    case 2: //256 color
        surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 200, 8, 0, 0, 0, 0);
        palette = (surface->format)->palette;
        if (palette) {
            for (i = 0; i < 256; i++) {
                palette->colors[i].r = (menudata[i * 3] & 0xFF) * 4;
                palette->colors[i].g = (menudata[i * 3 + 1] & 0xFF) * 4;
                palette->colors[i].b = (menudata[i * 3 + 2] & 0xFF) * 4;
                palette->colors[i].unused = 0; 
            }
            palette->ncolors = 256;
        }

        tmpchar = (char *)surface->pixels;
        for (i = 256 * 3; i < 256 * 3 + 320*200; i++) {
            *tmpchar = menudata[i];
            tmpchar++;
        }

        image = SDL_DisplayFormat(surface);
        palette = NULL;

        SDL_FreeSurface(surface);

        break;
    }

    free (menudata);

    src.x = 0;
    src.y = 0;
    src.w = image->w;
    src.h = image->h;
	
    dest.x = 0;
    dest.y = 0;
    dest.w = image->w;
    dest.h = image->h;

    if (game == 0) { //Titus

        sel[0].x = 120;
        sel[0].y = 160;
        sel[0].w = 8;
        sel[0].h = 8;
	
        sel[1].x = 120;
        sel[1].y = 173;
        sel[1].w = 8;
        sel[1].h = 8;

    } else if (game == 1) { //Moktar

        sel[0].x = 130;
        sel[0].y = 167;
        sel[0].w = 8;
        sel[0].h = 8;
	
        sel[1].x = 130;
        sel[1].y = 180;
        sel[1].w = 8;
        sel[1].h = 8;

    }

    tick_start = SDL_GetTicks();

    while (image_alpha < 255) { //Fade in

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_FreeSurface(image);
                return (-1);
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    SDL_FreeSurface(image);
                    return (-1);
                }
            }
        }

        image_alpha = (SDL_GetTicks() - tick_start) * 256 / fade_time;

        if (image_alpha > 255)
            image_alpha = 255;

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        //SDL_SetAlpha(sel_surface, SDL_SRCALPHA, image_alpha);
        SDL_SetAlpha(image, SDL_SRCALPHA, image_alpha);
        SDL_BlitSurface(image, &src, screen, &dest);
        SDL_BlitSurface(image, &sel[1], screen, &sel[0]);
        SDL_BlitSurface(image, &sel[0], screen, &sel[selection]);
        //SDL_BlitSurface(sel_surface, &sel_src, screen, &sel[selection]);
        SDL_Flip(screen);
        SDL_Delay(1);
    }

    beforemenuloop:

    while (menuloop) { //View the menu

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_FreeSurface(image);
                return (-1);
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    SDL_FreeSurface(image);
                    return (-1);
                }
                if (event.key.keysym.sym == SDLK_UP)
                    selection = 0;
                if (event.key.keysym.sym == SDLK_DOWN)
                    selection = 1;
                if (event.key.keysym.sym == SDLK_RETURN)
                    menuloop = 0;

            }
        }

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(image, &src, screen, &dest);
        SDL_BlitSurface(image, &sel[1], screen, &sel[0]);
        SDL_BlitSurface(image, &sel[0], screen, &sel[selection]);
        //SDL_BlitSurface(sel_surface, &sel_src, screen, &sel[selection]);
        SDL_Flip(screen);
        SDL_Delay(1);
    }

    switch (selection) {
    case 0: //Start

        break;

    case 1: //Password
        retval = enterpassword();

        if (retval < 0)
            return retval;

        if (retval > 0) {
            if (retval <= levelcount)
                curlevel = retval;
        }
        selection = 0;
        menuloop = 1;
        goto beforemenuloop;
        break;

    default:
        return (-1);
        break;
    }

    tick_start = SDL_GetTicks();
    image_alpha = 0;
    while (image_alpha < 255) { //Fade out

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_FreeSurface(image);
                return (-1);
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    SDL_FreeSurface(image);
                    return (-1);
                }
            }
        }

        image_alpha = (SDL_GetTicks() - tick_start) * 256 / fade_time;

        if (image_alpha > 255)
            image_alpha = 255;

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        //SDL_SetAlpha(sel_surface, SDL_SRCALPHA, image_alpha);
        SDL_SetAlpha(image, SDL_SRCALPHA, 255 - image_alpha);
        SDL_BlitSurface(image, &src, screen, &dest);
        SDL_FillRect(screen, &sel[0], 0); //SDL_MapRGB(surface->format, 0, 0, 0));
        SDL_BlitSurface(image, &sel[0], screen, &sel[selection]);
        //SDL_BlitSurface(sel_surface, &sel_src, screen, &sel[selection]);
        SDL_Flip(screen);
        SDL_Delay(1);
    }

    return (curlevel);

}

int enterpassword(){
    int retval;
    char code[] = "____";
    int i;
    SDL_Event event;
    char tmpchar;

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_Flip(screen);

    SDL_Print_Text("CODE", 111, 80, 0);

    for (i = 0; i < 4; ) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return (-1);
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return (-1);
                }

                if ((event.key.keysym.unicode & 0xFF80) == 0) {
                    tmpchar = (char)(event.key.keysym.unicode & 0x007F);

                    if ((tmpchar >= CHAR_0) && (tmpchar <= CHAR_9))
                        code[i++] = tmpchar;

                    if ((tmpchar >= CHAR_a) && (tmpchar <= CHAR_f))
                        tmpchar -= (CHAR_a - CHAR_A);

                    if ((tmpchar >= CHAR_A) && (tmpchar <= CHAR_F))
                        code[i++] = tmpchar;

                }
            }
        }
        SDL_Print_Text(code, 159, 80, 0);
        SDL_Flip(screen);
    }

    for (i = 0; i < levelcount; i++) {
        if (strcmp (code, levelcode[i]) == 0) {
            SDL_Print_Text("LEVEL", 103, 104, 0);
            sprintf(code, "%d", i + 1);
            SDL_Print_Text(code, 199 - 8 * strlen(code), 104, 0);
            SDL_Flip(screen);
            retval = waitforbutton();

            if (retval < 0)
                return retval;

            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
            SDL_Flip(screen);

            return (i + 1);
        }
    }

    SDL_Print_Text("!  WRONG CODE  !", 87, 104, 0);
    SDL_Flip(screen);
    retval = waitforbutton();

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_Flip(screen);
    return (retval);
}
