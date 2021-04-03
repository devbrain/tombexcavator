
#include <SDL.h>

#include "ds_volume.h"
#include "data_source.h"
#include "sprite.h"

#include <iostream>
#include <string.h>
#include <stdio.h>


// ==========================================================
int main (int argc, char* argv [])
{
	ds::volume_c v ("c:/dos/demonstar/game.glb");

	const ds::fat_entry_c* fe = v.get ("S_MEGA2ICON");

	std::cout << fe->name () << " : " << fe->length () << std::endl;
	
	sprite_c s;
	s.load (fe);
	
	

	SDL_Surface *screen;
	screen = SDL_SetVideoMode(640, 480, 8, SDL_SWSURFACE);

    if ( screen == NULL ) {

        fprintf(stderr, "Zopa 640x480: %s\n", SDL_GetError());

        exit(1);

    }

	s.blit (screen, 0, 0);


	char ch;
	std::cin >> ch;
	return 0;
}
