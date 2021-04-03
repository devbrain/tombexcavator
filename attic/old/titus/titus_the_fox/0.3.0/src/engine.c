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
#include "engine.h"
#include "error.h"
#include "original.h"
#include "sprites.h"
#include "level.h"
#include "keyboard.h"

static int playlevel(TITUS_level *level);
static int viewstatus(TITUS_level *level);
static int initlevel(TITUS_level *level);
static int displaylevel(TITUS_level *level);
static int movescreen(TITUS_level *level);
static int animate(TITUS_level *level);
static int movetitus(TITUS_level *level);

int playtitus(int firstlevel){
    int startx, starty;
    int retval;
    TITUS_level level;
    unsigned char *leveldata;
    SDL_Surface **sprites;
    unsigned char *spritedata;
    int leveldatasize;

    level.lives = 2;
    level.extrabonus = 0;

    retval = loadpixelformat(&(level.pixelformat));
    if (retval < 0) {
        freepixelformat(&(level.pixelformat));
        return retval;
    }

    retval = unSQZ(spritefile, &spritedata);
    if (retval < 0) {
        free (spritedata);
        freepixelformat(&(level.pixelformat));
        return retval;
    }
    retval = loadsprites(&sprites, spritedata, retval, level.pixelformat);
    free (spritedata);
    if (retval < 0) {
        freesprites(&sprites);
        freepixelformat(&(level.pixelformat));
        return retval;
    }

    for (level.levelnumber = firstlevel; level.levelnumber < levelcount; level.levelnumber++) {
        strcpy (level.title, leveltitle[level.levelnumber]);

        retval = unSQZ(levelfiles[level.levelnumber], &leveldata);
        if (retval < 0) {
            free (leveldata);
            freesprites(&sprites);
            freepixelformat(&(level.pixelformat));
            return retval;
        }
        leveldatasize = retval;

        retval = loadlevel(&level, leveldata, leveldatasize, sprites);
        free (leveldata);
        if (retval < 0) {
            freelevel (&level);
            freesprites(&sprites);
            freepixelformat(&(level.pixelformat));
            return retval;
        }

        while (level.player.status != PLAYER_STATUS_FINISHED) {

            retval = viewstatus(&level);
            if (retval < 0) {
                freelevel (&level);
                freesprites(&sprites);
                freepixelformat(&(level.pixelformat));
                return retval;
            }

            retval = initlevel (&level);

            retval = playlevel (&level);
            if (retval < 0) {
                freelevel (&level);
                freesprites(&sprites);
                freepixelformat(&(level.pixelformat));
                return retval;
            }

            if (level.player.status == PLAYER_STATUS_DEAD) {
                if (level.lives == 0) {
                    level.player.status = PLAYER_STATUS_GAMEOVER;
                } else {
                    level.lives--;
                }
            }

            if (level.player.status == PLAYER_STATUS_GAMEOVER) {
                freelevel (&level);
                freesprites(&sprites);
                freepixelformat(&(level.pixelformat));
                return 0;
            }

        }

        freelevel (&level);

        if (retval < 0) {
            freesprites(&sprites);
            freepixelformat(&(level.pixelformat));
            return retval;
        }

    }
    freesprites(&sprites);
    freepixelformat(&(level.pixelformat));
    if (game == 0) { //Titus
        retval = viewimage(titusfinishfile, titusfinishformat, 1, 0);
        if (retval < 0)
            return retval;
    }

    return (0);
}

static int playlevel(TITUS_level *level) {
    int status = 0;
    int retval = 0;
    SDL_Event event;
    while (status == 0) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                return ERROR_QUIT;

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE)
                    return ERROR_QUIT;

                if (event.key.keysym.sym == SDLK_F1) {
                    level->player.status = PLAYER_STATUS_DYING;
                    status = PLAYER_STATUS_DYING;
                    break;
                }

                if (event.key.keysym.sym == SDLK_F2) {
                    level->player.status = PLAYER_STATUS_GAMEOVER;
                    status = PLAYER_STATUS_GAMEOVER;
                    break;
                }

                if (event.key.keysym.sym == SDLK_F4) {
                    retval = viewstatus(level);
                    if (retval < 0)
                        return retval;
                }

                if (event.key.keysym.sym == SDLK_LEFT)
                    level->player.keyleft = 1;

                if (event.key.keysym.sym == SDLK_RIGHT)
                    level->player.keyright = 1;

                if (event.key.keysym.sym == SDLK_UP)
                    level->player.keyup = 1;

                if (event.key.keysym.sym == SDLK_DOWN)
                    level->player.keydown = 1;

                if (event.key.keysym.sym == SDLK_SPACE)
                    level->player.keyspace = 1;

                if (event.key.keysym.sym == SDLK_f) { //Finish event simulation
                    level->player.status = PLAYER_STATUS_FINISHED;
                    status = PLAYER_STATUS_FINISHED;
                    break;
                }

            }
            if (event.type == SDL_KEYUP) {

                if (event.key.keysym.sym == SDLK_LEFT)
                    level->player.keyleft = 0;

                if (event.key.keysym.sym == SDLK_RIGHT)
                    level->player.keyright = 0;

                if (event.key.keysym.sym == SDLK_UP)
                    level->player.keyup = 0;

                if (event.key.keysym.sym == SDLK_DOWN)
                    level->player.keydown = 0;

            }

        }
        //animate(level);
        //moveobjects(level);
        //moveenemies(level);
        //moveelevators(level);
        movetitus(level);
        movescreen(level);

        /*if (finished(level)) {
            level->player.status = PLAYER_STATUS_FINISHED;
            status = PLAYER_STATUS_FINISHED;
        }*/


        if (level->player.hp == 0) {
            level->player.status = PLAYER_STATUS_DYING;
            status = PLAYER_STATUS_DYING;
        }

        level->player.keyspace = 0;

        displaylevel(level);

        SDL_Delay(1);
    }

    if (status == PLAYER_STATUS_DYING) {
        level->player.status = PLAYER_STATUS_DEAD;
    }

    return (0);
}

static int viewstatus(TITUS_level *level){
    int retval;
    char tmpchars[10];
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    if (game == 0) { //Titus
        SDL_Print_Text("LEVEL", 13 * 8, 59, 0);
        SDL_Print_Text("EXTRA BONUS", 10 * 8, 10 * 12, 0);
        SDL_Print_Text("LIVES", 10 * 8, 11 * 12, 0);
    } else if (game == 1) { //Moktar
        SDL_Print_Text("ETAPE", 13 * 8, 59, 0);
        SDL_Print_Text("EXTRA BONUS", 10 * 8, 10 * 12, 0);
        SDL_Print_Text("VIE", 10 * 8, 11 * 12, 0);
    }


    sprintf(tmpchars, "%d", level->levelnumber + 1);
    SDL_Print_Text(tmpchars, 25 * 8 - strlen(tmpchars) * 8, 12 * 5, 0);

    SDL_Print_Text(leveltitle[level->levelnumber], 0, 12 * 5 + 16, 0);
    sprintf(tmpchars, "%d", level->extrabonus);
    SDL_Print_Text(tmpchars, 28 * 8 - strlen(tmpchars) * 8, 10 * 12, 0);

    sprintf(tmpchars, "%d", level->lives);
    SDL_Print_Text(tmpchars, 28 * 8 - strlen(tmpchars) * 8, 11 * 12, 0);

    SDL_Flip(screen);

    retval = waitforbutton();
    if (retval < 0)
        return retval;

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_Flip(screen);

    return (0);
}

static int initlevel(TITUS_level *level){
    int i;
    for (i = 0; i < level->objectcount; i++) {
        if (level->object[i].status != OBJECT_STATUS_NONEXIST) {
            level->object[i].x = level->object[i].initx;
            level->object[i].y = level->object[i].inity;
            level->object[i].animcycle = level->object[i].initanimcycle;
            level->object[i].status = level->object[i].initstatus;
        }
    }

    for (i = 0; i < level->enemycount; i++) {
        if (level->enemy[i].status != ENEMY_STATUS_NONEXIST) {
            level->enemy[i].x = level->enemy[i].initx;
            level->enemy[i].y = level->enemy[i].inity;
            level->enemy[i].animcycle = level->enemy[i].initanimcycle;
            level->enemy[i].status = level->enemy[i].initstatus;
            level->enemy[i].hp = level->enemy[i].inithp;
        }
    }

    for (i = 0; i < level->elevatorcount; i++) {
        if (level->elevator[i].exists) {
            level->elevator[i].x = level->elevator[i].initx;
            level->elevator[i].y = level->elevator[i].inity;
        }
    }

    level->player.x = level->player.initx;
    level->player.y = level->player.inity;
    level->player.hp = level->player.inithp;
    level->player.animcycle = 0;
    level->player.sprite.flipped = 0;
    level->player.status = PLAYER_STATUS_EXIST;
    level->player.keyleft = 0;
    level->player.keyright = 0;
    level->player.keyup = 0;
    level->player.keydown = 0;
    level->player.keyspace = 0;

    return (0);
}


static int displaylevel(TITUS_level *level){
    int i, j;
    SDL_Surface *image;
    SDL_Rect src, dest;

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    for (i = 0; i < level->height; i++) { //level sprites (tiles)
        for (j = 0; j < level->width; j++) {
            image = level->tile[level->tilemap[i][j]].current;

            src.x = 0;
            src.y = 0;
            src.w = image->w;
            src.h = image->h;
	
            dest.x = j * 16 - level->screenx;
            dest.y = i * 16 - level->screeny;
            dest.w = image->w;
            dest.h = image->h;

            SDL_BlitSurface(image, &src, screen, &dest);
        }
    }

    for (i = 9; i >= 0; i--) { //elevators
        if (level->elevator[i].exists) {
            image = level->elevator[i].sprite.spritedata;

            src.x = 0;
            src.y = 0;
            src.w = image->w;
            src.h = image->h;

            dest.x = level->elevator[i].x - level->screenx;
            dest.y = level->elevator[i].y - level->screeny;
            dest.w = image->w;
            dest.h = image->h;
            SDL_BlitSurface(image, &src, screen, &dest);
        }
    }

    for (i = 49; i >= 0; i--) { //enemies
        if ((level->enemy[i].status != ENEMY_STATUS_NONEXIST) && (level->enemy[i].status != ENEMY_STATUS_CARRIED) && (level->enemy[i].status != ENEMY_STATUS_DEAD)) {
            image = level->enemy[i].sprite.spritedata;

            src.x = 0;
            src.y = 0;
            src.w = image->w;
            src.h = image->h;

            dest.x = level->enemy[i].x - (image->w >> 1) - level->screenx;
            dest.y = level->enemy[i].y - image->h + 1 - level->screeny;
            dest.w = image->w;
            dest.h = image->h;

            SDL_BlitSurface(image, &src, screen, &dest);
        }
    }

    for (i = 39; i >= 0; i--) { //objects
        if ((level->object[i].status != OBJECT_STATUS_NONEXIST) && (level->object[i].status != OBJECT_STATUS_CARRIED) && (level->object[i].status != OBJECT_STATUS_GONE)) {
            image = level->object[i].sprite.spritedata;

            src.x = 0;
            src.y = 0;
            src.w = image->w;
            src.h = image->h;

            dest.x = level->object[i].x - (image->w >> 1) - level->screenx;
            dest.y = level->object[i].y - image->h + 1 - level->screeny;
            dest.w = image->w;
            dest.h = image->h;
            SDL_BlitSurface(image, &src, screen, &dest);
        }
    }

    image = level->player.sprite.spritedata; //Player

    src.x = 0;
    src.y = 0;
    src.w = image->w;
    src.h = image->h;
	
    dest.x = level->player.x - (image->w >> 1) - level->screenx;
    dest.y = level->player.y - image->h + 1 - level->screeny;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, &src, screen, &dest);

    dest.x = 0; //Draw a black rectangle below the "in-game resolution"
    dest.y = ingameheight;
    dest.w = screen->w;
    dest.h = screen->h - ingameheight;
    SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));

    dest.x = ingamewidth; //Draw a black rectangle to the right of the "in-game resolution"
    dest.y = 0;
    dest.w = screen->w - ingamewidth;
    dest.h = ingameheight;
    SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));

    SDL_Flip(screen);

    return (0);
}

static int movescreen(TITUS_level *level){

    if (level->player.x > (level->screenx + ingamewidth - 60))
        level->screenx = level->player.x - 60;

    if (level->player.y > (level->screeny + ingameheight - 60))
        level->screeny = level->player.y - 60;

    if (level->player.x < (level->screenx + 60))
        level->screenx = level->player.x - ingamewidth + 60;

    if (level->player.y < (level->screeny + 60))
        level->screeny = level->player.y - ingameheight + 60;

    if (((level->player.x + 8) <= level->hscrollimit * 16) && ((level->screenx + ingamewidth) > level->hscrollimit * 16))
        level->screenx = level->hscrollimit * 16 - ingamewidth;

    if ((level->player.y <= level->vscrollimit * 16) && ((level->screeny + ingameheight) > level->vscrollimit * 16))
        level->screeny = level->vscrollimit * 16 + 16 - ingameheight;

    if (level->screenx < 0)
        level->screenx = 0;

    if (level->screeny < 0)
        level->screeny = 0;

    if (level->screenx > (int)(level->width * 16 - ingamewidth))
        level->screenx = (int)(level->width * 16 - ingamewidth);

    if (level->screeny > (int)(level->height * 16 - ingameheight))
        level->screeny = (int)(level->height * 16 - ingameheight);

    return (0);
}

static int animate(TITUS_level *level){
    //level->player.sprite.spritedata = level->player


    return (0);
}

static int movetitus(TITUS_level *level){
    if (level->player.keyleft == 1) {
        level->player.x -= 2;
        if (level->player.sprite.flipped == 0) {
            level->player.sprite.flipped = 1;
            level->player.sprite.spritedata = SDL_DisplayFormat(copysurface(level->spritedata[0], level->player.sprite.flipped));
        }
    }

    if (level->player.keyright == 1) {
        level->player.x += 2;
        if (level->player.sprite.flipped == 1) {
            level->player.sprite.flipped = 0;
            level->player.sprite.spritedata = SDL_DisplayFormat(copysurface(level->spritedata[0], level->player.sprite.flipped));
        }
    }

    if (level->player.keyup == 1)
        level->player.y -= 2;

    if (level->player.keydown == 1)
        level->player.y += 2;

    return (0);
}
