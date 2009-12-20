/*
Copyright (C) 2002-2004 The TwinE team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "lba.h"
#include "SDL/SDL.h"

void readKeyboard(void) {
	SDL_Event event;
	int localKey;
	int i;
	int find = 0;
	short int temp;
	unsigned char temp2;
	char found = 0;
	int size;
	int j;
	unsigned char *keyboard;

	localKey = 0;

	printTextVar12 = 0;
	key1 = 0;
	skipIntro = 0;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: {
				exit(0);
				break;
			}
		case SDL_MOUSEBUTTONDOWN: {
				if (event.button.button == 3) {
					osystem_mouseRight = 1;
				} else {
					osystem_mouseLeft = 1;
				}
				break;
			}
		default: {
				break;
			}
		}
	}

	// case SDL_KEYDOWN:
	/*
	 * if(event.key.keysym.sym == SDLK_UP) printTextVar12|=2; if(event.key.keysym.sym == SDLK_DOWN)
	 * printTextVar12|=1; if(event.key.keysym.sym == SDLK_RETURN) key1=0;
	 */

	SDL_PumpEvents();

	keyboard = SDL_GetKeyState(&size);

	for (j = 0; j < size; j++) {
		if (keyboard[j]) {
			switch (j) {
			case SDLK_ESCAPE: {
					localKey = 0x1;
					break;
				}
			case SDLK_RETURN: {
					localKey = 0x1C;
					break;
				}
				/*      case SDLK_LSHIFT:
				        {
				          localKey = 0x2A;
				          break;
				        }*/
			case SDLK_LSHIFT:
			case SDLK_RSHIFT: {
					localKey = 0x36;
					break;
				}
			case SDLK_LALT:
			case SDLK_RALT: {
					localKey = 0x38;
					break;
				}
			case SDLK_SPACE: {
					localKey = 0x39;
					break;
				}
			case SDLK_UP:
			case SDLK_KP8: {
					localKey = 0x48;
					break;
				}
			case SDLK_DOWN:
			case SDLK_KP2: {
					localKey = 0x50;
					break;
				}
			case SDLK_LEFT:
			case SDLK_KP4: {
					localKey = 0x4B;
					break;
				}
			case SDLK_RIGHT:
			case SDLK_KP6: {
					localKey = 0x4D;
					break;
				}
			case SDLK_LCTRL:
			case SDLK_RCTRL: {
					localKey = 0x1D;
					break;
				}
			case SDLK_PAGEUP: {
					localKey = 0x49;
					break;
				}
			case SDLK_p: { // pause
					localKey = 'p';
					break;
				}
			case SDLK_h: { // holomap
					localKey = 'h';
					break;
				}
			case SDLK_j: {
					localKey = 'j';
					break;
				}
			case SDLK_w: { // Especial key to do the action
					localKey = 'w';
					break;
				}
			case SDLK_F1: {
					fkeys = 1; // F1
					break;
				}
			case SDLK_F2: {
					fkeys = 2; // F2
					break;
				}
			case SDLK_F3: {
					fkeys = 3; // F3
					break;
				}
			case SDLK_F4: {
					fkeys = 4; // F4
					break;
				}
			case SDLK_F6: {
					fkeys = 6; // F6
					break;
				}
			case SDLK_F12: {
					// fkeys = 12; // F12
					osystem_fullScreen();
					break;
				}

#ifdef GAME_DEBUG
			case SDLK_r: { // next room
					localKey = 'r';
					break;
				}
			case SDLK_f: { // previous room
					localKey = 'f';
					break;
				}
			case SDLK_t: { // increase story state
					localKey = 't';
					break;
				}
			case SDLK_g: { // decrease story state
					localKey = 'g';
					break;
				}
#endif
			}

			for (i = 0; i < 28; i++) {
				if (scanCodeTab1[i] == localKey) {
					find = i;
					found = 1;
				}
			}

			if (found != 0) {
				temp = scanCodeTab2[find];
				temp2 = temp & 0x00FF;

				if (temp2 == 0) {
					if (!(localKey & 0x80)) {
						printTextVar12 |= (temp & 0xFF00) >> 8;
					} else {
						printTextVar12 &= -((temp & 0xFF00) >> 8);
					}
				} else {
					// mode non géré ici
					key1 |= (temp & 0xFF00) >> 8;
				}
			} else {
				// no géré ici aussi
				skipIntro = localKey;
			}

			currentKey = event.key.keysym.scancode;
		}
	}

// printf("unicode= %X\n",event.key.keysym.unicode);
// printf("scancode= %X\n",skipIntro);
}

short int getKeyboardChar(void) {
	do {
		readKeyboard();
	} while (currentKey == 36);

	return (currentKey);
}
