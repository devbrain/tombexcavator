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

#ifdef USE_GL

#include "SDL.h"
#include "SDL_thread.h"
#include "lba.h"
#include "SDL_ttf.h"
#include "SDL_rotozoom.h"
#include "SDL_gfxPrimitives.h"

char *tempBuffer;
SDL_Surface *sdl_buffer;
SDL_Surface *sdl_buffer320x200;
SDL_Surface *sdl_bufferStretch;
SDL_Surface *sdl_bufferRGBA;
SDL_Surface *sdl_screen;  // that's the SDL global object for the screen
SDL_Color sdl_colors[256];
SDL_Surface *surfaceTable[16];
TTF_Font *font;
char* workBuffer;
GLuint    backTexture;
GLuint    modelsDisplayList;

GLUquadric* quad;

void OSystem::delay(int time) {
	SDL_Delay(time);
}

void OSystem::updateImage() {
	Flip(NULL);
}

void OSystem::getMouseStatus(mouseStatusStruct * mouseData) {

	SDL_GetMouseState(&mouseData->X, &mouseData->Y);

	mouseData->left = mouseLeft;
	mouseData->right = mouseRight;

	mouseLeft = 0;
	mouseRight = 0;
}

OSystem::OSystem(int argc, char *argv[])  // that's the constructor of the system dependent
// object used for the SDL port
{
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

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);

	if (TTF_Init() < 0) {
		fprintf(stderr, "Couldn't initialize TTF: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(TTF_Quit);

	int rendersolid = 0;
	int renderstyle = 0;
	int rendertype = 0;

	int ptsize = 11;

	font = TTF_OpenFont("verdana.ttf", ptsize);

	if (font == NULL) {
		fprintf(stderr, "Couldn't load %d pt font from %s: %s\n", ptsize, "verdana.ttf",
				SDL_GetError());
		exit(2);
	}

	TTF_SetFontStyle(font, renderstyle);

	SDL_WM_SetCaption("Little Big Adventure", "LBA");

	// SDL_ShowCursor (SDL_DISABLE);

	// SDL_EnableUNICODE (SDL_ENABLE); // not much used in fact

	SDL_PumpEvents();

	keyboard = SDL_GetKeyState(&size);

	keyboard[SDLK_RETURN] = 0;

	sdl_screen = SDL_SetVideoMode(800, 600, 32, SDL_OPENGL/*|SDL_FULLSCREEN*/);

	if (sdl_screen == NULL) {
		fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
		exit(1);
	}

	for (i = 0; i < 16; i++) {
		surfaceTable[i] =
			SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 32, rmask, gmask, bmask, 0);
	}

	mouseLeft = 0;
	mouseRight = 0;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);       // Black Background
	glMatrixMode(GL_PROJECTION);            // Select The Projection Matrix
	glLoadIdentity();                 // Reset The Projection Matrix
	glOrtho(0, 640, 480, 0, -100, 100);
	glMatrixMode(GL_MODELVIEW);             // Select The Modelview Matrix
	glLoadIdentity();                 // Reset The Modelview Matrix

	modelsDisplayList = glGenLists(1);

	quad = gluNewQuadric();

	glEnable(GL_DEPTH_TEST);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void OSystem::putpixel(int x, int y, int pixel) {
	int bpp = sdl_screen->format->BytesPerPixel;

	/*
	 * Here p is the address to the pixel we want to set
	 */
	Uint8 *p = (Uint8 *) sdl_screen->pixels + y * sdl_screen->pitch + x * bpp;

	*p = pixel;
}

void OSystem::setColor(byte i, byte R, byte G, byte B)  // cette fonction est vraiment vraiment
// tres tres lente...
{
	sdl_colors[i].r = R;
	sdl_colors[i].g = G;
	sdl_colors[i].b = B;

	SDL_SetColors(sdl_buffer, sdl_colors, i, 1);
}

void OSystem::setPalette(byte* localPalette) {
	int i;
	char *in = (char*)localPalette;
	char *out = (char*)palette;

	for (i = 0;i < 256;i++) {
		*(out++) = *(in++);
		*(out++) = *(in++);
		*(out++) = *(in++);
		in++;
	}
}

void OSystem::setPalette320x200(byte* localPalette) {
	int i;
	char *in = (char*)localPalette;
	char *out = (char*)palette;

	for (i = 0;i < 256;i++) {
		*(out++) = *(in++);
		*(out++) = *(in++);
		*(out++) = *(in++);
		in++;
	}
}

void OSystem::fadeBlackToWhite() {
	int i;

	SDL_Color colorPtr[256];

	SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);

	for (i = 0; i < 256; i += 3) {
		memset(colorPtr, i, 1024);
		SDL_SetPalette(sdl_screen, SDL_PHYSPAL, colorPtr, 0, 256);
	}
}

char tempBuffer2[1024*512*3];

void OSystem::Flip(unsigned char *videoBuffer) {
	glLoadIdentity();
	//glOrtho(0,640,480,0,1.0,100.0);
	glColor3ub(255, 255, 255);

	int i;
	char* in = workBuffer;
	char* out = tempBuffer2;

	/*glBegin(GL_TRIANGLES);
	  glTexCoord2f(0,0);
	  glVertex3i(0,0,-40.0f);
	  glTexCoord2f(640/1024,0);
	  glVertex3i(640,0,-40.0f);
	  glTexCoord2f(0,480/512);
	  glVertex3i(0,480,-40.0f);
	glEnd();*/

	/*  glDisable(GL_DEPTH_TEST);
	  glBindTexture(GL_TEXTURE_2D, backTexture);

	  glBegin(GL_TRIANGLES);

	    glTexCoord2f(0,0); // triangle haut gauche
	    glVertex3i(0,0,0.0f);
	    glTexCoord2f(640.f/1024.f,0);
	    glVertex3i(640,0,0.0f);
	    glTexCoord2f(0.0f,480.f/512.f);
	    glVertex3i(0,480,-0.0f);

	    glTexCoord2f(640.f/1024.f,0); // triangle haut gauche
	    glVertex3i(640,0,0.0f);
	    glTexCoord2f(640.f/1024.f,480.f/512.f);
	    glVertex3i(640,480,0.0f);
	    glTexCoord2f(0.0f,480.f/512.f);
	    glVertex3i(0,480,-0.0f);

	  glEnd();

	  glBindTexture(GL_TEXTURE_2D, 0);

	  glEnable(GL_DEPTH_TEST);
	  glCallList(modelsDisplayList);*/

//  SDL_GL_SwapBuffers( );
}

char tempBuffer3[640*480*3];

void OSystem::draw320x200BufferToScreen(unsigned char *videoBuffer) {
	int i;
	int j;
	char* in = (char*)videoBuffer;
	char* out = tempBuffer3;

	for (i = 0;i < 200;i++) {
		for (j = 0;j < 320;j++) {
			unsigned char color = *(in++);

			*(out++) = palette[color*3];
			*(out++) = palette[color*3+1];
			*(out++) = palette[color*3+2];
		}
	}

	glBindTexture(GL_TEXTURE_2D, backTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_RGB, GL_UNSIGNED_BYTE, tempBuffer3);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glColor3ub(255, 255, 255);

	//glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, backTexture);

	glBegin(GL_TRIANGLES);

	glTexCoord2f(0, 0); // triangle haut gauche
	glVertex3i(0, 0, 0.0f);
	glTexCoord2f(320.f / 1024.f, 0);
	glVertex3i(640, 0, 0.0f);
	glTexCoord2f(0.0f, 200.f / 512.f);
	glVertex3i(0, 480, -0.0f);

	glTexCoord2f(320.f / 1024.f, 0); // triangle haut gauche
	glVertex3i(640, 0, 0.0f);
	glTexCoord2f(320.f / 1024.f, 200.f / 512.f);
	glVertex3i(640, 480, 0.0f);
	glTexCoord2f(0.0f, 200.f / 512.f);
	glVertex3i(0, 480, -0.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_GL_SwapBuffers();
}
void OSystem::CopyBlockPhys(unsigned char *videoBuffer, int left, int top, int right, int bottom) {
	char* out = tempBuffer3;
	char* in = (char*)videoBuffer + left + top * 640;

	int i;
	int j;

	while ((right - left) % 4) {
		right++;
	}

	while ((bottom - top) % 4) {
		bottom++;
	}

	for (i = top;i < bottom;i++) {
		in = (char*)videoBuffer + left + i * 640;
		for (j = left;j < right;j++) {
			unsigned char color = *(in++);

			*(out++) = palette[color*3];
			*(out++) = palette[color*3+1];
			*(out++) = palette[color*3+2];
		}
	}


	glBindTexture(GL_TEXTURE_2D, backTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, left, top, right - left, bottom - top, GL_RGB, GL_UNSIGNED_BYTE, tempBuffer3);

}

void OSystem::initVideoBuffer(char *buffer, int width, int height) {
	memset(tempBuffer2, 0, 1024*512*3);
	glGenTextures(1, &backTexture);
	glBindTexture(GL_TEXTURE_2D, backTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBuffer2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int error = glGetError();

}

void OSystem::initBuffer(char *buffer, int width, int height) {
	sdl_buffer = SDL_CreateRGBSurfaceFrom(buffer, width, height, 8, 640, 0, 0, 0, 0);
	workBuffer = buffer;
}

void OSystem::crossFade(char *buffer, char *palette) {
	return;
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
	int i;

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

void OSystem::drawText(int X, int Y, char *string) {
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

void OSystem::drawTextColor(int X, int Y, char *string, unsigned char r, unsigned char g, unsigned char b) {
	SDL_Color forecol;
	SDL_Color white = { 0, 0, 0xFF, 0 };
	SDL_Rect rectangle;

	forecol.r = r;
	forecol.g = g;
	forecol.b = b;
	forecol.unused = 0;

	SDL_Surface *text;

	text = TTF_RenderText_Solid(font, string, forecol);

	rectangle.x = X;
	rectangle.y = Y - 2;
	rectangle.w = text->w;
	rectangle.h = text->h;

	SDL_BlitSurface(text, NULL, sdl_buffer, &rectangle);
	//  SDL_FreeSurface(text);
}

void OSystem::drawLine(int X1, int Y1, int X2, int Y2, unsigned char color, unsigned char* palette) {
	palette += color * 3;
	Uint32 colorRGBA = *(Uint32*)palette;
	colorRGBA |= 0xFF;

	lineColor(sdl_buffer, X1, Y1, X2, Y2, colorRGBA);
}

void OSystem::set320x200Mode(boolean mode) {
}

void OSystem::startDisplayList() {
	//glNewList(modelsDisplayList, GL_COMPILE);
}

void OSystem::stopDisplayList() {
	//glEndList();
}

void OSystem::startPoly() {
	glBegin(GL_POLYGON);
}
void OSystem::stopPoly() {
	glEnd();
}

void OSystem::addPointColor(float x, float y, float z, unsigned char color) {
	glColor3ub(palette[color*3], palette[color*3+1], palette[color*3+2]);
	glVertex3f(x, y, -z / 1000.f);
}

void OSystem::addLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned char color) {
	glBegin(GL_LINES);
	glColor3ub(palette[color*3], palette[color*3+1], palette[color*3+2]);
	glVertex3f(x1, y1, -z1 / 1000.f);
	glVertex3f(x2, y2, -z2 / 1000.f);
	glEnd();
}

void OSystem::addSphere(float x, float y, float z, float size, unsigned char color) {
	glColor3ub(palette[color*3], palette[color*3+1], palette[color*3+2]);
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPushMatrix();
	glTranslatef(x, y, -z / 1000.f);
	gluSphere(quad , size / 15.5f , 10 , 10);
	glPopMatrix();
}

/////////////////////////////// BRICKS MANAGEMENT /////////////////////////////////

#define BRICK_BUFFER_WIDTH 1024
#define BRICK_BUFFER_HEIGHT 1024

char brickBuffer[BRICK_BUFFER_WIDTH*BRICK_BUFFER_HEIGHT*4];

GLuint bricksTexture = 0;

int currentBrickX = 0;
int currentBrickY = 0;

struct brickEntry {
	char xOffset;
	char yOffset;
	char width;
	char height;

	int textureX;
	int textureY;

	float u1;
	float v1;
	float u2;
	float v2;
};

brickEntry brickTable[1000];

int currentBrick = 0;

int nextLineY = 0;

void OSystem::startBricks() {
	int i;
	int j;

	/*  for(i=0;i<BRICK_BUFFER_HEIGHT;i++)
	  {
	    for(j=0;j<BRICK_BUFFER_WIDTH;j++)
	    {
	      brickBuffer[i*BRICK_BUFFER_HEIGHT+j] = 0;
	      brickBuffer[i*BRICK_BUFFER_HEIGHT+j+1] = 0;
	      brickBuffer[i*BRICK_BUFFER_HEIGHT+j+2] = 0;
	      brickBuffer[i*BRICK_BUFFER_HEIGHT+j+3] = 255;
	    }
	  }*/

	memset(brickBuffer, 0, BRICK_BUFFER_HEIGHT*BRICK_BUFFER_WIDTH*4);

	currentBrick = 0;
	currentBrickX = 2;
	currentBrickY = 2;
	nextLineY = 0;
}

void OSystem::addBrickToBuffer(char* brickData) {
	int offsetX = *(brickData + 2);
	int offsetY = *(brickData + 3);
	int width = *brickData;
	int height = *(brickData + 1);

	int left = *(brickData + 2);
	int top = *(brickData + 3);
	int right = *brickData + left - 1;
	int bottom = *(brickData + 1) + top - 1;


	if (currentBrickX + offsetX + width > BRICK_BUFFER_WIDTH) { // is line change required ?
		currentBrickX = 2;
		currentBrickY += nextLineY + 2;
		nextLineY = 0;
	}

	brickTable[currentBrick].xOffset = offsetX;
	brickTable[currentBrick].yOffset = offsetY;
	brickTable[currentBrick].width = width;
	brickTable[currentBrick].height = height;
	brickTable[currentBrick].textureX = currentBrickX;
	brickTable[currentBrick].textureY = currentBrickY;

	brickTable[currentBrick].u1 = ((float)currentBrickX) / (float)BRICK_BUFFER_WIDTH;
	brickTable[currentBrick].v1 = ((float)currentBrickY) / (float)BRICK_BUFFER_HEIGHT;
	brickTable[currentBrick].u2 = (((float)currentBrickX + width)) / (float)BRICK_BUFFER_WIDTH;
	brickTable[currentBrick].v2 = (((float)currentBrickY + height)) / (float)BRICK_BUFFER_HEIGHT;

	currentBrick++;

	if (nextLineY < height) {
		nextLineY = height;
	}

	brickData += 4;
	char* ptr = brickData;

	right++;
	bottom++;

	char* outPtr = brickBuffer + (currentBrickY * BRICK_BUFFER_WIDTH * 4 + currentBrickX * 4);

	int offset = -((right - left) - BRICK_BUFFER_WIDTH);
	offset *= 4;

	int c1;
	int c2;
	int i;
	int iteration;
	int temp;

	for (c1 = 0; c1 < bottom - top; c1++) {
		int vc3 = *(ptr++);
		for (c2 = 0; c2 < vc3; c2++) {
			temp = *(ptr++);
			iteration = temp & 0x3F;
			if (temp & 0xC0) {
				iteration++;
				if (!(temp & 0x40)) {
					temp = (*(ptr++)) & 0xFF;
					for (i = 0; i < iteration; i++) {
						*(outPtr++) = palette[temp*3];
						*(outPtr++) = palette[temp*3+1];
						*(outPtr++) = palette[temp*3+2];
						*(outPtr++) = 255;
					}
				} else {
					for (i = 0; i < iteration; i++) {
						unsigned char color = *(ptr++);
						*(outPtr++) = palette[color*3];
						*(outPtr++) = palette[color*3+1];
						*(outPtr++) = palette[color*3+2];
						*(outPtr++) = 255;
					}
				}
			} else {
				outPtr += (iteration + 1) * 4;
			}
		}
		outPtr += offset;
	}

	currentBrickX += width + offsetX + 2;
}

void OSystem::finishBricks() {
	if (bricksTexture) {
		glDeleteTextures(1, &bricksTexture);
	}

	glGenTextures(1, &bricksTexture);

	glBindTexture(GL_TEXTURE_2D, bricksTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BRICK_BUFFER_WIDTH, BRICK_BUFFER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, brickBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if 0
	FILE* fileHandle;
	fileHandle = fopen("bricks.raw", "wb+");
	fwrite(brickBuffer, BRICK_BUFFER_WIDTH, BRICK_BUFFER_HEIGHT*4, fileHandle);
	fclose(fileHandle);
#endif
}

void OSystem::drawBrick(int brickNumber, int x, int y, int z) {
	float realX;
	float realY;
	float realZ;

	realX = (x - z) * 24 + 288; // x pos
	realY = ((x + z) * 12) - (y * 15) + 215;  // y pos
	realZ = -(z - x - y);

	realX += brickTable[brickNumber].xOffset;
	realY += brickTable[brickNumber].yOffset;

	glBindTexture(GL_TEXTURE_2D, bricksTexture);
	glColor4f(1.f, 1.f, 1.f, 1.f);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_EQUAL, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);

	glBegin(GL_QUADS);

	glTexCoord2f(brickTable[brickNumber].u1, brickTable[brickNumber].v1);
	glVertex2f(realX, realY);

	glTexCoord2f(brickTable[brickNumber].u2, brickTable[brickNumber].v1);
	glVertex2f(realX + brickTable[brickNumber].width, realY);

	glTexCoord2f(brickTable[brickNumber].u2, brickTable[brickNumber].v2);
	glVertex2f(realX + brickTable[brickNumber].width, realY + brickTable[brickNumber].height);

	glTexCoord2f(brickTable[brickNumber].u1, brickTable[brickNumber].v2);
	glVertex2f(realX, realY + brickTable[brickNumber].height);


	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}

void OSystem::startFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OSystem::stopFrame() {
	SDL_GL_SwapBuffers();
}


#endif

