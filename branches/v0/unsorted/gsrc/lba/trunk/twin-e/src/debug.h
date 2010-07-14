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

enum debuggerColor {
	SCRIPT_COLOR_DEFAULT,
	SCRIPT_COLOR_LABEL,
	SCRIPT_COLOR_ANIM,
	BUTTON_COLOR_TEXT,
	BUTTON_COLOR_TEXT_ACTIVATED,
};

typedef enum debuggerColor debuggerColor;

struct scriptLineData {
	char* line;
	char indentation;
	int lineNumber;
	debuggerColor color;
};

typedef struct scriptLineData scriptLineData;

struct scriptData {
	int numOfLignes;
	scriptLineData *lines;
};

typedef struct scriptData scriptData;

enum buttonType {
	NO_BUTTON,
	BUTTON_OK,

	BUTTON_TRACK_UP,
	BUTTON_TRACK_DOWN,
	BUTTON_COMP_UP,
	BUTTON_COMP_DOWN,

	BUTTON_GEN,
	BUTTON_INFO,

	BUTTON_CUBE_CLIP,

	BUTTON_HAVE_ALL_ITEMS,

	BUTTON_SHOW_BOUNDING_BOXES,

	BUTTON_SHOW_CUBE_CHANGE_ZONES,
	BUTTON_SHOW_CAMERA_ZONES,
	BUTTON_SHOW_SCENARIC_ZONES,
	BUTTON_SHOW_GRM_ZONES,
	BUTTON_SHOW_OBJ_ZONES,
	BUTTON_SHOW_TEXT_ZONES,
	BUTTON_SHOW_LADDER_ZONES,

	BUTTON_SHOW_ACTORS_NUMBER,

	BUTTON_SHOW_FLAGS,

	BUTTON_SHOW_SPRITE_CLIP,

// cube clip buttons
	BUTTON_CUBE_CLIP_INC_X,
	BUTTON_CUBE_CLIP_DEC_X,

	BUTTON_CUBE_CLIP_INC_Y,
	BUTTON_CUBE_CLIP_DEC_Y,

	BUTTON_CUBE_CLIP_INC_Z,
	BUTTON_CUBE_CLIP_DEC_Z,

	BUTTON_CUBE_CLIP_TOGGLE,
//

};

typedef enum buttonType buttonType;

struct buttonStruct {
	int X;
	int Y;
	int Width;
	int Height;
	buttonType type;
	char *text;
	char color;
};

typedef struct buttonStruct buttonStruct;

struct winStruct {
	int X;
	int Y;
	int Width;
	int Height;

	int numOfButtons;

	buttonStruct buttons[256];
};

typedef struct winStruct winStruct;

// cubClip vars
extern boolean debugger_cubeClipEnabled;
extern int debugger_cubeClipX;
extern int debugger_cubeClipY;
extern int debugger_cubeClipZ;
//

extern boolean bShowBoundingBoxes;

extern boolean bShowSpriteClip;

extern boolean bShowCubeChangeZones;
extern boolean bShowCameraZones;
extern boolean bShowScenaricZones;
extern boolean bShowGRMZones;
extern boolean bShowObjZones;
extern boolean bShowTextZones;
extern boolean bShowLadderZones;

extern boolean bShowActorNumbers;

extern boolean bShowFlags;

extern actor* flagActor;

//LBA_engine * engine;
//OSystem *osystem;

void debugger_init(void);

int debugger_processDebug(void);
int debugger_findActor(int X, int Y);
int debugger_inBox(int X, int Y, int top, int left, int bottom, int right);
void debugger_debugActor(int num);

scriptData *debugger_getActorTrackScript(int num);
scriptData *debugger_getActorComScript(int num);

void debugger_manipActor(unsigned char **scriptPtr, char *buffer);
void debugger_doCalc(unsigned char **scriptPtr, char *buffer);

void debugger_addLine(char *buffer, scriptData * script);
void debugger_addLineColor(char *buffer, scriptData * script, debuggerColor color);
void debugger_fillArea(int X, int Y, int width, int height);
void debugger_fillArea2(int X, int Y, int width, int height, char color);
void debugger_addWin(int index, int X, int Y, int width, int height);
//    void addButton(int winIndew, int X, int Y, int width, int height, char *text, buttonType type);
void debugger_addButton(int winIndew, int X, int Y, int width, int height, char *text, buttonType type, short int boolVar);
buttonType debugger_processInput();
buttonType debugger_findButton(int X, int Y);
void debugger_drawAll(void);

void debugger_debugMainMenu(void);
void debugger_debugCubeClip(void);

extern actorBoxStruct actorBox[256];	// up to 256 actor on screen
extern int debugger_numOfActorOnScreen;

extern winStruct debugger_windows[256];
extern int debugger_numOfWindows;
/*
};
*/

#endif
