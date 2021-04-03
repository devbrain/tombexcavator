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

#include "SDL/SDL.h"

int waitforbutton() {
    SDL_Event event;
    int waiting = 1;
    while (waiting > 0)
    {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                waiting = -1;

            if (event.type == SDL_KEYDOWN) {
                waiting = 0;
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    waiting = -1;
            }
        }
        SDL_Delay(1);
    }
    return (waiting);
}

