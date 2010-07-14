#ifndef SPRITES_H
#define SPRITES_H

#include "SDL/SDL.h"

SDL_Surface * copysurface(SDL_Surface * original, char flip);
SDL_Surface * SDL_LoadSprite(unsigned char * first, char width, char height, unsigned int offset, SDL_PixelFormat * pixelformat);
int loadsprites(SDL_Surface ***sprites, char * spritedata, int spritedatasize, SDL_PixelFormat * pixelformat);
SDL_Surface * SDL_LoadTile(unsigned char * first, int i, SDL_PixelFormat * pixelformat);
int copypixelformat(SDL_PixelFormat * destformat, SDL_PixelFormat * srcformat);
int freesprites(SDL_Surface ***sprites);

#endif

