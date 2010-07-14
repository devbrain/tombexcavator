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

#ifdef GAME_DEBUG

#include "lba.h"

boolean debugger_cubeClipEnabled;
int debugger_cubeClipX;
int debugger_cubeClipY;
int debugger_cubeClipZ;
//

actor* flagActor;

boolean bShowBoundingBoxes;

boolean bShowSpriteClip;

boolean bShowCubeChangeZones;
boolean bShowCameraZones;
boolean bShowScenaricZones;
boolean bShowGRMZones;
boolean bShowObjZones;
boolean bShowTextZones;
boolean bShowLadderZones;

boolean bShowActorNumbers;

boolean bShowFlags;

actorBoxStruct actorBox[256]; // up to 256 actor on screen
int debugger_numOfActorOnScreen;

winStruct debugger_windows[256];
int debugger_numOfWindows;

unsigned char colorTab[][3] = {
	{0x0, 0x0, 0x0}, // SCRIPT_COLOR_DEFAULT
	{0xFF, 0x0, 0x0}, // SCRIPT_COLOR_LABEL,
	{0, 247, 0}, // SCRIPT_COLOR_ANIM,
	{252, 255, 0}, // BUTTON_COLOR_TEXT,
	{243, 246, 0}, // BUTTON_COLOR_TEXT_ACTIVATED,
};


//int manipActorVar1 = 0;

void debugger_init(void) {
	debugger_cubeClipEnabled = false;
	debugger_cubeClipX = 64;
	debugger_cubeClipY = 25;
	debugger_cubeClipZ = 64;
}

int debugger_processDebug(void) {
	mouseStatusStruct mouseData;
	int actorNum;

//  freezeTime();

	osystem_getMouseStatus(&mouseData);

	debugger_numOfWindows = 0;

	if (mouseData.right) {
		debugger_debugMainMenu();
		fullRedraw(1);
	} else
		if (mouseData.left) {
			actorNum = debugger_findActor(mouseData.X, mouseData.Y);

			if (actorNum != -1) {
				debugger_debugActor(actorNum);
				fullRedraw(1);
			}
		}

//  unfreezeTime();

	return (0);
}

int debugger_findActor(int X, int Y) {
	int i;

	for (i = debugger_numOfActorOnScreen; i >= 0; i--) {
		if (debugger_inBox(X, Y, actorBox[i].top, actorBox[i].left, actorBox[i].bottom, actorBox[i].right)) {
			if (!(actorBox[i].top == 0 && actorBox[i].right == 639 && actorBox[i].left == 0 && actorBox[i].bottom == 479))
				return (actorBox[i].actorNum);
		}
	}

	return (-1);
}

int debugger_inBox(int X, int Y, int top, int left, int bottom, int right) {
	if (X > left && X < right)
		if (Y > top && Y < bottom)
			return (1);

	return (0);
}

void debugger_fillArea(int X, int Y, int width, int height) {
	int i, j;
	unsigned char *ptr;
	int offset;

	ptr = frontVideoBuffer + screenLockupTable[Y] + X;
	offset = 640 - (width);

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(ptr++) = 74;
		}
		ptr += offset;
	}

	// osystem_Flip(frontVideoBuffer);
	// osystem_CopyBlockPhys(frontVideoBuffer,0,0,640,480);

}

void debugger_fillArea2(int X, int Y, int width, int height, char color) {
	int i, j;
	unsigned char *ptr;
	int offset;

	ptr = frontVideoBuffer + screenLockupTable[Y] + X;
	offset = 640 - (width);

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(ptr++) = color;
		}
		ptr += offset;
	}

	// osystem_Flip(frontVideoBuffer);
	// osystem_CopyBlockPhys(frontVideoBuffer,0,0,640,480);

}

void debugger_addWin(int index, int X, int Y, int width, int height) {
	debugger_windows[index].X = X;
	debugger_windows[index].Y = Y;
	debugger_windows[index].Width = width;
	debugger_windows[index].Height = height;
	debugger_windows[index].numOfButtons = 0;

	debugger_numOfWindows++;
}

void debugger_addButtonNoColor(int winIndex, int X, int Y, int width, int height, char *text, buttonType type) {
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].X = X;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].Y = Y;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].Width = width;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].Height = height;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].type = type;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].color = 71;

	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].text = (char *) malloc(strlen(text) + 1);

	strcpy(debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].text, text);

	debugger_windows[winIndex].numOfButtons++;
}

void debugger_addButton(int winIndex, int X, int Y, int width, int height, char *text, buttonType type, short int boolVar) {
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].X = X;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].Y = Y;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].Width = width;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].Height = height;
	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].type = type;
	if (boolVar)
		debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].color = 100;
	else
		debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].color = 71;

	debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].text = (char *) malloc(strlen(text) + 1);

	strcpy(debugger_windows[winIndex].buttons[debugger_windows[winIndex].numOfButtons].text, text);

	debugger_windows[winIndex].numOfButtons++;
}

buttonType debugger_findButton(int X, int Y) {
	int i;
	int j;

	for (i = 0; i < debugger_numOfWindows; i++) {
		for (j = 0; j < debugger_windows[i].numOfButtons; j++) {
			if (X > (debugger_windows[i].X + debugger_windows[i].buttons[j].X)
					&& X < (debugger_windows[i].X + debugger_windows[i].buttons[j].X + debugger_windows[i].buttons[j].Width)
					&& Y > (debugger_windows[i].Y + debugger_windows[i].buttons[j].Y)
					&& Y < (debugger_windows[i].Y + debugger_windows[i].buttons[j].Y + debugger_windows[i].buttons[j].Height)) {
				return (debugger_windows[i].buttons[j].type);
			}
		}
	}

	return (NO_BUTTON);
}

buttonType debugger_processInput() {
	mouseStatusStruct mouseData;
	buttonType button;

	do {
		readKeyboard();
		osystem_getMouseStatus(&mouseData);

		if (mouseData.left) {
			button = debugger_findButton(mouseData.X, mouseData.Y);

			if (button != NO_BUTTON)
				return (button);
		}
	} while (1);
}

void debugger_drawAll() {
	int i;
	int j;

	for (i = 0; i < debugger_numOfWindows; i++) {
		int X;
		int Y;

		X = debugger_windows[i].X;
		Y = debugger_windows[i].Y;

		debugger_fillArea(X, Y, debugger_windows[i].Width, debugger_windows[i].Height);

		for (j = 0; j < debugger_windows[i].numOfButtons; j++) {
			unsigned char r;
			unsigned char g;
			unsigned char b;

			r = colorTab[BUTTON_COLOR_TEXT][0];
			g = colorTab[BUTTON_COLOR_TEXT][1];
			b = colorTab[BUTTON_COLOR_TEXT][2];

			if (debugger_windows[i].buttons[j].type != BUTTON_INFO)
				debugger_fillArea2(X + debugger_windows[i].buttons[j].X, Y + debugger_windows[i].buttons[j].Y, debugger_windows[i].buttons[j].Width, debugger_windows[i].buttons[j].Height, debugger_windows[i].buttons[j].color);

			osystem_drawTextColor(X + debugger_windows[i].buttons[j].X, Y + debugger_windows[i].buttons[j].Y, debugger_windows[i].buttons[j].text, r, g, b);
		}
	}
}

void debugger_debugCubeClip(void) {
	boolean bQuit = false;
	buttonType button;
	char string[255];

	do {
		debugger_numOfWindows = 0;
		debugger_addWin(0, 100, 100, 96, 56);
		debugger_addButton(0, 1, 1, 94, 10, "ON/OFF", BUTTON_CUBE_CLIP_TOGGLE, debugger_cubeClipEnabled);

		debugger_addButtonNoColor(0, 1, 12, 16, 10, "-", BUTTON_CUBE_CLIP_DEC_X);
		sprintf(string, "X: %d", debugger_cubeClipX);
		debugger_addButtonNoColor(0, 18, 12, 60, 10, string, BUTTON_GEN);
		debugger_addButtonNoColor(0, 79, 12, 16, 10, "+", BUTTON_CUBE_CLIP_INC_X);

		debugger_addButtonNoColor(0, 1, 23, 16, 10, "-", BUTTON_CUBE_CLIP_DEC_Y);
		sprintf(string, "Y: %d", debugger_cubeClipY);
		debugger_addButtonNoColor(0, 18, 23, 60, 10, string, BUTTON_GEN);
		debugger_addButtonNoColor(0, 79, 23, 16, 10, "+", BUTTON_CUBE_CLIP_INC_Y);

		debugger_addButtonNoColor(0, 1, 34, 16, 10, "-", BUTTON_CUBE_CLIP_DEC_Z);
		sprintf(string, "Z: %d", debugger_cubeClipZ);
		debugger_addButtonNoColor(0, 18, 34, 60, 10, string, BUTTON_GEN);
		debugger_addButtonNoColor(0, 79, 34, 16, 10, "+", BUTTON_CUBE_CLIP_INC_Z);

		debugger_addButtonNoColor(0, 1, 45, 94, 10, "OK", BUTTON_OK);

		debugger_drawAll();

		osystem_Flip(frontVideoBuffer);

		button = debugger_processInput();

		switch (button) {
		case BUTTON_OK: {
				bQuit = true;
				break;
			}
		case BUTTON_CUBE_CLIP_DEC_X: {
				if (debugger_cubeClipX > 0)
					debugger_cubeClipX--;
				break;
			}
		case BUTTON_CUBE_CLIP_INC_X: {
				if (debugger_cubeClipX < 64)
					debugger_cubeClipX++;
				break;
			}
		case BUTTON_CUBE_CLIP_DEC_Y: {
				if (debugger_cubeClipY > 0)
					debugger_cubeClipY--;
				break;
			}
		case BUTTON_CUBE_CLIP_INC_Y: {
				if (debugger_cubeClipY < 25)
					debugger_cubeClipY++;
				break;
			}
		case BUTTON_CUBE_CLIP_DEC_Z: {
				if (debugger_cubeClipZ > 0)
					debugger_cubeClipZ--;
				break;
			}
		case BUTTON_CUBE_CLIP_INC_Z: {
				if (debugger_cubeClipZ < 64)
					debugger_cubeClipZ++;
				break;
			}
		case BUTTON_CUBE_CLIP_TOGGLE: {
				if (debugger_cubeClipEnabled) {
					debugger_cubeClipEnabled = false;
				} else {
					debugger_cubeClipEnabled = true;
				}
				break;
			}
		default: {
				break;
			}
		}

		fullRedraw(1);

	} while (!bQuit);
}

void debugger_debugMainMenu(void) {
	boolean bQuit = false;
	buttonType button;

	do {
		debugger_numOfWindows = 0;
		debugger_addWin(0, 10, 10, 220, 300);

		debugger_addButtonNoColor(0, 20, 10, 140, 10, "   Cube Clip", BUTTON_CUBE_CLIP);

		debugger_addButtonNoColor(0, 20, 21, 140, 10, "Inventory Full", BUTTON_HAVE_ALL_ITEMS);

		debugger_addButton(0, 20, 32, 140, 10, "Show Bounding Box", BUTTON_SHOW_BOUNDING_BOXES, bShowBoundingBoxes);

		debugger_addButton(0, 20, 43, 140, 10, "Show Cube Zones", BUTTON_SHOW_CUBE_CHANGE_ZONES, bShowCubeChangeZones);
		debugger_addButton(0, 20, 54, 140, 10, "Show Camera Zones", BUTTON_SHOW_CAMERA_ZONES, bShowCameraZones);
		debugger_addButton(0, 20, 65, 140, 10, "Show Scenaric Zones", BUTTON_SHOW_SCENARIC_ZONES, bShowScenaricZones);
		debugger_addButton(0, 20, 76, 140, 10, "Show GRM Zones", BUTTON_SHOW_GRM_ZONES, bShowGRMZones);
		debugger_addButton(0, 20, 87, 140, 10, "Show Obj Zones", BUTTON_SHOW_OBJ_ZONES, bShowObjZones);
		debugger_addButton(0, 20, 98, 140, 10, "Show Text Zones", BUTTON_SHOW_TEXT_ZONES, bShowTextZones);
		debugger_addButton(0, 20, 109, 140, 10, "Show Ladder Zones", BUTTON_SHOW_LADDER_ZONES, bShowLadderZones);
		debugger_addButton(0, 20, 120, 140, 10, "Show Actors Number", BUTTON_SHOW_ACTORS_NUMBER, bShowActorNumbers);
		debugger_addButton(0, 20, 131, 140, 10, "Show Flags", BUTTON_SHOW_FLAGS, bShowFlags);

		debugger_addButton(0, 20, 142, 140, 10, "Show Sprite Clip", BUTTON_SHOW_SPRITE_CLIP, bShowSpriteClip);


		debugger_addButtonNoColor(0, 65, 285, 80, 10, "       OK", BUTTON_OK);

		debugger_drawAll();

		osystem_Flip(frontVideoBuffer);

		button = debugger_processInput();

		switch (button) {
		case BUTTON_OK: {
				bQuit = true;
				break;
			}
		case BUTTON_CUBE_CLIP: {
				fullRedraw(1);
				debugger_debugCubeClip();
				break;
			}
		case BUTTON_HAVE_ALL_ITEMS: {
				int i;
				for (i = 0;i <= 27;i++) {
					vars[i] = 1;
				}
				break;
			}
		case BUTTON_SHOW_BOUNDING_BOXES: {
				bShowBoundingBoxes ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_CUBE_CHANGE_ZONES: {
				bShowCubeChangeZones ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_CAMERA_ZONES: {
				bShowCameraZones ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_SCENARIC_ZONES: {
				bShowScenaricZones ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_GRM_ZONES: {
				bShowGRMZones ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_OBJ_ZONES: {
				bShowObjZones ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_TEXT_ZONES: {
				bShowTextZones ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_LADDER_ZONES: {
				bShowLadderZones ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_ACTORS_NUMBER: {
				bShowActorNumbers ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_FLAGS: {
				bShowFlags ^= 1;
				fullRedraw(1);
				break;
			}
		case BUTTON_SHOW_SPRITE_CLIP: {
				bShowSpriteClip ^= 1;
				fullRedraw(1);
				break;
			}
		default: {
				break;
			}
		}

	} while (!bQuit);
}

void debugger_debugActor(int num) {
	int i;
	buttonType button;
	int numOfLines;
	int startLineTrack = 0;
	int startLineCom = 0;
	int Y;
	char string[256];

	// printf("Debug actor %d\n",num);

	scriptData *comScript;
	scriptData *trackScript;

#ifndef TXT_SCRIPTS
	comScript = debugger_getActorComScript(num);
	trackScript = debugger_getActorTrackScript(num);
#else
	comScript = (scriptData*)malloc(sizeof(scriptData));
	trackScript = (scriptData*)malloc(sizeof(scriptData));

	comScript->numOfLignes = 0;
	trackScript->numOfLignes = 0;
#endif

	do {
		short int fallable;
		short int noshado;
		short int backgrd;
		short int carrier;
		short int zonable;
		short int objcol;
		short int brick;
//      short int no_col;
		short int clip;
		short int no_aff;
		short int miniZv;
		short int pushable;
//      short int codejeu;
//      short int no_choc;
//      short int noclip;

		short int money;
		short int life;
		short int magic;
		short int clove;
		short int key;

		objcol = actors[num].staticFlagsBF.bComputeCollisionWithObj;
		brick = actors[num].staticFlagsBF.bComputeCollisionWithBricks;
		zonable = actors[num].staticFlagsBF.bIsZonable;
		clip = actors[num].staticFlagsBF.bIsUsingClipping;
		pushable = actors[num].staticFlagsBF.bIsPushable;
		//0x20: dead
		//0x40: can drown
		//0x80: ?
		//0x100: ?
		no_aff = actors[num].staticFlagsBF.bNoDisplay;
		// 0x400: spriteActor
		fallable = actors[num].staticFlagsBF.bIsFallable;
		noshado = actors[num].staticFlagsBF.bDoesntCastShadow;
		backgrd = actors[num].staticFlagsBF.bIsBackgrounded;
		carrier = actors[num].staticFlagsBF.bIsCarrier;
		miniZv = actors[num].staticFlagsBF.bIsUsingMiniZv;

		money = actors[num].field_10 & 0x0010;
		life = actors[num].field_10 & 0x0020;
		magic = actors[num].field_10 & 0x0040;
		key = actors[num].field_10 & 0x0080;
		clove = actors[num].field_10 & 0x0100;

		debugger_numOfWindows = 0;
		debugger_addWin(0, 420, 145, 220, 300); // track win
		debugger_addButtonNoColor(0, 205, 15, 15, 140, "U", BUTTON_TRACK_UP);
		debugger_addButtonNoColor(0, 205, 156, 15, 140, "D", BUTTON_TRACK_DOWN);

		debugger_addWin(1, 0, 145, 418, 300); // comp win
		debugger_addButtonNoColor(1, 403, 15, 15, 140, "U", BUTTON_COMP_UP);
		debugger_addButtonNoColor(1, 403, 156, 15, 140, "D", BUTTON_COMP_DOWN);

		debugger_addWin(2, 0, 447, 640, 32);  // bottom status win
		debugger_addButtonNoColor(2, 0, 15, 640, 15, "OK", BUTTON_OK);

		debugger_addWin(3, 0, 0, 527, 80);  // status win
#ifdef TXT_SCRIPTS
		debugger_addButtonNoColor(3, 0, 0, 90, 15, actors[num].name, BUTTON_GEN);
#else
		debugger_addButtonNoColor(3, 0, 0, 90, 15, "Name", BUTTON_GEN);
#endif

		debugger_addButtonNoColor(3, 0, 16, 15, 15, "-", BUTTON_GEN);
		sprintf(string, "X: %d", actors[num].X);
		debugger_addButtonNoColor(3, 16, 16, 70, 15, string, BUTTON_GEN);
		debugger_addButtonNoColor(3, 87, 16, 15, 15, "+", BUTTON_GEN);

		debugger_addButtonNoColor(3, 0, 32, 15, 15, "-", BUTTON_GEN);
		sprintf(string, "Y: %d", actors[num].Y);
		debugger_addButtonNoColor(3, 16, 32, 70, 15, string, BUTTON_GEN);
		debugger_addButtonNoColor(3, 87, 32, 15, 15, "+", BUTTON_GEN);

		debugger_addButtonNoColor(3, 0, 48, 15, 15, "-", BUTTON_GEN);
		sprintf(string, "Z: %d", actors[num].Z);
		debugger_addButtonNoColor(3, 16, 48, 70, 15, string, BUTTON_GEN);
		debugger_addButtonNoColor(3, 87, 48, 15, 15, "+", BUTTON_GEN);

		debugger_addButtonNoColor(3, 103, 16, 40, 15, "Angle", BUTTON_INFO);
		debugger_addButtonNoColor(3, 144, 16, 15, 15, "-", BUTTON_GEN);
		sprintf(string, "%d", actors[num].angle);
		debugger_addButtonNoColor(3, 160, 16, 35, 15, string, BUTTON_GEN);
		debugger_addButtonNoColor(3, 196, 16, 15, 15, "+", BUTTON_GEN);

		debugger_addButtonNoColor(3, 103, 32, 40, 15, "Vitesse", BUTTON_INFO);
		debugger_addButtonNoColor(3, 144, 32, 15, 15, "-", BUTTON_GEN);
		sprintf(string, "%d", actors[num].speed);
		debugger_addButtonNoColor(3, 160, 32, 35, 15, string, BUTTON_GEN);
		debugger_addButtonNoColor(3, 196, 32, 15, 15, "+", BUTTON_GEN);

		debugger_addButtonNoColor(3, 103, 48, 40, 15, "Vie", BUTTON_INFO);
		debugger_addButtonNoColor(3, 144, 48, 15, 15, "-", BUTTON_GEN);
		sprintf(string, "%d", actors[num].life);
		debugger_addButtonNoColor(3, 160, 48, 35, 15, string, BUTTON_GEN);
		debugger_addButtonNoColor(3, 196, 48, 15, 15, "+", BUTTON_GEN);

		debugger_addButtonNoColor(3, 95, 64, 48, 15, "Armure", BUTTON_INFO);
		debugger_addButtonNoColor(3, 144, 64, 15, 15, "-", BUTTON_GEN);
		sprintf(string, "%d", actors[num].field_14);
		debugger_addButtonNoColor(3, 160, 64, 35, 15, string, BUTTON_GEN);
		debugger_addButtonNoColor(3, 196, 64, 15, 15, "+", BUTTON_GEN);

		debugger_addButton(3, 212, 0, 50, 15, "MONEY", BUTTON_GEN, money);
		debugger_addButton(3, 212, 16, 50, 15, "LIFE", BUTTON_GEN, life);
		debugger_addButton(3, 212, 32, 50, 15, "MAGIC", BUTTON_GEN, magic);
		debugger_addButton(3, 212, 48, 50, 15, "KEY", BUTTON_GEN, key);
		debugger_addButton(3, 212, 64, 50, 15, "CLOVE", BUTTON_GEN, clove);

		debugger_addButton(3, 263, 0, 65, 15, "FALLABLE", BUTTON_GEN, fallable);
		debugger_addButton(3, 263, 16, 65, 15, "NOSHADO", BUTTON_GEN, noshado);
		debugger_addButton(3, 263, 32, 65, 15, "BACKGRD", BUTTON_GEN, backgrd);
		debugger_addButton(3, 263, 48, 65, 15, "CARRIER", BUTTON_GEN, carrier);

		debugger_addButton(3, 329, 0, 65, 15, "ZONABLE", BUTTON_GEN, zonable);
		debugger_addButton(3, 329, 16, 65, 15, "OBJCOL", BUTTON_GEN, objcol);
		debugger_addButton(3, 329, 32, 65, 15, "BRICK", BUTTON_GEN, brick);
		debugger_addButtonNoColor(3, 329, 48, 65, 15, "NO_COL", BUTTON_GEN);
		debugger_addButton(3, 329, 64, 65, 15, "CLIP", BUTTON_GEN, clip);

		debugger_addButton(3, 395, 0, 65, 15, "NO_AFF", BUTTON_GEN, no_aff);
		debugger_addButton(3, 395, 16, 65, 15, "MINIZV", BUTTON_GEN, miniZv);
		debugger_addButton(3, 395, 32, 65, 15, "PUSHABLE", BUTTON_GEN, pushable);
		debugger_addButtonNoColor(3, 395, 48, 65, 15, "CODE JEU", BUTTON_GEN);
		debugger_addButtonNoColor(3, 395, 64, 65, 15, "NO_CHOC", BUTTON_GEN);

		debugger_addButtonNoColor(3, 461, 0, 65, 15, "CHG CODE", BUTTON_GEN);
		debugger_addButtonNoColor(3, 461, 16, 65, 15, "NOCLIP", BUTTON_GEN);
		debugger_addButtonNoColor(3, 461, 32, 65, 15, "ZBUFFER", BUTTON_GEN);
		debugger_addButtonNoColor(3, 461, 48, 65, 15, "MESSAGES", BUTTON_GEN);
		debugger_addButtonNoColor(3, 461, 64, 65, 15, "Free", BUTTON_GEN);

		debugger_addWin(4, 0, 82, 200, 61); // file win

		debugger_drawAll();

		numOfLines = 25;
		Y = 150;

		if (trackScript->numOfLignes < 25)
			numOfLines = trackScript->numOfLignes;

		for (i = startLineTrack; i < (numOfLines + startLineTrack); i++) {
			unsigned char r;
			unsigned char g;
			unsigned char b;

			r = colorTab[trackScript->lines[i].color][0];
			g = colorTab[trackScript->lines[i].color][1];
			b = colorTab[trackScript->lines[i].color][2];

			osystem_drawTextColor(425, Y, trackScript->lines[i].line, r, g, b);
			Y += 11;
		}

		numOfLines = 25;
		Y = 150;

		numOfLines = comScript->numOfLignes - startLineCom;
		if (numOfLines > 25)
			numOfLines = 25;

		for (i = startLineCom; i < (numOfLines + startLineCom); i++) {
			char buffer[50];
			sprintf(buffer, "%d:", comScript->lines[i].lineNumber);
			osystem_drawTextColor(5, Y, buffer, 0, 0, 0);
			Y += 11;
		}

		Y = 150;
		for (i = startLineCom; i < (numOfLines + startLineCom); i++) {
			unsigned char r;
			unsigned char g;
			unsigned char b;

			r = colorTab[comScript->lines[i].color][0];
			g = colorTab[comScript->lines[i].color][1];
			b = colorTab[comScript->lines[i].color][2];

			osystem_drawTextColor(50, Y, comScript->lines[i].line, r, g, b);
			Y += 11;
		}


		osystem_Flip(frontVideoBuffer);

		button = debugger_processInput();

		if (button == BUTTON_TRACK_DOWN && ((startLineTrack + 25) < trackScript->numOfLignes))
			startLineTrack++;

		if (button == BUTTON_TRACK_UP && (startLineTrack > 0))
			startLineTrack--;

		if (button == BUTTON_COMP_DOWN && ((startLineCom + 25) < comScript->numOfLignes)) {
			startLineCom += 25;
			if (startLineCom > comScript->numOfLignes)
				startLineCom = comScript->numOfLignes;
		}

		if (button == BUTTON_COMP_UP && (startLineCom > 0))
			startLineCom -= 25;

	} while (button != BUTTON_OK);
}

void debugger_addLine(char *buffer, scriptData * script) {
	script->lines = (scriptLineData *) realloc(script->lines, sizeof(scriptLineData) * (script->numOfLignes + 1));

	script->lines[script->numOfLignes].line = (char *) malloc(strlen(buffer) + 1);
	script->lines[script->numOfLignes].color = SCRIPT_COLOR_DEFAULT;
	script->lines[script->numOfLignes].indentation = 0;

	strcpy(script->lines[script->numOfLignes].line, buffer);
	script->numOfLignes++;
}

void debugger_addLineColor(char *buffer, scriptData * script, debuggerColor color) {
	script->lines = (scriptLineData *) realloc(script->lines, sizeof(scriptLineData) * (script->numOfLignes + 1));

	script->lines[script->numOfLignes].line = (char *) malloc(strlen(buffer) + 1);
	script->lines[script->numOfLignes].color = color;
	script->lines[script->numOfLignes].indentation = 0;

	strcpy(script->lines[script->numOfLignes].line, buffer);
	script->numOfLignes++;
}

scriptData *debugger_getActorTrackScript(int num) {
	unsigned char *scriptPtr;
	int finish = 0;
	unsigned char opcode;
	scriptData *script;
	char buffer[256];
//    int lineNumber;

	script = (scriptData *) malloc(sizeof(scriptData));

	script->lines = 0;
	script->numOfLignes = 0;

	scriptPtr = actors[num].moveScript;

	do {
		opcode = *(scriptPtr++);
		switch (opcode) {
		case 0: {
				sprintf(buffer, "END");
				debugger_addLine(buffer, script);
				finish = 1;
				break;
			}
		case 1: {
				debugger_addLine("NOP", script);
				break;
			}
		case 2: {
				sprintf(buffer, "BODY %d", *(scriptPtr++));
				debugger_addLine(buffer, script);
				break;
			}
		case 3: {
				sprintf(buffer, "ANIM %d", *(scriptPtr++));  // bleu
				debugger_addLineColor(buffer, script, SCRIPT_COLOR_ANIM);
				break;
			}
		case 4: {
				sprintf(buffer, "GOTO_POINT %d", *(scriptPtr++));
				debugger_addLine(buffer, script);
				break;
			}
		case 5: {
				sprintf(buffer, "WAIT_ANIM");
				debugger_addLine(buffer, script);
				break;
			}
		case 6: {
				debugger_addLine("LOOP", script);
				break;
			}
		case 7: {
				short int temp;

				temp = *(short int *)(scriptPtr);

				scriptPtr += 2;

				sprintf(buffer, "ANGLE %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 8: {
				sprintf(buffer, "POS_POINT %d", *(scriptPtr++));
				debugger_addLine(buffer, script);
				break;
			}
		case 9: {
				sprintf(buffer, "LABEL %d", (*scriptPtr++)); // rouge
				debugger_addLineColor(buffer, script, SCRIPT_COLOR_LABEL);
				break;
			}
		case 10: {
				short int temp;

				temp = *(short int *)(scriptPtr);

				scriptPtr += 2;

				temp = *(actors[num].moveScript + temp + 1);

				sprintf(buffer, "GOTO %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 11: {
				sprintf(buffer, "STOP");
				debugger_addLine(buffer, script);
				break;
			}
		case 12: {
				sprintf(buffer, "GOTO_SYM_POINT %d", *(scriptPtr++));
				debugger_addLine(buffer, script);
				break;
			}
		case 13: {
				sprintf(buffer, "WAIT_NB_ANIM %d %d", *(scriptPtr++), *(scriptPtr++));
				debugger_addLine(buffer, script);
				break;
			}
		case 14: {
				short int temp;

				temp = *(short int *)(scriptPtr);

				scriptPtr += 2;

				sprintf(buffer, "SAMPLE %d", temp);  // bleu
				debugger_addLine(buffer, script);
				break;
			}
		case 15: {
				sprintf(buffer, "GOTO_POINT_3D %d", *(scriptPtr++));
				debugger_addLine(buffer, script);
				break;
			}
		case 16: {
				short int temp;

				temp = *(short int *) scriptPtr;
				scriptPtr += 2;

				sprintf(buffer, "SPEED %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 17: {
				sprintf(buffer, "BACKGROUND %d", *(scriptPtr++));
				debugger_addLine(buffer, script);
				break;
			}
		case 18: {
				short int temp1;
				short int temp2;
				unsigned char temp3;

				temp3 = *(unsigned char *)(scriptPtr++);

				temp1 = *(int *)(scriptPtr);
				scriptPtr += 2;

				temp2 = *(int *)(scriptPtr);
				scriptPtr += 2;

				sprintf(buffer, "WAIT_NB_SECOND %d", temp3);
				debugger_addLine(buffer, script);
				break;
			}
		case 19: {
				sprintf(buffer, "NO_BODY");
				debugger_addLine(buffer, script);
				break;
			}
		case 20: {
				short int temp;

				temp = *(short int *) scriptPtr;

				scriptPtr += 2;

				sprintf(buffer, "BETA %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 21: {
				short int temp;

				temp = *(short int *)(scriptPtr);

				scriptPtr += 2;

				sprintf(buffer, "OPEN_LEFT %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 22: {
				short int temp;

				temp = *(short int *)(scriptPtr);

				scriptPtr += 2;

				sprintf(buffer, "OPEN_RIGHT %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 23: {
				short int temp;

				temp = *(short int *)(scriptPtr);

				scriptPtr += 2;

				sprintf(buffer, "OPEN_UP %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 24: {
				short int temp;

				temp = *(short int *)(scriptPtr);

				scriptPtr += 2;

				sprintf(buffer, "OPEN_DOWN %d", temp);
				debugger_addLine(buffer, script);
				break;
			}

		case 25: {
				sprintf(buffer, "CLOSE");
				debugger_addLine(buffer, script);
				break;
			}
		case 26: {
				sprintf(buffer, "WAIT_DOOR");
				debugger_addLine(buffer, script);
				break;
			}
		case 27: {
				short int temp;

				temp = *(short int *) scriptPtr;
				scriptPtr += 2;
				sprintf(buffer, "SAMPLE_RND %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 28: {
				short int temp;

				temp = *(short int *) scriptPtr;
				scriptPtr += 2;
				sprintf(buffer, "SAMPLE_ALWAYS %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 29: {
				short int temp;

				temp = *(short int *) scriptPtr;
				scriptPtr += 2;
				sprintf(buffer, "SAMPLE_STOP %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 30: {
				sprintf(buffer, "PLAY_FLA %s", scriptPtr);
				scriptPtr += strlen((char *) scriptPtr) + 1;
				debugger_addLine(buffer, script);
				break;
			}
		case 31: {
				short int temp;

				temp = *(short int *) scriptPtr;
				scriptPtr += 2;
				sprintf(buffer, "REPEAT_SAMPLE %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 32: {
				short int temp;

				temp = *(short int *) scriptPtr;
				scriptPtr += 2;
				sprintf(buffer, "SIMPLE_SAMPLE %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 33: {
				short int temp;

				temp = *(short int *) scriptPtr;
				scriptPtr += 2;
				sprintf(buffer, "FACE_TWINKEL %d", temp);
				debugger_addLine(buffer, script);
				break;
			}
		case 34: {
				short int temp1;
				short int temp2;

				temp1 = *(short int *) scriptPtr;
				scriptPtr += 2;
				temp2 = *(short int *) scriptPtr;
				scriptPtr += 2;
				sprintf(buffer, "ANGLE_RND %d %d", temp1, temp2);
				debugger_addLine(buffer, script);
				break;
			}
		default: {
				sprintf(buffer, "Unknown opcode %d", opcode);
				debugger_addLine(buffer, script);
				finish = 1;
				break;
			}
		}
	} while (!finish);

	return (script);
}

scriptData *debugger_getActorComScript(int num) {
	unsigned char *scriptPtr;
	boolean finish = false;
	boolean endComportement = false;
	int currentComportement = 0;
	unsigned char opcode;
	scriptData *script;
	char buffer[256];
	char buffer2[256];
	int currentLineNumber;

	script = (scriptData *) malloc(sizeof(scriptData));

	script->lines = 0;
	script->numOfLignes = 0;

	scriptPtr = actors[num].actorScript;

	do {
		endComportement = false;

		if (*scriptPtr != 0) {
			sprintf(buffer, "COMPORTEMENT %d _________________________________", currentComportement);
			debugger_addLine(buffer, script);
			script->lines[script->numOfLignes-1].lineNumber = scriptPtr - actors[num].actorScript;
		}

		do {
			sprintf(buffer, "");
			currentLineNumber = scriptPtr - actors[num].actorScript;
			opcode = *(scriptPtr++);
			switch (opcode) {
			case 0: {
					strcat(buffer, "END");
					debugger_addLine(buffer, script);
					finish = 1;
					endComportement = true;
					break;
				}
			case 1: {
					strcat(buffer, "NOP");
					debugger_addLine(buffer, script);
					break;
				}
			case 2: {
					short int temp;

					strcat(buffer, "SNIF ");
					debugger_manipActor(&scriptPtr, buffer);
					debugger_doCalc(&scriptPtr, buffer);
					temp = *(short int *) scriptPtr;
					scriptPtr += 2;
					sprintf(buffer2, " goto %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 3: {
					short int temp;

					temp = *(short int *) scriptPtr;
					scriptPtr += 2;

					sprintf(buffer2, "OFFSET %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 4: {
					short int temp;

					strcat(buffer, "NEVERIF ");
					debugger_manipActor(&scriptPtr, buffer);
					debugger_doCalc(&scriptPtr, buffer);

					temp = *(short int *) scriptPtr;
					scriptPtr += 2;

					sprintf(buffer2, " goto %d", temp);
					strcat(buffer, buffer2);

					debugger_addLine(buffer, script);
					break;
				}
			case 10: {
					sprintf(buffer2, "LABEL %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 11: {
					strcat(buffer, "RETURN");
					debugger_addLine(buffer, script);
					break;
				}
			case 12: {
					short int temp;

					strcat(buffer, "IF ");
					debugger_manipActor(&scriptPtr, buffer);
					debugger_doCalc(&scriptPtr, buffer);
					temp = *(short int *) scriptPtr;
					scriptPtr += 2;
					sprintf(buffer2, " else goto %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 13: {
					short int temp;

					strcat(buffer, "SWIF ");
					debugger_manipActor(&scriptPtr, buffer);
					debugger_doCalc(&scriptPtr, buffer);
					temp = *(short int *) scriptPtr;
					scriptPtr += 2;
					sprintf(buffer2, " else goto %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 14: {
					short int temp;

					strcat(buffer, "ONEIF ");
					debugger_manipActor(&scriptPtr, buffer);
					debugger_doCalc(&scriptPtr, buffer);
					temp = *(short int *) scriptPtr;
					scriptPtr += 2;
					sprintf(buffer2, " else goto %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 15: {
					short int temp;

					temp = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "ELSE %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 17: {
					sprintf(buffer2, "BODY %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 18: {
					char temp1;
					char temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(scriptPtr++);

					sprintf(buffer2, "BODY_OBJ %d %d", temp2, temp1);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 19: {
					sprintf(buffer2, "ANIM %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 20: {
					char temp1;
					char temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(scriptPtr++);

					sprintf(buffer2, "ANIM_OBJ %d %d", temp2, temp1);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 21: {
					short int temp;

					temp = *(short int *) scriptPtr;
					scriptPtr += 2;

					sprintf(buffer2, "SET_LIFE %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 22: {
					char temp1;
					short int temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(short int *) scriptPtr;
					scriptPtr += 2;

					sprintf(buffer2, "SET_LIFE_OBJ %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 23: {
					short int temp;

					temp = *(short int *) scriptPtr;

					scriptPtr += 2;

					temp = *(actors[num].moveScript + temp + 1);

					sprintf(buffer2, "SET_TRACK %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 24: {
					char temp1;
					short int temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(short int *) scriptPtr;

					scriptPtr += 2;

					temp2 = *(actors[num].moveScript + temp2 + 1);

					sprintf(buffer2, "SET_TRACK_OBJ %d %d", temp2, temp1);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 25: {
					short int temp;

					temp = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "MESSAGE %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 26: {
					sprintf(buffer2, "FALLABLE %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 27: {
					char temp;
					char temp2;

					temp = *scriptPtr++;

					if (temp == 2) {
						temp2 = *scriptPtr++;
						sprintf(buffer2, "SET_DIR %d follow %d", temp, temp2);
					} else {
						sprintf(buffer2, "SET_DIR %d", temp);
					}
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 28: {
					char temp;
					char temp2;
					char temp3;

					temp3 = *scriptPtr++;
					temp = *scriptPtr++;

					if (temp == 2) {
						temp2 = *scriptPtr++;
						sprintf(buffer2, "SET_DIR_OBJ (actor %d) %d follow %d", temp3, temp,
								temp2);
					} else {
						sprintf(buffer2, "SET_DIR_OBJ (actor %d) %d", temp3, temp);
					}
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;

				}
			case 29: {
					sprintf(buffer2, "CAM_FOLLOW %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 30: {
					sprintf(buffer2, "COMPORTEMENT_HERO %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 31: {
					unsigned char temp1;
					unsigned char temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(scriptPtr++);

					sprintf(buffer2, "SET_FLAG_CUBE %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 32: {
					scriptPtr++;
					strcat(buffer, "COMPORTEMENT %d");
					debugger_addLine(buffer, script);
					break;
				}
			case 33: {
					short int temp;

					temp = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "SET_COMPORTEMENT %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 34: {
					char temp1;
					short int temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "SET_COMPORTEMENT_OBJ %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 35: {
					strcat(buffer, "END_COMPORTEMENT");
					debugger_addLine(buffer, script);

					endComportement = true;

					break;
				}
			case 36: {
					unsigned char temp1;
					char temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(scriptPtr++);

					sprintf(buffer2, "SET_FLAG_GAME %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 37: {
					sprintf(buffer2, "KILL_OBJ %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 38: {
					strcat(buffer, "SUICIDE");
					debugger_addLine(buffer, script);
					break;
				}
			case 39: {
					strcat(buffer, "USE_ONE_LITTLE_KEY");
					debugger_addLine(buffer, script);
					break;
				}
			case 40: {
					short int temp;

					temp = *(short int *) scriptPtr;
					scriptPtr += 2;

					sprintf(buffer2, "GIVE_GOLD_PIECES %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 41: {
					strcat(buffer, "END_LIFE");
					debugger_addLine(buffer, script);
					break;
				}
			case 42: {
					strcat(buffer, "STOP_L_TRACK");
					debugger_addLine(buffer, script);
					break;
				}
			case 43: {
					strcat(buffer, "RESTORE_L_TRACK");
					debugger_addLine(buffer, script);
					break;
				}
			case 44: {
					char temp1;
					short int temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(short int *)(scriptPtr);
					scriptPtr += 2;

					sprintf(buffer2, "MESSAGE_OBJ %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 45: {
					strcat(buffer, "INC_CHAPTER");
					debugger_addLine(buffer, script);
					break;
				}
			case 46: {
					sprintf(buffer2, "FOUND_OBJECT %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 47: {
					short int temp;

					temp = *(short int *)(scriptPtr);
					scriptPtr += 2;

					sprintf(buffer2, "SET_DOOR_LEFT %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 48: {
					short int temp;

					temp = *(short int *)(scriptPtr);
					scriptPtr += 2;

					sprintf(buffer2, "SET_DOOR_RIGHT %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 49: {
					short int temp;

					temp = *(short int *)(scriptPtr);
					scriptPtr += 2;

					sprintf(buffer2, "SET_DOOR_UP %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 50: {
					short int temp;

					temp = *(short int *)(scriptPtr++);
					scriptPtr += 2;

					sprintf(buffer2, "SET_DOOR_DOWN %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 51: {
					sprintf(buffer2, "GIVE_BONUS %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 52: {
					sprintf(buffer2, "CHANGE_CUBE %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 53: {
					sprintf(buffer2, "OBJ_COL %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 54: {
					sprintf(buffer2, "BRICK_COL %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 55: {
					short int temp;

					strcat(buffer, "OR_IF ");
					debugger_manipActor(&scriptPtr, buffer);
					debugger_doCalc(&scriptPtr, buffer);
					temp = *(short int *) scriptPtr;
					scriptPtr += 2;
					sprintf(buffer2, " goto %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 56: {
					sprintf(buffer2, "INVISIBLE %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 57: {
					sprintf(buffer2, "ZOOM %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 58: {
					sprintf(buffer2, "POS_POINT %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 59: {
					sprintf(buffer2, "SET_MAGIC_LEVEL %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 60: {
					sprintf(buffer2, "SUB_MAGIC_POINT %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 61: {
					char temp1;
					char temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(scriptPtr++);

					sprintf(buffer2, "SET_LIFE_POINT_OBJ %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 62: {
					char temp1;
					char temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(scriptPtr++);

					sprintf(buffer2, "SUB_LIFE_POINT_OBJ %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 63: {
					char temp1;
					char temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(scriptPtr++);

					sprintf(buffer2, "HIT_OBJ %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 64: {
					int temp = strlen((char *) scriptPtr);

					sprintf(buffer2, "PLAY_FLA %s", (char *) scriptPtr);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					scriptPtr += temp + 1;
					break;
				}
			case 65: {
					sprintf(buffer2, "PLAY_MIDI %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 66: {
					strcat(buffer, "INC_CLOVER_BOX");
					debugger_addLine(buffer, script);
					break;
				}
			case 67: {
					sprintf(buffer2, "SET_USED_INVENTORY %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 68: {
					short int temp;

					temp = *(short int *)(scriptPtr);

					scriptPtr += 2;

					sprintf(buffer2, "ADD_CHOICE %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 69: {
					short int temp;

					temp = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "ASK_CHOICE %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 70: {
					short int temp;

					temp = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "BIG_MESSAGE %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 71: {
					sprintf(buffer2, "INIT_PINGOUIN %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 72: {
					sprintf(buffer2, "SET_HOLO_POS %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 73: {
					sprintf(buffer2, "CLR_HOLO_POS %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 74: {
					sprintf(buffer2, "ADD_FUEL %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 75: {
					sprintf(buffer2, "SUB_FUEL%d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 76: {
					sprintf(buffer2, "SET_GRM %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 77: {
					short int temp;

					temp = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "SAY_MESSAGE %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 78: {
					sprintf(buffer2, "SAY_MESSAGE_OBJ %d", *(scriptPtr++)); // recheck !
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 79: {
					strcat(buffer, "FULL_POINT");
					debugger_addLine(buffer, script);
					break;
				}
			case 80: {
					short int angle;

					angle = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "BETA %d", angle);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 81: {
					strcat(buffer, "GRM_OFF");
					debugger_addLine(buffer, script);
					break;
				}
			case 82: {
					strcat(buffer, "FADE_PAL_RED");
					debugger_addLine(buffer, script);
					break;
				}
			case 83: {
					strcat(buffer, "FADE_ALARM_RED");
					debugger_addLine(buffer, script);
					break;
				}
			case 84: {
					strcat(buffer, "FADE_ALARM_PAL");
					debugger_addLine(buffer, script);
					break;
				}
			case 85: {
					strcat(buffer, "FADE_RED_PAL");
					debugger_addLine(buffer, script);
					break;
				}
			case 86: {
					strcat(buffer, "FADE_RED_ALARM");
					debugger_addLine(buffer, script);
					break;
				}
			case 87: {
					strcat(buffer, "FADE_PAL_ALARM");
					debugger_addLine(buffer, script);
					break;
				}
			case 88: {
					char temp;

					temp = *(scriptPtr++);

					sprintf(buffer2, "EXPLODE_OBJ %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 89: {
					strcat(buffer, "BULLE_ON");
					debugger_addLine(buffer, script);
					break;
				}
			case 90: {
					strcat(buffer, "BULLE_OFF");
					debugger_addLine(buffer, script);
					break;
				}
			case 91: {
					char temp1;
					short int temp2;

					temp1 = *(scriptPtr++);
					temp2 = *(short int *) scriptPtr;

					scriptPtr += 2;

					sprintf(buffer2, "ASK_CHOICE_OBJ %d %d", temp1, temp2);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 92: {
					strcat(buffer, "SET_DARK_PAL");
					debugger_addLine(buffer, script);
					break;
				}
			case 93: {
					strcat(buffer, "SET_NORMAL_PAL");
					debugger_addLine(buffer, script);
					break;
				}
			case 94: {
					strcat(buffer, "MESSAGE_SENDELL");
					debugger_addLine(buffer, script);
					break;
				}
			case 95: {
					sprintf(buffer2, "ANIM_SET %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 96: {
					sprintf(buffer2, "HOLOMAP_TRAJ %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 97: {
					strcat(buffer, "GAME_OVER");
					debugger_addLine(buffer, script);
					break;
				}
			case 98: {
					strcat(buffer, "THE_END");
					debugger_addLine(buffer, script);
					break;
				}
			case 99: {
					strcat(buffer, "MIDI_OFF");
					debugger_addLine(buffer, script);
					break;
				}
			case 100: {
					sprintf(buffer2, "PLAY_CD_TRACK %d", *(scriptPtr++));
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 101: {
					strcat(buffer, "PROJ_ISO");
					debugger_addLine(buffer, script);
					break;
				}
			case 102: {
					strcat(buffer, "PROJ_3D");
					debugger_addLine(buffer, script);
					break;
				}
			case 103: {
					short int temp;

					temp = *(short int *) scriptPtr;
					scriptPtr += 2;

					sprintf(buffer2, "TEXT %d", temp);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					break;
				}
			case 104: {
					strcat(buffer, "CLEAR_TEXT");
					debugger_addLine(buffer, script);
					break;
				}
			case 105: {
					strcat(buffer, "BRUTAL_EXIT");
					debugger_addLine(buffer, script);
					break;
				}
			default: {
					sprintf(buffer2, "Unknown opcode %d", opcode);
					strcat(buffer, buffer2);
					debugger_addLine(buffer, script);
					finish = 1;
					break;
				}
			}

			script->lines[script->numOfLignes-1].lineNumber = currentLineNumber;

		} while (!endComportement);

		currentComportement ++;
	} while (!finish);

	return (script);
}

void debugger_manipActor(unsigned char **scriptPtr, char *buffer) {
	unsigned char opcode;
	char buffer2[256];

	opcode = **(scriptPtr);

	*(scriptPtr) = (*(scriptPtr)) + 1;

	manipActorVar1 = 0;

	switch (opcode) {
	case 0: {
			strcat(buffer, "COL");
			break;
		}
	case 1: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "COL_OBJ %d", temp);
			strcat(buffer, buffer2);
			break;
		}
	case 2: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "DISTANCE %d", temp);
			strcat(buffer, buffer2);
			manipActorVar1 = 1;
			break;
		}
	case 3: {
			strcat(buffer, "ZONE");
			break;
		}
	case 4: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "ZONE_OBJ %d", temp);
			strcat(buffer, buffer2);
			break;
		}
	case 5: {
			strcat(buffer, "BODY");
			break;
		}
	case 6: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "BODY_OBJ %d", temp);
			strcat(buffer, buffer2);
			break;
		}
	case 7: {
			strcat(buffer, "ANIM");
			break;
		}
	case 8: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "ANIM_OBJ %d", temp);
			strcat(buffer, buffer2);
			break;
		}
	case 9: {
			strcat(buffer, "L_TRACK");
			break;
		}
	case 10: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "L_TRACK %d", temp);
			strcat(buffer, buffer2);
			break;
		}
	case 11: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;
			sprintf(buffer2, "FLAG_CUBE %d", temp);
			strcat(buffer, buffer2);
			break;
		}
	case 12: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "CONE_VIEW %d", temp);
			strcat(buffer, buffer2);
			manipActorVar1 = 1;
			break;
		}
	case 13: {
			sprintf(buffer2, "HIT_BY");
			strcat(buffer, buffer2);
			break;
		}
	case 14: {
			strcat(buffer, "ACTION");
			break;
		}
	case 15: {
			sprintf(buffer2, "FLAG_GAME %d", **scriptPtr);
			*(scriptPtr) = (*(scriptPtr)) + 1;
			strcat(buffer, buffer2);
			break;
		}
	case 16: {
			strcat(buffer, "LIFE_POINT");
			break;
		}
	case 17: {
			char temp;

			temp = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "LIFE_POINT_OBJ %d", temp);
			strcat(buffer, buffer2);
			break;
		}
	case 18: {
			strcat(buffer, "NB_LITTLE_KEYS");
			break;
		}
	case 19: {
			strcat(buffer, "NB_GOLD_PIECES");
			manipActorVar1 = 1;
			break;
		}
	case 20: {
			strcat(buffer, "COMPORTEMENT_HERO");
			break;
		}
	case 21: {
			strcat(buffer, "CHAPTER");
			break;
		}
	case 22: {
			char temp;

			temp = **(scriptPtr);

			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "DISTANCE_3D %d", temp);
			strcat(buffer, buffer2);
			manipActorVar1 = 1;
			break;
		}
	case 25: {
			char temp;

			temp = **(scriptPtr);

			*(scriptPtr) = (*(scriptPtr)) + 1;

			sprintf(buffer2, "USE_INVENTORY %d", temp);
			strcat(buffer, buffer2);
			break;

		}
	case 26: {
			strcat(buffer, "CHOICE");
			manipActorVar1 = 1;
			break;
		}
	case 27: {
			strcat(buffer, "FUEL");
			break;
		}
	default: {
			printf("manip actor %d\n", opcode);
			exit(1);
		}
	}
}

void debugger_doCalc(unsigned char **scriptPtr, char *buffer) {
	unsigned char opcode;
	int opcode2;
	char buffer2[256];

	opcode = **(scriptPtr);

	*(scriptPtr) = (*(scriptPtr)) + 1;

	switch (manipActorVar1) {
	case 0: {
			opcode2 = **scriptPtr;
			*(scriptPtr) = (*(scriptPtr)) + 1;
			break;
		}
	case 1: {
			opcode2 = *((short int *)(*scriptPtr));
			*(scriptPtr) = (*(scriptPtr)) + 2;
			break;
		}
	default: {
			printf("Bad mode in docalc!\n");
			exit(1);
			break;
		}
	}

	switch (opcode) {
	case 0: {
			sprintf(buffer2, " == %d", opcode2);
			strcat(buffer, buffer2);
			break;
		}
	case 1: {
			sprintf(buffer2, " > %d", opcode2);
			strcat(buffer, buffer2);
			break;
		}
	case 2: {
			sprintf(buffer2, " < %d", opcode2);
			strcat(buffer, buffer2);
			break;
		}
	case 3: {
			sprintf(buffer2, " >= %d", opcode2);
			strcat(buffer, buffer2);
			break;
		}
	case 4: {
			sprintf(buffer2, " <= %d", opcode2);
			strcat(buffer, buffer2);
			break;
		}
	case 5: {
			sprintf(buffer2, " != %d", opcode2);
			strcat(buffer, buffer2);
			break;
		}
	default: {
			printf("doCalc default: %d\n", opcode);
			exit(1);
			break;
		}
	}
}

#endif
