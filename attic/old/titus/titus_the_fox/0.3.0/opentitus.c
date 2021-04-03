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
#include "backbuffer.h"
#include "viewimage.h"
#include "fonts.h"
#include "menu.h"
#include "engine.h"
#include "original.h"
#include "error.h"

int main(int argc, char *argv[]) {

    int retval;
    int keepplaying = 1;
    int curlevel = 0;

    retval = init();
    if (retval < 0)
        return retval;

    retval = viewintrotext();
    if (retval == -1)
        return 0;
    if (retval < 0)
        return retval;

    retval = viewimage(tituslogofile, tituslogoformat, 0, 4000);
    if (retval == -1)
        return 0;
    if (retval < 0)
        return retval;

    retval = viewimage(titusintrofile, titusintroformat, 0, 6500);
    if (retval == -1)
        return 0;
    if (retval < 0)
        return retval;

    while (keepplaying) {
        retval = viewmenu(titusmenufile, titusmenuformat);

        if (retval == -1)
            return 0;
        if (retval < 0)
            return retval;
        if (retval == 0)
            keepplaying = 0;
        if (retval <= levelcount) {
            retval = playtitus(retval - 1);
            if (retval == -1)
                return 0;
            if (retval < 0)
                return retval;
        }
    }

    return 0;
}

int init() {

    int retval;

    retval = readconfig(OPENTITUS_CONFIG_FILE);
    if (retval < 0)
        return retval;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    atexit(checkerror);
    atexit(SDL_Quit);
	
    switch (videomode) {
    case 0: //window mode
        screen = SDL_SetVideoMode(reswidth, resheight, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
        SDL_WM_SetCaption(OPENTITUS_WINDOW_TEXT, 0);
        break;
    case 1: //fullscreen
        screen = SDL_SetVideoMode(reswidth, resheight, bitdepth, SDL_DOUBLEBUF | SDL_FULLSCREEN);
        break;
    }

    if (screen == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return 1;
    }

    SDL_EnableUNICODE (1);

    initoriginal();

    initcodes();

    initleveltitles();

}

void checkerror(void) {
    printf(lasterror);
}
