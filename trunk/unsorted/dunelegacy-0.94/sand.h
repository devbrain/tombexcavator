#ifndef DUNE_SAND_H
#define DUNE_SAND_H
//Makefile and MSVC project files declare the right one
//if compiling with another method uncomment the right line
//#define PLATFORM_WINDOWS 1
//#define PLATFORM_LINUX 1

#define MAPMAGIC 3358395
#define SAVEMAGIC 5342535

//workaround:
//Variable Scope in for-statement Extends Beyond Loop
//ERROR C2374
#ifdef _MSC_VER
#define for if(0);else for
#endif

#define PI 3.1415926535897932384626433832795
#define DIAGONALSPEEDCONST 0.70710678118654752440084436210485
#define DIAGONALCOST 1.4142135623730950488016887242097

/*#ifndef MAKEIPADDRESS
#define MAKEIPADDRESS(b1,b2,b3,b4)  (((b1)<<24)+((b2)<<16)+((b3)<<8)+((b4)))
#endif*/

#define	NUM_SAVEGAMES 6
#define GAMESPEED_MAX 25
#define GAMESPEED_MIN 5
#define VOLUME_MAX 100
#define VOLUME_MIN 0

#define MESSAGE_BUFFER_SIZE 5
#define MAX_PLAYERS 6
#define MAX_CLIENTYS 5
#define TICK_INTERVAL 30
#define PORT 5029
#define BUTTON_CLICK_TIME 100
#define NUM_MULTIGAMES 20

#define MSG_ACCEPT 'a'
#define MSG_ATTACK 't'
#define MSG_BUILDCOMMAND 'b'
#define MSG_BUILDCOMPLETE 'p'
#define MSG_BUTTONHIT 'h'
#define MSG_CANCELCOMMAND 'n'
#define MSG_CONSISTANCY 'o'
#define MSG_CREATEOBJECT 'r'
#define MSG_DEVASTATE 'k'
#define MSG_DEVIATE 'v'
#define MSG_JOIN 'j'
#define MSG_MESSAGE 'm'
#define MSG_OBJECTCOMMAND 'c'
#define MSG_OBJECTDESTROY 'd'
#define MSG_PALACESPECIAL 'l'
#define MSG_PALACEENABLE 'u'
#define MSG_PLACESTRUCTURE 'g'
#define MSG_QUIT 'q'
#define MSG_SETATTACKMODE 'i'
#define MSG_STARPORTORDER 'e'
#define MSG_STARPORTCANCEL 'f'
#define MSG_START 's'

#define BUILDERBUTTONSHEIGHT 48
#define BUILDERBUTTONSMINX 51
#define BUILDERBUTTONSMAXX 115
#define BUILDERBUTTONSMINY 201
#define BUILDERBUTTONSMAXY 423
#define BUILDERBUTTONSSPACING 10

#define BUILDERBUTTONSUPDOWNMINX 39
#define BUILDERBUTTONSUPDOWNMAXX 118
#define BUILDERBUTTONSUPMINY 167
#define BUILDERBUTTONSUPMAXY 188
#define BUILDERBUTTONSDOWNMINY 435
#define BUILDERBUTTONSDOWNMAXY 456

#define MAX_LINE 256
#define MAX_MUSICFILES 1000
#define MAX_MSGLENGTH 30
#define MAX_NAMELENGTH 20
#define MAX_UNITSOUNDS 5
#define MAX_XSIZE 256
#define MAX_YSIZE 256
#define ROCKFILLER 1		//how many times random generator will try to remove sand "holes" for rock from the map
#define SPICEFILLER 1		//for spice
#define ROCKFIELDS 10		//how many fields it will randomly generate
#define SPICEFIELDS 10
#define BUILDRANGE 2
#define RANDOMSPICEMIN 300	//how much spice on each spice tile
#define RANDOMSPICEMAX 500
#define RANDOMTHICKSPICEMIN 200
#define RANDOMTHICKSPICEMAX 300
#define ROCKBITS 10			//how many of the single bits of rock
#define conv2char 2.0 * PI / 256
#define BLOCKSIZE 16		//size of tile pieces 16x16
#define GAMEBARWIDTH 144
#define NONE -1
#define RANDOM -2

#define DEVIATIONTIME 1000
#define HARVESTERMAXSPICE 700
#define HARVESTSPEED 0.20
#define HARVESTEREXTRACTSPEED 0.75
#define RANDOMHARVESTMOVE 300
#define HEAVILYDAMAGEDRATIO 0.3	//if health/maxHealth < this, when damaged will become heavily damage- smoke and shit
#define HEAVILYDAMAGEDSPEEDMULTIPLIER 0.666666666666
#define MAXBUILDLIST 100		//the max amount of items a builder can have in its to build list
#define MAX_FONT_SIZE 50
#define BUILDTIMEMULTIPLIER 10	//more of this will make things build slower
#define RANDOMTURNTIMER 2000	//less of this makes units randomly turn more
#define CHECKTIME 10
#define NUMSELECTEDLISTS 9
#define NUM_INFANTRY_PER_CELL 5		//how many infantry can fit in a cell

#define LASTSANDFRAME 2	//is number spice output frames - 1

typedef enum
{
	HOUSE_ATREIDES,		//house definitions
	HOUSE_ORDOS,
	HOUSE_HARKONNEN,
	HOUSE_SARDAUKAR,
	HOUSE_FREMEN,
	HOUSE_MERCENARY,
	NUM_HOUSES
} HOUSETYPE;

#define UNIT_REPAIRCOST 0.1
#define DEFAULT_GUARDRANGE 10			//0 - 10, how far unit will search for enemy when guarding
#define DEFAULT_STARTINGCREDITS 3000
#define DEFAULT_TURNSPEED 0.625
#define DEFAULT_TURRETTURNSPEED 0.625
#define DEFAULT_UNITFRAMETIME 5
#define DEFAULT_UNITVIEWRANGE 5
#define DEVIATION_TIME 10000

#define NUM_STRUCTUREFRAMES 1
#define BARRACKS_HITPOINTS 800
#define BARRACKS_POWERREQUIREMENT 20
#define BARRACKS_VIEWRANGE 4 
#define CONSTRUCTIONYARD_HITPOINTS 1500
#define CONSTRUCTIONYARD_VIEWRANGE 6
#define GUNTURRET_HITPOINTS 700
#define GUNTURRET_POWERREQUIREMENT 10
#define GUNTURRET_VIEWRANGE 5
#define GUNTURRET_WEAPONDAMAGE 100
#define GUNTURRET_WEAPONRANGE 5
#define HEAVYFACTORY_HITPOINTS 1000
#define HEAVYFACTORY_POWERREQUIREMENT 35
#define HEAVYFACTORY_VIEWRANGE 5
#define HIGHTECHFACTORY_HITPOINTS 1000
#define HIGHTECHFACTORY_POWERREQUIREMENT 35
#define HIGHTECHFACTORY_VIEWRANGE 5
#define IX_HITPOINTS 1000
#define IX_POWERREQUIREMENT 40
#define IX_VIEWRANGE 3
#define LIGHTFACTORY_HITPOINTS 1000
#define LIGHTFACTORY_POWERREQUIREMENT 20
#define LIGHTFACTORY_VIEWRANGE 5
#define PALACE_HITPOINTS 2000
#define PALACE_POWERREQUIREMENT 10
#define PALACE_VIEWRANGE 8
#define RADAR_HITPOINTS 1000
#define RADAR_POWERREQUIREMENT 30
#define RADAR_VIEWRANGE 8
#define REFINERY_CAPACITY 2000
#define REFINERY_HITPOINTS 1500
#define REFINERY_POWERREQUIREMENT 30
#define REFINERY_VIEWRANGE 6
#define REPAIRYARD_HITPOINTS 1000
#define REPAIRYARD_POWERREQUIREMENT 20
#define REPAIRYARD_VIEWRANGE 5
#define ROCKETTURRET_HITPOINTS 700
#define ROCKETTURRET_POWERREQUIREMENT 15
#define ROCKETTURRET_VIEWRANGE 7
#define ROCKETTURRET_WEAPONDAMAGE 100 
#define ROCKETTURRET_WEAPONRANGE 9
#define SILO_CAPACITY 1000
#define SILO_POWERREQUIREMENT 5
#define SILO_VIEWRANGE 3
#define SILO_HITPOINTS 750
#define STARPORT_HITPOINTS 2000
#define STARPORT_POWERREQUIREMENT 80
#define STARPORT_VIEWRANGE 6
#define TILE_VIEWRANGE 2
#define WALL_HITPOINTS 500
#define WALL_VIEWRANGE 2
#define WINDTRAP_HITPOINTS 1000
#define WINDTRAP_POWERSUPPLY 100
#define WINDTRAP_VIEWRANGE 3 
#define WOR_HITPOINTS 1500
#define WOR_POWERREQUIREMENT 20
#define WOR_VIEWRANGE 5

#define CARRYALL_HITPOINTS 100
#define CARRYALL_SPEED 3.0
#define CARRYALL_TURNSPEED 0.1250
#define CARRYALL_VIEWRANGE 8
#define CARRYALL_WEAPONRANGE 0
#define CARRYALL_WEAPONDAMAGE 0
#define DEVASTATOR_HITPOINTS 500
#define DEVASTATOR_SPEED 0.25
#define DEVASTATOR_VIEWRANGE 8
#define DEVASTATOR_WEAPONRANGE 7
#define DEVASTATOR_WEAPONDAMAGE 100
#define DEVIATOR_HITPOINTS 200
#define DEVIATOR_SPEED 0.5
#define DEVIATOR_VIEWRANGE 8
#define DEVIATOR_WEAPONRANGE 6
#define DEVIATOR_WEAPONDAMAGE 40
#define HARVESTER_HITPOINTS 400
#define HARVESTER_VIEWRANGE 5
#define HARVESTER_WEAPONRANGE 0
#define HARVESTER_WEAPONDAMAGE 0
#define INFANTRY_FRAMETIME 500
#define INFANTRY_HITPOINTS 100
#define INFANTRY_VIEWRANGE 4
#define INFANTRY_SPEED 0.4
#define INFANTRY_WEAPONRANGE 3
#define INFANTRY_WEAPONDAMAGE 5
#define LAUNCHER_HITPOINTS 200
#define LAUNCHER_SPEED 0.5
#define LAUNCHER_VIEWRANGE 8
#define LAUNCHER_WEAPONRANGE 10
#define LAUNCHER_WEAPONDAMAGE 70
#define MCV_HITPOINTS 400
#define MCV_VIEWRANGE 7
#define MCV_WEAPONRANGE 0
#define MCV_WEAPONDAMAGE 0
#define ORNITHOPTER_HITPOINTS 100
#define ORNITHOPTER_SPEED 3.0
#define ORNITHOPTER_TURNSPEED 0.1250
#define ORNITHOPTER_VIEWRANGE 8
#define ORNITHOPTER_WEAPONRANGE 5
#define ORNITHOPTER_WEAPONDAMAGE 30
#define QUAD_HITPOINTS 450
#define QUAD_VIEWRANGE 4
#define QUAD_WEAPONRANGE 3
#define QUAD_WEAPONDAMAGE 25
#define SANDWORM_HITPOINTS 3000
#define SANDWORM_SPEED 1.0
#define SANDWORM_VIEWRANGE 8
#define SANDWORM_WEAPONRANGE 0
#define SANDWORM_WEAPONDAMAGE 1000
#define SIEGETANK_HITPOINTS 450
#define SIEGETANK_SPEED 0.3
#define SIEGETANK_VIEWRANGE 8
#define SIEGETANK_WEAPONRANGE 6
#define SIEGETANK_WEAPONDAMAGE 70
#define SONICTANK_HITPOINTS 200
#define SONICTANK_SPEED 0.5
#define SONICTANK_VIEWRANGE 8
#define SONICTANK_WEAPONRANGE 7
#define SONICTANK_WEAPONDAMAGE 70
#define TROOPER_HITPOINTS 100
#define TROOPER_VIEWRANGE 4
#define TROOPER_SPEED 0.3
#define TROOPER_WEAPONRANGE 7
#define TROOPER_WEAPONDAMAGE 50
#define TANK_HITPOINTS 350
#define TANK_SPEED 0.5
#define TANK_VIEWRANGE 6
#define TANK_WEAPONRANGE 5
#define TANK_WEAPONDAMAGE 70
#define TRIKE_HITPOINTS 300
#define TRIKE_VIEWRANGE 4
#define TRIKE_WEAPONRANGE 3
#define TRIKE_WEAPONDAMAGE 20 

#define NUM_ANGLES 8
#define MAXANGLE 7
#define RIGHT 0
#define RIGHTUP 1
#define UP 2
#define LEFTUP 3
#define LEFT 4
#define LEFTDOWN 5
#define DOWN 6
#define RIGHTDOWN 7

#define COLOUR_CLEAR 0
#define COLOUR_BLACK 12
#define COLOUR_DARKGREY 13
#define COLOUR_BLUE 11
#define COLOUR_LIGHTBLUE 9
#define COLOUR_LIGHTGREY 14
#define COLOUR_BROWN 95
#define COLOUR_YELLOW 123
#define COLOUR_GREEN 3
#define COLOUR_LIGHTGREEN 4
#define COLOUR_RED 231
#define COLOUR_WHITE 255
#define COLOUR_ORANGE 83

#define COLOUR_DESERTSAND 105
#define COLOUR_SPICE 111
#define COLOUR_THICKSPICE 116
#define COLOUR_MOUNTAIN 47

#define COLOUR_ATREIDES 160


#define COLOUR_ORDOS 176
#define COLOUR_HARKONNEN 144
#define COLOUR_SARDAUKAR 208
#define COLOUR_FREMEN 192
#define COLOUR_MERCENARY 128

/*use std::max!*/
//#define min(a, b) (((a)<(b))?(a):(b))
//#define max(a, b) (((a)>(b))?(a):(b))

#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_net.h"
#include "SDL/SDL_thread.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
/*
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_net.h>
#include <SDL_thread.h>
#include <SDL_ttf.h>
*/
#include <fstream>
#include <time.h>
#include <new>

typedef struct
{
	short	x,
			y;
} COORDTYPE;

#include "terrainData.h"
#include "data.h"
#include "Link.h"
#include "LinkedList.h"
#include "ListIterator.h"


#include "mmath.h"

typedef enum
{
	CURSOR_NORMAL,
	CURSOR_RIGHT,
	CURSOR_UP,
	CURSOR_LEFT,
	CURSOR_DOWN,
	CURSOR_TARGET,
	CURSOR_SELECTION,
	NUM_CURSORS
} CURSORTYPE;

typedef enum {SCOUT, STANDGROUND, DEFENSIVE, AGGRESSIVE} ATTACKTYPE;
typedef enum {NOTBLOCKED, COMBLOCKED, MOUNTAIN, INFANTRY} BLOCKEDTYPE;
typedef enum {EASY, MEDIUM, HARD/*, IMPOSSIBLE*/} DIFFICULTYTYPE;
typedef enum {GS_NONE, START, BEGUN, END} GAMESTATETYPE;
typedef enum {CUSTOM, MULTIPLAYER, ORIGINAL, RANDOMMAP, SKIRMISH} GAMETYPE;
typedef enum {ATREIDES, ORDOS, HARKONNEN, SARDAUKAR, FREMEN, MERCENERY} PLAYERHOUSE;
typedef enum {CLIENT, SERVER, SINGLE} PLAYERTYPE;

typedef struct
{
	int	minX,
		maxX,
		minY,
		maxY,
		radarMinX,
		radarMaxX,
		radarMinY,
		radarMaxY;
} BORDERTYPE;

typedef struct
{
	char* dat;
} DATAFILE;

typedef struct
{
	GAMETYPE gameType;
	PLAYERTYPE playerType;

	bool	concreteRequired,
			doubleBuffered,
			finished,
			isHuman[MAX_PLAYERS],
			update,
			won;

	char	lastAddress[MAX_LINE],
			localPlayerName[MAX_NAMELENGTH],
			mapFilename[MAX_LINE],
			playerName[MAX_PLAYERS][MAX_NAMELENGTH],
			screen[MAX_LINE],
			serverString[MAX_LINE];

	int		maxPathSearch,	//how many nodes units will search to path before stopping
			maxPlayers,
			width, height,
			winFlags,
			
			playerColour[MAX_PLAYERS],
			playerHouse[MAX_PLAYERS],
			playerTeam[MAX_PLAYERS],
			playerMapNumber[MAX_PLAYERS],
			playerNum,
			techLevel,
			numPlayers,
			gamespeed,
			campaignLevel;
	

	DIFFICULTYTYPE	playerDifficulty[MAX_PLAYERS];
			
	Uint16	port;

	IPaddress	serverIp;
} SETTINGSTYPE;

extern SETTINGSTYPE settings;

extern SDL_Palette	*palette;
extern TTF_Font		*font[MAX_FONT_SIZE];

extern bool debug;
extern char messageBuffer[MESSAGE_BUFFER_SIZE][MAX_LINE];
extern int messageTimer[MESSAGE_BUFFER_SIZE];

extern SDL_Surface* screen;
extern BORDERTYPE* dborder;
extern DATAFILE* dataFile;
extern int lookDist[11];
extern int houseColour[NUM_HOUSES];
extern int	Music_Last,
			Music_AttackFirst,
			Music_AttackLast,
			Music_IntroFirst,
			Music_IntroLast,
			Music_LoseFirst,
			Music_LoseLast,
			Music_PeaceFirst,
			Music_PeaceLast,
			Music_WinFirst,
			Music_WinLast;

#include "BuildItemClass.h"
#include "PlayerClass.h"
#include "AiPlayerClass.h"
#include "gui/gui.h"
#include "gui/Graphics.h"
#include "Net.h"

#include "ObjectClass.h"
#include "BulletClass.h"

#include "SoundPlayerClass.h"

#include "TerrainClass.h"
#include "PriorityQ.h"

#include "MapClass.h"

#include "MentatClass.h"
#include "CutScene.h"

extern PlayerClass	*thisPlayer,
					**player;

extern SoundPlayerClass* soundPlayer;

extern MessageBox messageBox;

void loadDataFile();
void destroyDataFile();
void fixDisplayFormat();

void selectAll(ListIterator* aList);
void unselectAll(ListIterator* aList);
void copyList(ListIterator* from, ListIterator* to);
bool objectHasABuildList(ObjectClass* object);

extern MapClass		*map;

extern ListIterator		*selectedLists;
extern ListIterator		*unitList;
extern ListIterator		*selectedList;
extern ListIterator		*structureList;
extern ListIterator		*bulletList;

extern ListIterator		*airUnitBlitList;
extern ListIterator		*groundUnitBlitList;
extern ListIterator		*infantryBlitList;
extern ListIterator		*selectionBlitList;
extern ListIterator		*undergroundUnitBlitList;

extern bool edit_mode;

void fixWalls(int xPos, int yPos);
void make_random_map(int sizeX, int sizeY);
void smooth_spot(int i, int j);
void smooth_terrain();
int readMapPlayerNumbers(char* filename, int numbers[MAX_PLAYERS]);
int readMapMaxPlayers(char* filename);
bool loadMap(char* mapName);

void do_editor();

//sand shit
void drawCursor();
void do_game();
void game_loop();
void game_loadMap();

//mentat stuff
void game_mentat_exited();
void setMentat(MentatClass* m);
//misc
int resolveItemPicture(int id);
void game_loadEntryCallback(int num);
//void init_save_load_windows();
//void game_load();
//void game_save();

extern CURSORTYPE cursorFrame;	
extern int	drawnMouseX,
			drawnMouseY;

extern bool	shade,
			fog_wanted;

extern bool	attackMoveMode,
			builderSelectMode,
			fullscreenMode,
			groupCreateMode,
			menuMode,
			moveDownMode,
			moveLeftMode,
			moveRightMode,
			moveUpMode,
			placingMode,
			shift,
			radarMode,
			selectionMode,
			quitGame ;

extern int	radarSizeX,
			radarSizeY,
			radarOffsetX,
			radarOffsetY;

extern GAMESTATETYPE gameState;

extern SDL_Rect	gameBarPos,
				selectionRect;

extern Button unitIcon;

extern VBox	devastatorInterface,
			harvesterInterface,
			mcvInterface,
			palaceInterface,
			unitInterface;
extern int	structuresCreated,
			unitsCreated;

extern int debugCount;
extern MentatClass *mentat;
//extern int settings.campaignLevel;


//extern Window		loadWindow;
//extern Window		saveWindow;
extern bool			inMainMenu;
extern bool			loadingGame;

#endif

