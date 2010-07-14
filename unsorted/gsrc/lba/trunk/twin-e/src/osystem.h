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

#include "SDL.h"

/*
class OSystem
{
  public:*/
int osystem_init(int argc, char *argv[]);

extern int osystem_mouseRight;
extern int osystem_mouseLeft;
extern int fullscreen; // meant to be bool, but set to int to ensure C compartibility

void osystem_mainLoop(void);
void osystem_delay(int time);
void osystem_crossFade(char *buffer, char *palette);
void osystem_FlaPCXCrossFade(SDL_Surface *image);
void osystem_fadeBlackToWhite();
void osystem_updateImage();
void osystem_initBuffer(char *buffer, int width, int height);
void osystem_initVideoBuffer(char *buffer, int width, int height);
void osystem_putpixel(int x, int y, int pixel);
void osystem_setColor(byte i, byte R, byte G, byte B);
void osystem_setPalette(byte * palette);
void osystem_setPalette320x200(byte * palette);
void osystem_Flip(unsigned char *videoBuffer);
void osystem_draw320x200BufferToScreen(unsigned char *videoBuffer);
void osystem_CopyBlockPhys(unsigned char *videoBuffer, int left, int top, int right, int bottom);
#ifdef GAME_DEBUG
void osystem_drawText(int X, int Y, char *text);
void osystem_drawTextColor(int X, int Y, char *string, unsigned char R, unsigned char G, unsigned char B);
#endif
void osystem_drawLine(int X1, int X2, int Y1, int Y2, unsigned char color, unsigned char* palette);
void osystem_getMouseStatus(mouseStatusStruct * mouseData);

void osystem_set320x200Mode(boolean mode);

void osystem_fullScreen();

//////////////////

void osystem_sendBackgroundTexture(unsigned char* videoBuffer);

void osystem_startDisplayList();
void osystem_stopDisplayList();
void osystem_startPoly();
void osystem_stopPoly();

#ifdef USE_GL
#ifdef PCLIKE
void osystem_addPointColor(float x, float y, float z, unsigned char color);
void osystem_addLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned char color);
void osystem_addSphere(float x, float y, float z, float size, unsigned char color);
#else
void osystem_addPointColor(int x, int y, int z, unsigned char color);
void osystem_addLine(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char color);
void osystem_addSphere(int x, int y, int z, int size, unsigned char color);
#endif
#else
void osystem_addPointColor(int x, int y, int z, unsigned char color);
void osystem_addLine(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char color);
void osystem_addSphere(int x, int y, int z, int size, unsigned char color);
#endif

#ifdef USE_GL
void osystem_startBricks();
void osystem_addBrickToBuffer(char* brickData);
void osystem_finishBricks();
void osystem_drawBrick(int brickNumber, int x, int y, int z);
void osystem_startFrame();
void osystem_stopFrame();
#endif

/* private:
};*/
