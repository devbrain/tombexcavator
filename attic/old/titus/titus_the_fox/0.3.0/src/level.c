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
#include "convert.h"

int loadlevel(TITUS_level *level, unsigned char *leveldata, int leveldatasize, SDL_Surface **sprites){
    int i;
    int j;
    unsigned int offset;
    unsigned int pre_size;

    level->player.inithp = 16;
    level->player.status = PLAYER_STATUS_EXIST;

    //Pre-malloc zeroing, to avoid overflow during a freelevel call in case of this function returns an error
    level->height = 0;
    level->objectcount = 0;
    level->enemycount = 0;
    level->elevatorcount = 0;

    pre_size = (leveldatasize - 35828) >> 8; //level->height
    level->width = 256;

    level->tilemap = (unsigned char **)SDL_malloc(sizeof(unsigned char *) * pre_size);
    if (level->tilemap == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }
    for (i = 0; i < pre_size; i++) {
        level->tilemap[i] = (unsigned char *)SDL_malloc(sizeof(unsigned char) * level->width);
        if (level->tilemap[i] == NULL) {
            sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
            return (ERROR_NOT_ENOUGH_MEMORY);
        }
        level->height++;
        for (j = 0; j < level->width; j++)
            level->tilemap[i][j] = leveldata[i * level->width + j];
    }


    level->pixelformat->palette->colors[14].r = orig_palette_level_colour[level->levelnumber].r;
    level->pixelformat->palette->colors[14].g = orig_palette_level_colour[level->levelnumber].g;
    level->pixelformat->palette->colors[14].b = orig_palette_level_colour[level->levelnumber].b;
    level->pixelformat->palette->colors[14].unused = orig_palette_level_colour[level->levelnumber].unused;

    offset = level->height * level->width;
    for (i = 0; i < 256; i++) {
        level->tile[i].tiledata = SDL_DisplayFormat(SDL_LoadTile(leveldata, offset + i * 128, level->pixelformat));
        level->tile[i].current = level->tile[i].tiledata;
        level->tile[i].horizflag = leveldata[offset + 32768 + i] & 0xFF;
        level->tile[i].floorflag = leveldata[offset + 32768 + 256 + i] & 0xFF;
        level->tile[i].ceilflag = leveldata[offset + 32768 + 512 + i] & 0x7F;
        if ((leveldata[offset + 32768 + 512 + i] & 0x80) == 0x80) {
            level->tile[i].animcycle = 1; //1-2-3
        } else if (i > 0) {
            if ((leveldata[offset + 32768 + 512 + i - 1] & 0x80) == 0x80) {
                level->tile[i].animcycle = 5; //4-5-6
            } else if (i > 1) {
                if ((leveldata[offset + 32768 + 512 + i - 2] & 0x80) == 0x80) {
                    level->tile[i].animcycle = 9; //7-8-9
                }
            }
        }
    }

    level->spritedata = sprites;

    level->player.initx = loadint16(leveldata[level->height * level->width + 33779], leveldata[level->height * level->width + 33778]);
    level->player.inity = loadint16(leveldata[level->height * level->width + 33781], leveldata[level->height * level->width + 33780]);

    level->player.sprite.spritedata = SDL_DisplayFormat(copysurface(level->spritedata[0], 0));
    if (level->player.sprite.spritedata == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }

    level->finishx = loadint16(leveldata[level->height * level->width + 35825], leveldata[level->height * level->width + 35824]);
    level->finishy = loadint16(leveldata[level->height * level->width + 35827], leveldata[level->height * level->width + 35826]);

    level->boss = NULL;
    level->finish_object = NULL;

    pre_size = 40; //level->objectcount

    level->object = (TITUS_object *)SDL_malloc(sizeof(TITUS_object) * pre_size);
    if (level->object == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }
    level->objectcount = pre_size;
    offset = level->height * level->width + 33536;
    for (i = 0; i < level->objectcount; i++) {
        level->object[i].sprite.number = loaduint16(leveldata[offset + i * 6 + 1], leveldata[offset + i * 6 + 0]);
        level->object[i].status = (level->object[i].sprite.number != 0xFFFF) ? OBJECT_STATUS_EXIST : OBJECT_STATUS_NONEXIST;
        level->object[i].initstatus = level->object[i].status;
        if (level->object[i].status == OBJECT_STATUS_EXIST) {
            level->object[i].sprite.supporting = ((level->object[i].sprite.number & 0x2000) == 0x2000);
            level->object[i].sprite.flash = ((level->object[i].sprite.number & 0x4000) == 0x4000);
            level->object[i].sprite.flipped = ((level->object[i].sprite.number & 0x8000) == 0x8000);
            level->object[i].sprite.number = level->object[i].sprite.number & 0x1FFF;
            level->object[i].sprite.spritedata = copysurface(level->spritedata[level->object[i].sprite.number], level->object[i].sprite.flipped);
            if (level->object[i].sprite.spritedata == NULL) {
                sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
                return (ERROR_NOT_ENOUGH_MEMORY);
            }
            level->object[i].initanimcycle = 0; //This needs to be checked... needed for the cage on the level with the big baby
            level->object[i].initx = loadint16(leveldata[offset + i * 6 + 3], leveldata[offset + i * 6 + 2]);
            level->object[i].inity = loadint16(leveldata[offset + i * 6 + 5], leveldata[offset + i * 6 + 4]);
        }
    }

    pre_size = 50; //level->enemycount

    level->enemy = (TITUS_enemy *)SDL_malloc(sizeof(TITUS_enemy) * pre_size);
    if (level->enemy == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }
    level->enemycount = pre_size;
    offset = level->height * level->width + 33782;
    for (i = 0; i < level->enemycount; i++) {
        level->enemy[i].sprite.number = loaduint16(leveldata[offset + 5], leveldata[offset + 4]);
        level->enemy[i].status = (level->enemy[i].sprite.number != 0xFFFF) ? ENEMY_STATUS_EXIST : ENEMY_STATUS_NONEXIST;
        level->enemy[i].initstatus = level->enemy[i].status;
        if (level->enemy[i].status == ENEMY_STATUS_EXIST) {
            level->enemy[i].sprite.supporting = ((level->enemy[i].sprite.number & 0x2000) == 0x2000); //Is this really necessary?
            level->enemy[i].sprite.flash = ((level->enemy[i].sprite.number & 0x4000) == 0x4000);
            level->enemy[i].sprite.flipped = ((level->enemy[i].sprite.number & 0x8000) == 0x8000);
            level->enemy[i].sprite.number = (level->enemy[i].sprite.number & 0x1FFF) + 101;
            level->enemy[i].sprite.spritedata = copysurface(level->spritedata[level->enemy[i].sprite.number], level->enemy[i].sprite.flipped);
            if (level->enemy[i].sprite.spritedata == NULL) {
                sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
                return (ERROR_NOT_ENOUGH_MEMORY);
            }
            level->enemy[i].initanimcycle = 0; //This needs to be checked...
            level->enemy[i].initx = loadint16(leveldata[offset + 1], leveldata[offset + 0]);
            level->enemy[i].inity = loadint16(leveldata[offset + 3], leveldata[offset + 2]);
            level->enemy[i].type = loaduint16(leveldata[offset + 7], leveldata[offset + 6]) & 0x1FFF;
            level->enemy[i].speedx = loadint16(leveldata[offset + 9], leveldata[offset + 8]);
            level->enemy[i].inithp = leveldata[offset + 10];
            level->enemy[i].power = loadint16(leveldata[offset + 12], leveldata[offset + 11]);

            switch (level->enemy[i].type) {
            case 0: //Noclip walk
            case 1: //Noclip walk
                level->enemy[i].centerx = loadint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].rangex = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                break;
            case 2: //Shoot
                level->enemy[i].delay = (unsigned int)leveldata[offset + 16] & 0xFF;
                level->enemy[i].rangex = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].direction = (level->enemy[i].rangex >> 14) & 0x0003;
                level->enemy[i].rangex = level->enemy[i].rangex & 0x3FFF;
                break;
            case 3: //Noclip walk, jump to player
            case 4: //Noclip walk, jump to player
                level->enemy[i].centerx = loadint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].rangex = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].rangey = (unsigned int)leveldata[offset + 19] & 0xFF;
                break;
            case 5: //Noclip walk, move to player
            case 6: //Noclip walk, move to player
                level->enemy[i].centerx = loadint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].rangex = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].rangey = (unsigned int)leveldata[offset + 19] & 0xFF;
                break;
            case 7: //Gravity walk, hit when near
                level->enemy[i].speedx = (int)leveldata[offset + 19] & 0xFF;
                level->enemy[i].rangex = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 8: //Gravity walk when off-screen
                level->enemy[i].speedx = (int)leveldata[offset + 19] & 0xFF;
                break;
            case 9: //Walk and periodically pop-up
                level->enemy[i].speedx = (int)leveldata[offset + 19] & 0xFF;
                level->enemy[i].rangex = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 10: //Alert when near, walk when nearer
                level->enemy[i].speedx = (int)leveldata[offset + 19] & 0xFF;
                level->enemy[i].rangex = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 11: //Walk and shoot
                level->enemy[i].speedx = (int)leveldata[offset + 19] & 0xFF;
                level->enemy[i].rangex = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 12: //Jump (immortal)
                level->enemy[i].rangey = loaduint16(leveldata[offset + 16], leveldata[offset + 15]);;
                level->enemy[i].delay = (unsigned int)leveldata[offset + 19] & 0xFF;
                break;
            case 13: //Bounce
                level->enemy[i].delay = (unsigned int)leveldata[offset + 20] & 0xFF;
                level->enemy[i].rangex = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 14: //Gravity walk when off-screen (immortal)
                level->enemy[i].speedx = (int)leveldata[offset + 19] & 0xFF;
                break;
            case 15: //Nothing (immortal)
                break;
            case 16: //Nothing
                break;
            case 17: //Drop (immortal)
                level->enemy[i].rangex = loaduint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].delay = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].rangey = loaduint16(leveldata[offset + 22], leveldata[offset + 21]);
                break;
            case 18: //Drop (immortal)
                level->enemy[i].rangex = loaduint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].rangey = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].speedy = leveldata[offset + 19];
                break;
            }
        }
        offset += 26;
    }

    level->bonuscount = 100;

    level->bonus = (TITUS_bonus *)SDL_malloc(sizeof(TITUS_bonus) * level->bonuscount);
    if (level->bonus == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }

    offset = level->height * level->width + 35082;
    for (i = 0; i < level->bonuscount; i++) {
        level->bonus[i].x = leveldata[offset + 2];
        level->bonus[i].y = leveldata[offset + 3];
        level->bonus[i].exists = ((level->bonus[i].x != 0xFF) & (level->bonus[i].y != 0xFF));
        if (level->bonus[i].exists) {
            level->bonus[i].replacetile = leveldata[offset + 1];
            level->tilemap[level->bonus[i].y][level->bonus[i].x] = leveldata[offset + 0]; //Overwrite the actual tile
        }
        offset += 4;
    }

    level->gatecount = 20;

    level->gate = (TITUS_gate *)SDL_malloc(sizeof(TITUS_gate) * level->gatecount);
    if (level->gate == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }

    offset = level->height * level->width + 35484;
    for (i = 0; i < level->gatecount; i++) {
        level->gate[i].entrancey = (unsigned int)leveldata[offset + 1] & 0xFF;
        level->gate[i].exists = (level->gate[i].entrancey != 0xFF);
        if (level->gate[i].exists) {
            level->gate[i].entrancex = (unsigned int)leveldata[offset + 0] & 0xFF;
            level->gate[i].screenx = (unsigned int)leveldata[offset + 2] & 0xFF;
            level->gate[i].screeny = (unsigned int)leveldata[offset + 3] & 0xFF;
            level->gate[i].exitx = (unsigned int)leveldata[offset + 4] & 0xFF;
            level->gate[i].exity = (unsigned int)leveldata[offset + 5] & 0xFF;
            level->gate[i].scrolling = leveldata[offset + 6];
        }
        offset += 7;
    }

    pre_size = 10; //level->elevatorcount

    level->elevator = (TITUS_elevator *)SDL_malloc(sizeof(TITUS_elevator) * pre_size);
    if (level->elevator == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }
    level->elevatorcount = pre_size;
    offset = level->height * level->width + 35624;
    for (i = 0; i < level->elevatorcount; i++) {
        level->elevator[i].sprite.number = loaduint16(leveldata[offset + 5], leveldata[offset + 4]);
        level->elevator[i].speed = leveldata[offset + 7];
        level->elevator[i].initx = loadint16(leveldata[offset + 13], leveldata[offset + 12]);
        level->elevator[i].inity = loadint16(leveldata[offset + 15], leveldata[offset + 14]);
        level->elevator[i].exists = ((level->elevator[i].sprite.number != 0xFFFF) && (level->elevator[i].speed < 8) && (level->elevator[i].x >= -16) && (level->elevator[i].y >= 0));
        if (level->elevator[i].exists) {
            level->elevator[i].sprite.number += 30;
            level->elevator[i].sprite.supporting = ((level->elevator[i].sprite.number & 0x2000) == 0x2000);
            level->elevator[i].sprite.flash = ((level->elevator[i].sprite.number & 0x4000) == 0x4000);
            level->elevator[i].sprite.flipped = ((level->elevator[i].sprite.number & 0x8000) == 0x8000);
            level->elevator[i].sprite.number = (level->elevator[i].sprite.number & 0x1FFF);
            level->elevator[i].sprite.spritedata = copysurface(level->spritedata[level->elevator[i].sprite.number], level->elevator[i].sprite.flipped);
            if (level->elevator[i].sprite.spritedata == NULL) {
                sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
                return (ERROR_NOT_ENOUGH_MEMORY);
            }
            level->elevator[i].range = loaduint16(leveldata[offset + 11], leveldata[offset + 10]);
            level->elevator[i].direction = (unsigned int)leveldata[offset + 16] & 0xFF;
        }
        offset += 20;
    }

    offset = level->height * level->width + 33776;
    level->vscrollimit = loadint16(leveldata[offset + 1], leveldata[offset + 0]) + 12;

    offset = level->height * level->width + 35482;
    level->hscrollimit = loadint16(leveldata[offset + 1], leveldata[offset + 0]) + 20;

    return (0);
}

int freelevel(TITUS_level *level){
    int i;

    for (i = 0; i < level->height; i++)
        free (level->tilemap[i]);

    free (level->tilemap);

    for (i = 0; i < 256; i++)
        SDL_FreeSurface(level->tile[i].tiledata);

    SDL_FreeSurface(level->player.sprite.spritedata);

    for (i = 0; i < level->objectcount; i++) {
        if (level->object[i].status != OBJECT_STATUS_NONEXIST)
            SDL_FreeSurface(level->object[i].sprite.spritedata);
    }

    free (level->object);

    for (i = 0; i < level->enemycount; i++)
        if (level->enemy[i].status != ENEMY_STATUS_NONEXIST)
            SDL_FreeSurface((level->enemy[i]).sprite.spritedata);

    free (level->enemy);

    free (level->bonus);

    free (level->gate);

    for (i = 0; i < level->elevatorcount; i++)
        if (level->elevator[i].exists)
            SDL_FreeSurface(level->elevator[i].sprite.spritedata);

    free (level->elevator);

    return (0);
}

int loadpixelformat(SDL_PixelFormat **pixelformat){
    int i;

    *pixelformat = (SDL_PixelFormat *)SDL_malloc(sizeof(SDL_PixelFormat));
    if (*pixelformat == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }

    (*pixelformat)->palette = (SDL_Palette *)SDL_malloc(sizeof(SDL_Palette));
    if ((*pixelformat)->palette == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }

    (*pixelformat)->palette->ncolors = 16;

    (*pixelformat)->palette->colors = (SDL_Color *)SDL_malloc(sizeof(SDL_Color) * (*pixelformat)->palette->ncolors);
    if ((*pixelformat)->palette->colors == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (ERROR_NOT_ENOUGH_MEMORY);
    }

    for (i = 0; i < (*pixelformat)->palette->ncolors; i++) {
        (*pixelformat)->palette->colors[i].r = orig_palette_colour[i].r;
        (*pixelformat)->palette->colors[i].g = orig_palette_colour[i].g;
        (*pixelformat)->palette->colors[i].b = orig_palette_colour[i].b;
        (*pixelformat)->palette->colors[i].unused = orig_palette_colour[i].unused;
    }

    (*pixelformat)->BitsPerPixel = 8;
    (*pixelformat)->BytesPerPixel = 1;

    (*pixelformat)->Rloss = 0;
    (*pixelformat)->Gloss = 0;
    (*pixelformat)->Bloss = 0;
    (*pixelformat)->Aloss = 0;

    (*pixelformat)->Rshift = 0;
    (*pixelformat)->Gshift = 0;
    (*pixelformat)->Bshift = 0;
    (*pixelformat)->Ashift = 0;

    (*pixelformat)->Rmask = 0;
    (*pixelformat)->Gmask = 0;
    (*pixelformat)->Bmask = 0;
    (*pixelformat)->Amask = 0;

    (*pixelformat)->colorkey = 0;
    (*pixelformat)->alpha = 255;

    return (0);
}

int freepixelformat(SDL_PixelFormat **pixelformat){
    free ((*pixelformat)->palette->colors);
    free ((*pixelformat)->palette);
    free (*pixelformat);
    return (0);
}
