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

void studioTick(void);


#define playSounds true

#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "lba.h"

#ifdef USE_SDL_MIXER
#include "SDL_Mixer.h"
#endif

#ifdef GAME_DEBUG
#include <SDL_ttf.h>
#endif

//#include "SDL_rotozoom.h"
//#include "SDL_gfxPrimitives.h"

#define WTIME 10

#ifdef WTIME
#ifdef _WIN32
#include <sys/timeb.h>
#include <windows.h>
typedef unsigned _int64 uTime_t;
#else
#include <sys/time.h>
typedef unsigned long long uTime_t;
#endif
#ifndef _EE
uTime_t origin;

uTime_t fetch_time() {
	uTime_t r;

#ifdef _WIN32
	struct timeb t;

	ftime(&t);
	r = (t.time * 1000) + t.millitm;
#else
	struct timeval t;

	gettimeofday(&t, NULL);
	r = (t.tv_sec * 1000) + t.tv_usec / 1000;
#endif

	return r;
}

uTime_t diff_time(uTime_t origin) {
	uTime_t current = fetch_time();

	return current - origin;
}
#endif

#endif

#ifndef USE_GL
#include "osystem.h"

int osystem_mouseRight;
int osystem_mouseLeft;
int fullscreen;

char *tempBuffer;
SDL_Surface *sdl_buffer;
SDL_Surface *sdl_buffer320x200;
SDL_Surface *sdl_bufferStretch;
SDL_Surface *sdl_bufferRGBA;
SDL_Surface *sdl_screen;  // that's the SDL global object for the screen
SDL_Color sdl_colors[256];
SDL_Surface *surfaceTable[16];

#ifdef GAME_DEBUG
TTF_Font *font;
#endif

void osystem_mainLoop(void) {
#define SPEED 15              /* Ticks per Frame */
#define SLEEP_MIN 2          /* Minimum time a sleep takes, usually 2*GRAN */
#define SLEEP_GRAN 1         /* Granularity of sleep */

//int frames=0;                   /* Number of frames displayed */
#ifndef _EE
	long int t_start, t_left;
	long unsigned int t_end;
	long int q = 0;                   /* Dummy */
#endif

	while (!breakmainLoop) { // To be able to quit the game ;)
#ifndef _EE
		t_start = SDL_GetTicks();
		/*
		#ifdef WTIME
		if (diff_time(origin) < WTIME)
		{
		#ifdef _WIN32
		#ifndef FASTDEBUG
		  SDL_Delay((Uint32)(WTIME - diff_time(origin)));
		#endif
		#else
		  usleep((WTIME - diff_time(origin)) * 1000);
		#endif
		}
		origin = fetch_time();
		#endif*/
#endif

		mainLoopInteration();
#ifndef _EE
		t_end = t_start + SPEED;
		t_left = t_start - SDL_GetTicks() + SPEED;

		if (t_left > 0) {
			if (t_left > SLEEP_MIN) {
				SDL_Delay(t_left - SLEEP_GRAN);
			}
			while (SDL_GetTicks() < t_end) {
				q++;
			};
		} else {
			// printf("CPU to slow by %d ticks/round\n",-t_left);
		};
#endif
		lba_time++;
	}
}

void osystem_delay(int time) {
//   SDL_Delay(time);
}

void osystem_updateImage() {
}

void osystem_getMouseStatus(mouseStatusStruct * mouseData) {

	SDL_GetMouseState(&mouseData->X, &mouseData->Y);

	mouseData->left = osystem_mouseLeft;
	mouseData->right = osystem_mouseRight;

	osystem_mouseLeft = 0;
	osystem_mouseRight = 0;
}

int osystem_init(int argc, char *argv[])  // that's the constructor of the system dependent
// object used for the SDL port
{
	/*    int rendersolid = 0;

	    int rendertype = 0; */

#ifdef GAME_DEBUG
	int renderstyle = 0;
	int ptsize = 11;
#endif
	unsigned char *keyboard;
	int size;
	int i;

	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	fullscreen = 0;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

// 	{
// 		SDL_version compile_version, *link_version;
// 		SDL_VERSION(&compile_version);
// 		printf("compiled with SDL version: %d.%d.%d\n",
// 			compile_version.major,
// 			compile_version.minor,
// 			compile_version.patch);
// 		link_version = SDL_Linked_Version();
// 		printf("running with SDL version: %d.%d.%d\n",
// 			link_version->major,
// 			link_version->minor,
// 			link_version->patch);
// 	}

	atexit(SDL_Quit);

#ifdef GAME_DEBUG

	if (TTF_Init() < 0) {
		fprintf(stderr, "Couldn't initialize TTF: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(TTF_Quit);

	font = TTF_OpenFont("verdana.ttf", ptsize);

	if (font == NULL) {
		fprintf(stderr, "Couldn't load %d pt font from %s: %s\n", ptsize, "verdana.ttf", SDL_GetError());
		exit(2);
	}

	TTF_SetFontStyle(font, renderstyle);

#endif

	if (playSounds) {
#ifdef USE_SDL_MIXER
		SDL_version compile_version, *link_version;
		MIX_VERSION(&compile_version);
		printf("compiled with SDL_mixer version: %d.%d.%d\n",
			   compile_version.major,
			   compile_version.minor,
			   compile_version.patch);
		link_version = Mix_Linked_Version();
		printf("running with SDL_mixer version: %d.%d.%d\n",
			   link_version->major,
			   link_version->minor,
			   link_version->patch);

		// start SDL with audio support
		if (SDL_Init(SDL_INIT_AUDIO) == -1) {
			printf("SDL_Init: %s\n", SDL_GetError());
			exit(1);
		}
		// open 44.1KHz, signed 16bit, system byte order,
		//      stereo audio, using 1024 byte chunks
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
			printf("Mix_OpenAudio: %s\n", Mix_GetError());
			exit(2);
		}

		Mix_AllocateChannels(16);
#endif
	}

	SDL_WM_SetCaption("Little Big Adventure: TwinEngine", "LBA");

	// SDL_ShowCursor (SDL_DISABLE);

	// SDL_EnableUNICODE (SDL_ENABLE); // not much used in fact

	SDL_PumpEvents();

	keyboard = SDL_GetKeyState(&size);

	keyboard[SDLK_RETURN] = 0;

	sdl_screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE/*|SDL_FULLSCREEN*/);

	if (sdl_screen == NULL) {
		fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
		exit(1);
	}

	for (i = 0; i < 16; i++) {
		surfaceTable[i] =
			SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32, rmask, gmask, bmask, 0);
	}

	osystem_mouseLeft = 0;
	osystem_mouseRight = 0;

	return 0;
}

void osystem_putpixel(int x, int y, int pixel) {
	int bpp = sdl_screen->format->BytesPerPixel;

	/*
	 * Here p is the address to the pixel we want to set
	 */
	Uint8 *p = (Uint8 *) sdl_screen->pixels + y * sdl_screen->pitch + x * bpp;

	*p = pixel;
}

void osystem_setColor(byte i, byte R, byte G, byte B) // cette fonction est vraiment vraiment
// tres tres lente...
{
	sdl_colors[i].r = R;
	sdl_colors[i].g = G;
	sdl_colors[i].b = B;

	SDL_SetColors(sdl_buffer, sdl_colors, i, 1);
}

void osystem_setPalette(byte * palette) {
	// int i;
	SDL_Color *sdl_colorsTemp = (SDL_Color *) palette;

	SDL_SetColors(sdl_buffer, sdl_colorsTemp, 0, 256);

	SDL_BlitSurface(sdl_buffer, NULL, sdl_screen, NULL);

	SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
}

void osystem_setPalette320x200(byte * palette) {
	// int i;
	SDL_Color *sdl_colorsTemp = (SDL_Color *) palette;

	SDL_SetColors(sdl_buffer320x200, sdl_colorsTemp, 0, 256);

//    SDL_BlitSurface(sdl_buffer320x200, NULL, sdl_screen, NULL);

	//  SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
}

void osystem_fadeBlackToWhite() {
	int i;

	SDL_Color colorPtr[256];

	SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);

	for (i = 0; i < 256; i += 3) {
		memset(colorPtr, i, 1024);
		SDL_SetPalette(sdl_screen, SDL_PHYSPAL, colorPtr, 0, 256);
	}
}

void osystem_Flip(unsigned char *videoBuffer) {
	SDL_BlitSurface(sdl_buffer, NULL, sdl_screen, NULL);

	SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
}

void osystem_draw320x200BufferToScreen(unsigned char *videoBuffer) {
	SDL_BlitSurface(sdl_buffer320x200, NULL, sdl_bufferRGBA, NULL);

//  sdl_bufferStretch=zoomSurface(sdl_bufferRGBA, 2, 2.4, SMOOTHING_ON);

//  SDL_FillRect(sdl_screen,NULL,0);

	SDL_BlitSurface(sdl_buffer320x200, NULL, sdl_screen, NULL);

	SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);

	//SDL_FreeSurface(sdl_bufferStretch);
}

void osystem_CopyBlockPhys(unsigned char *videoBuffer, int left, int top, int right, int bottom) {
	SDL_Rect rectangle;

	rectangle.x = left;
	rectangle.y = top;
	rectangle.w = right - left + 1 ;
	rectangle.h = bottom - top + 1 ;

	SDL_BlitSurface(sdl_buffer, &rectangle, sdl_screen, &rectangle);

	SDL_UpdateRect(sdl_screen, left, top, right - left + 1, bottom - top + 1);
}

void osystem_initVideoBuffer(char *buffer, int width, int height) {
	Uint32 rmask, gmask, bmask, amask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	sdl_bufferRGBA = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 200, 32, rmask, gmask, bmask, amask);
	sdl_buffer320x200 = SDL_CreateRGBSurfaceFrom(buffer, width, height, 8, 320, 0, 0, 0, 0);
}

void osystem_initBuffer(char *buffer, int width, int height) {
	sdl_buffer = SDL_CreateRGBSurfaceFrom(buffer, width, height, 8, 640, 0, 0, 0, 0);
}

void osystem_crossFade(char *buffer, char *palette) {
#ifndef FASTDEBUG
	int i;
#endif
	SDL_Surface *backupSurface;
	SDL_Surface *newSurface;
	SDL_Surface *tempSurface;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	backupSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32, rmask, gmask, bmask, 0);
	newSurface =
		SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 640, 480, 32, rmask, gmask, bmask, 0);

	tempSurface = SDL_CreateRGBSurfaceFrom(buffer, 640, 480, 8, 640, 0, 0, 0, 0);
	SDL_SetColors(tempSurface, (SDL_Color *) palette, 0, 256);

	SDL_BlitSurface(sdl_screen, NULL, backupSurface, NULL);
	SDL_BlitSurface(tempSurface, NULL, newSurface, NULL);

#ifndef FASTDEBUG


	/*    for (i = 0; i < 16; i++)
	  {
	      SDL_BlitSurface(backupSurface, NULL, surfaceTable[i], NULL);
	      SDL_SetAlpha(newSurface, SDL_SRCALPHA | SDL_RLEACCEL, i * 16);
	      SDL_BlitSurface(newSurface, NULL, surfaceTable[i], NULL);
	  }

	    for (i = 0; i < 16; i++)
	  {
	      SDL_BlitSurface(surfaceTable[i], NULL, sdl_screen, NULL);
	      SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
	  }*/

	for (i = 0; i < 8; i++) {
		SDL_BlitSurface(backupSurface, NULL, surfaceTable[i], NULL);
		SDL_SetAlpha(newSurface, SDL_SRCALPHA | SDL_RLEACCEL, i * 32);
		SDL_BlitSurface(newSurface, NULL, surfaceTable[i], NULL);
		SDL_BlitSurface(surfaceTable[i], NULL, sdl_screen, NULL);
		SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
		SDL_Delay(20);

	}

#endif

	SDL_BlitSurface(newSurface, NULL, sdl_screen, NULL);
	SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);

	SDL_FreeSurface(backupSurface);
	SDL_FreeSurface(newSurface);
	SDL_FreeSurface(tempSurface);
}

#ifdef GAME_DEBUG
void osystem_drawText(int X, int Y, char *string) {
	SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };
	SDL_Color black = { 0x00, 0x00, 0x00, 0 };
	SDL_Color *forecol = &black;
	SDL_Rect rectangle;

	SDL_Surface *text;

	text = TTF_RenderText_Solid(font, string, *forecol);

	rectangle.x = X;
	rectangle.y = Y - 2;
	rectangle.w = text->w;
	rectangle.h = text->h;

	SDL_BlitSurface(text, NULL, sdl_buffer, &rectangle);
	SDL_FreeSurface(text);
}

void osystem_drawTextColor(int X, int Y, char *string, unsigned char r, unsigned char g, unsigned char b) {
	if (strcmp(string, "")) {
		SDL_Surface *text;
		SDL_Color forecol;
		SDL_Color white = { 0, 0, 0xFF, 0 };
		SDL_Rect rectangle;

		forecol.r = r;
		forecol.g = g;
		forecol.b = b;
		forecol.unused = 0;

		text = TTF_RenderText_Solid(font, string, forecol);

		rectangle.x = X;
		rectangle.y = Y - 2;
		rectangle.w = text->w;
		rectangle.h = text->h;

		SDL_BlitSurface(text, NULL, sdl_buffer, &rectangle);
		//SDL_FreeSurface(text);
	}
}
#endif

void osystem_drawLine(int X1, int Y1, int X2, int Y2, unsigned char color, unsigned char* palette) {
	Uint32 colorRGBA;

	palette += color * 3;
	colorRGBA = *(Uint32*)palette;
	colorRGBA |= 0xFF;

// lineColor(sdl_buffer, X1, Y1, X2, Y2, colorRGBA);
}

void osystem_FlaPCXCrossFade(SDL_Surface *image) {
	int i;
	SDL_Surface *backupSurface;
	SDL_Surface *newSurface;
	SDL_Surface *tempSurface;
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	backupSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32, rmask, gmask, bmask, 0);
	newSurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 640, 480, 32, rmask, gmask, bmask, 0);

	SDL_SetColors(sdl_screen, image->format->palette->colors, 0, 256);

	SDL_BlitSurface(sdl_screen, NULL, backupSurface, NULL);
	SDL_BlitSurface(tempSurface, NULL, newSurface, NULL);

	for (i = 0; i < 8; i++) {
		SDL_BlitSurface(backupSurface, NULL, surfaceTable[i], NULL);
		SDL_SetAlpha(newSurface, SDL_SRCALPHA | SDL_RLEACCEL, i * 32);
		SDL_BlitSurface(newSurface, NULL, surfaceTable[i], NULL);
		SDL_BlitSurface(surfaceTable[i], NULL, sdl_screen, NULL);
		SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
		SDL_Delay(20);
	}

	SDL_BlitSurface(newSurface, NULL, sdl_screen, NULL);
	SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);

	SDL_FreeSurface(backupSurface);
	SDL_FreeSurface(newSurface);
	SDL_FreeSurface(tempSurface);
}

void osystem_set320x200Mode(boolean mode) {
}

void osystem_startDisplayList() {
}

void osystem_stopDisplayList() {
}

void osystem_startPoly() {
}

void osystem_stopPoly() {
}

void osystem_addPointColor(int x, int y, int z, unsigned char color) {
}

void osystem_addLine(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char color) {
}

void osystem_addSphere(int x, int y, int z, int size, unsigned char color) {
}

void osystem_fullScreen() {
	SDL_FreeSurface(sdl_screen);
	sdl_screen = 0;

	fullscreen = 1 - fullscreen;

	if (fullscreen == 0) {
		sdl_screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
		SDL_SetColors(sdl_screen, palette, 0, 256);
		SDL_ShowCursor(1);
	} else {
		sdl_screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE | SDL_FULLSCREEN);

		SDL_SetColors(sdl_screen, palette, 0, 256);
		SDL_ShowCursor(0);
	}
	requestBackgroundRedraw = 1;
}

#endif
