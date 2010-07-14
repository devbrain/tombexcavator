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

int setSomething3Var12;
int setSomething3Var14;
int setSomething3Var16;

int setSomething2Var1;
int setSomething2Var2;
int setSomething2Var3;

int destX;
int destY;
int destZ;

int cameraVar1;
int cameraVar2;
int cameraVar3;


short int *tab1;
short int *tab2;
short int *tab3;

int _angleX;
int _angleY;
int _angleZ;

int _cameraAngleX;
int _cameraAngleY;
int _cameraAngleZ;

int renderLeft;
int renderRight;
int renderTop;
int renderBottom;

int _X;
int _Y;
int _Z;

short int polyTab[960];
short int polyTab2[960];

int _numOfPrimitives;

int isUsingOrhoProjection;

pointTab _projectedPointTable[800];
pointTab _flattenPointTable[800];
short int shadeTable[500];

int setSomethingVar1;
int setSomethingVar2;
short int setSomethingVar3;

unsigned char *frontVideoBuffer;
//OSystem *osystem;

int _baseMatrix[3 * 3];

int _numOfPoints;
int _numOfParts;
unsigned char *_pointsPtr;
unsigned char *_partsPtr;

int _matrixTable[271];  // should be matrixes
unsigned char *_currentMatrixTableEntry;

int *_shadePtr;

int _shadeMatrix[9];
int _lightX;
int _lightY;
int _lightZ;

short int primitiveCounter;
renderTabEntry *renderTabEntryPtr;
renderTabEntry *renderTabEntryPtr2;
renderTabEntry *renderTabSortedPtr;

renderTabEntry renderTab[1000];
renderTabEntry renderTabSorted[1000];
unsigned char renderTab7[10000];

/*int renderBottom;
int renderLeft;
int renderRight;
int renderTop; */

int renderLoop;

short int vertexCoordinates[193];

int numOfVertex;

unsigned char *renderV19;

short int FillVertic_AType;

short int pRenderV3[96];
short int *pRenderV1;
short int *pRenderV2;
short int numOfVertexRemaining;
short int polyCropped;

short int vleft;
short int vtop;
short int vright;
short int vbottom;

unsigned char oldVertexParam;
unsigned char vertexParam1;
unsigned char vertexParam2;

int textWindowTop;
int textWindowLeftSave;
int textWindowLeft;
int textWindowTopSave;
int textWindowRight;
int textWindowRightSave;
int textWindowBottom;
int textWindowBottomSave;

unsigned char *_partsPtr2;

//debugger _debugger;

int useFlaPCX;
int flaTime;
short int flaVar2;
int numOfFrameInFLA;
char flaPalette[256*3];
char flaPaletteRGBA[256*4];
FLAheaderStruct flaHeaderData;
byte* workVideoBufferCopy;
FILE* dataFileHandle;
int flahVar2;
int flahVar3;
int flaSpeed;
int samplesInFla;
frameDataStruct frameData;
int runFLAscriptVar0;
int lastNumOfColor;
int lastStartColor;
char flaBuffer[320*200];

#ifdef PCLIKE
SDL_CD* cdrom;
#endif //PCLIKE

short int key;
uint16 useSound;
byte useSB;
hqr_entry* HQR_Midi;
uint32 unkPtr;    // recheck
uint32 cfg_file;

byte *workVideoBuffer;
//    byte *frontVideoBuffer;
byte *frontVideoBufferbis;

byte *videoPtr1;
byte *videoPtr2;
byte *videoPtr3;
byte *videoPtr4;
byte *videoPtr5;
byte *videoPtr6;
byte *videoPtr7;
byte *videoPtr8;
byte *videoPtr9;
byte *videoPtr10;
byte *videoPtr11;
byte *videoPtr12;
byte *videoPtr13;

byte palette[256 * 3];  // tempvalue
byte palette2[256 * 3]; // tempvalue
byte paletteRGBA[256 * 4];

unsigned char outBuffer[512000];

int cropLeft;

byte *bufSpeak;
byte *bufMemoSeek;
char *bufText;
char *bufOrder;
byte *bufAni1;
byte *bufAni2;
byte *menuPal;
byte menuPalRGBA[1024];
byte *shadowSprite;
byte *spriteActorData;
byte *lbaFont;

#ifdef PRELOAD_ALL
hqr_entry *HQR_Fic;
hqr_entry *HQR_Bodies;
hqr_entry *HQR_Scenes;
hqr_entry *HQR_Text;
hqr_entry *HQR_Grids;
hqr_entry *HQR_Bll;
#endif
hqr_entry *HQR_Inventory;
hqr_entry *HQR_Sprites;
hqr_entry *HQR_Anims;
hqr_entry *HQR_Samples;

byte *bufCube;
byte *bufferBrick;
byte *bufferBrick2;

byte *fntFont;

int interCharSpace;   // espace inter lettre

int spaceLenght;    // largeur d'un espace

int textColor;
int progressiveTextStartColor;
int progressiveTextStopColor;
int progressiveTextStepSize;
int progressiveTextBufferSize;

int setup_lst;

int samplesLoaded;

int textVar1;   // current text bank

byte textVar2[256];

byte textVar3;

int language;

int languageCD1;

int mainMenu3Var1;
int mainMenu3Var2;

char mainMenu3Var3[256];

FILE *voxFileHandle;
int printTextVar5;

/*    int textWindowTop;
int textWindowLeftSave;
int textWindowLeft;
int textWindowTopSave;
int textWindowRight;
int textWindowRightSave;
int textWindowBottom;
int textWindowBottomSave;*/

int dialogueBoxLeft;
int dialogueBoxTop;
int dialogueBoxRight;
int dialogueBoxBottom;

short int key1;
short int printTextVar12;
int printTextVar13;

int largeurEcran;
int hauteurEcran;

int flagDisplayText;

short int skipIntro;
short int palReseted;

short int buttonDrawVar1;

char saveGameFileName[143];

char currentRoom;
char mainMenuVar3;
int gameStaffIsDisplayed;
short int shadowMode;

char mainMenuVar1[60];

///////////////////////////////////////////////////////////
// MENU DATA

short int mainMenuData[] = {
	0,          // unk
	4,          // num of buttons
	200,        // unk
	0,          // unk
	0,          // unk
//---------------------------- exit points
	20,         // new game
	0,
	21,         // continue game
	0,
	23,         // options
	0,
	22,         // quit
};

short int subMenu2Data[] = {
	0,
	2,
	240,
	0,
	0,
	28,
	0,
	27,
};

short int soundMenuData[] = {
	0,
	4,
	0,
	0,
	0,
	24,
	0,
	30,
	0,
	46,
	0,
	47,
};

short int subMenuData[] = {
	0,
	5,
	0,
	0,
	0,
	26,
	0,
	4,
	6,
	31,
	7,
	32,
	8,
	33,
};

/////////////////////////////////////////////

char needChangeRoom;
short int chapter;
short int newGameVar4;

int newGameVar5;

int screenLockupTable[2000];  // valeur temporaire...

int initVideoVar1;

// int reinitVar1;
// int progressiveTextStartColor;
short int newTwinsenX;
short int newTwinsenZ;
short int newTwinsenY;
short int brutalExit;
short int numClover;
short int numCloverBox;
short int currentPingouin;
short int magicLevel;
short int magicPoint;
short int numCoin;
short int numKey;
short int usingSword;

short int currentTextBank;
short int fuel;
short int cropBottomScreen;
short int currentlyFollowedActor;
short int startupAngleInCube;

short int comportementHero;

short int startupComportementHeroInCube;

short int numTextEntry; // nombre d'entree de text dans la bank actuelle

int currentTextLength;
char *currentTextPtr;

int stringLenght;

short int useAlternatePalette;
short int drawInGameTransBox;

// SDL_CD *cdrom;

// short int isUsingOrhoProjection;

// int setSomething2Var1;
// int setSomething2Var2;
// int setSomething2Var3;

char buf1[256];

char buf2[256];   // check size

char *printText8Ptr1;
char *printText8Ptr2;

int printText8Var1;
int printText8Var2;
int printText8Var3;

int TEXT_CurrentLetterX;
int printText8Var5;
int printText8Var6;
int TEXT_CurrentLetterY;
char *printText8Var8;

int printText10Var1;
int spaceLength;

int dialogueBoxParam1;
int dialogueBoxParam2;

short int playMusciVar1;

int currentlyPlayingCDTrack;

char playerName[30];

char enterPlayerNameVar1;

char allowedCharIndex[71];

short int currentKey;

short int enterPlayerNameVar2;

int addLineBreakX;
int printText8PrepareBufferVar2;

int wordSizeChar;
int wordSizePixel;

char spaceChar;
short int talkingActor;

/*
* short int backInitVar3; short int backDialogueBoxRight; short int backDialogueBoxBottom;
* short int back2InitVar3; short int back2DialogueBoxRight; short int back2DialogueBoxBottom;
* short int backInitVar2;
*
* short int back3DialogueBoxRight;
*/



//    short int FillVertic_AType;

//    short int vertexCoordinates[193];

//    short int pRenderV3[96];
//    short int *pRenderV1;
//    short int *pRenderV2;
//    short int numOfVertexRemaining;
//    short int polyCropped;

/*    short int vleft;
short int vtop;
short int vright;
short int vbottom;

unsigned char oldVertexParam;
unsigned char vertexParam1;
unsigned char vertexParam2;*/

//    short int numOfVertex;
// short int back3DialogueBoxBottom;

short int pt8s4var1[96];
short int *pt8s4var2;
short int *pt8s4var3;
short int *pt8s4var4;
short int *pt8s4var5;
short int *pt8s4var6;

int printText8PrepareBufferVar3;

/*    short int *tab1;
short int *tab2;
short int *tab3; */

actor actors[100];    // yeah, can use up to 100 actor !
actor *twinsen;

short int holomapTraj;

short int roomMusic;
short int currentlyPlayingMidi;
short int twinsenPositionModeInNewCube;

short int newTwinsenXByZone;
short int newTwinsenZByZone;

short int newTwinsenYByZone;

short int twinsenZBeforeFall;

timeStruct mainLoopVar1;
short int mainLoopVar4;
short int disableScreenRecenter;

int timeToNextRoomSample;
int newCameraX;
int newCameraZ;
int newCameraY;

short int magicBallIdx;
short int twinsenMoved;
short int useAnotherGrm;

short int currentGrid2;
short int requestBackgroundRedraw;
short int lockPalette;
short int changeRoomVar10;
short int changeRoomVar11;

int numActorInRoom;

extraListStruct extraList[50];

byte cubeFlags[80];
byte itemUsed[28];

int currentPositionInBodyPtrTab;
short int numOfZones;
short int numFlags;

byte *file3D0;
byte *file3D1;
byte *file3D2;
byte *file3D3;
byte *file3D4;

short int TCos0Init;
short int TCos1Init;
short int TCos2Init;
short int TCos3Init;
short int TCos4Init;

unsigned char *loadTwinsenCostumesVar1;

unsigned char *fireEffectVar1;

unsigned char *fireEffectVar2;

unsigned char *scenePtr;
unsigned char *localScenePtr;

short int sceneRoomNumber;
sceneStruct sceneVar2;
sceneStruct sceneVar3;
sceneStruct sceneVar4;
short int sceneVar14;
short int sceneVar15;
short int newTwinsenXByScene;
short int newTwinsenYByScene;
short int newTwinsenZByScene;
//flagDataStruct *flagData;
flagDataStruct flagData[NUM_MAX_FLAGS];
ZONE_Box zoneData[NUM_MAX_ZONES];

unsigned char *currentGrid;
unsigned char *currentBll = NULL;

int numberOfBll;
short int autoAgressivity;

short int mainLoopVar5;
short int mainLoopVar6;
short int mainLoopVar7;

int musicPosition;
int musicLength;

int useSamples;

int time1;
int time3;

unsigned char vars[256];
byte GV14[150];
int counter;

short int fullRedrawVar1;
short int fullRedrawVar2;

int numOfRedrawBox;

currentDirtyBoxListStruct currentDirtyBoxList[300];
currentDirtyBoxListStruct nextDirtyBoxList[300];

short int fullRedrawVar8;

unsigned char scanCodeTab1[29];
unsigned short int scanCodeTab2[31];

unsigned char *actorScriptPtr;

int cameraX;
int cameraZ;
int cameraY;

#ifdef USE_FLOAT
float projectedPositionX;
float projectedPositionY;
float projectedPositionZ;
#else
short int projectedPositionX;
short int projectedPositionY;
short int projectedPositionZ;
#endif

int zbufferVar1;
int zbufferVar2;

short int manipActorVar1;

short int manipActorResult;

short int mainLoopVar9;

unsigned char *bodyPtrTab[200];

short int loadCostumeVar;
short int loadCostumeVar2;
short int loadCostumeVar3;
short int loadCostumeVar4;
short int loadCostumeVar5;
short int loadCostumeVar6;

drawListStruct drawList[150];

unsigned char *menuCostumeIndex;
short int TCOS[4];

timeStruct timeVar;

short int winTab[4];

char dataString[256];

short int currentLineColor;

unsigned char *pri2Ptr2;

int renderV9;
int renderV11;
int renderV10;

short int numOfPrimitives;
//    renderTabEntry *renderTabEntryPtr;
//    renderTabEntry *renderTabEntryPtr2;
//  renderTabEntry *renderTabSortedPtr;
unsigned short int costumeHeader;

//    renderTabEntry renderTab[1000];
//    renderTabEntry renderTabSorted[1000];
//    short int polyTab[960];
//    short int polyTab2[960];

short int numOfPri1;
unsigned char *pri1Ptr;

short int numOfPri2;
unsigned char *pri2Ptr;

int renderTab2[271];
int *renderTab3;

//    unsigned char *renderV19;

pointTab renderTab5[800];
pointTab renderTab6[800];
//    short int shadeTable[500];

int *renderV21;

int rs1v1;
int rs1v2;

short int rs1s2v1;
unsigned char *rs1s2v2;

//    short int primitiveCounter;

//    unsigned char renderTab7[10000];

unsigned char *render23;

short int render25;

unsigned char *render24;

//    int renderLoop;

int action;

int shadowX;
int shadowZ;
int shadowY;

int renderV1;
int renderV2;
int renderV3;

unsigned char *flagModelPtr;

zbufferDataStruct zbufferData[28][150];
short int zbufferTab[28];

char *keyFramePtr;
char *animVar1;
char *lastKeyFramePtr;

short int processActorSub2Var0;
short int processActorSub2Var1;

int DoTrackVar1;

int mainLoopVar17;

short int currentlyProcessedActorNum;

actor *processActorVar1;
short int processActorVar2;
short int processActorVar3;
short int processActorVar4;

short int fieldCauseDamage;
short int processActorX;
short int processActorY;
short int processActorZ;

short int processActorVar5;
short int processActorVar6;
short int currentY;
short int currentX;
short int currentZ;

int getPosVar1;
int getPosVar2;
int getPosVar3;

short int processActorVar11;
short int processActorVar12;
short int processActorVar13;

short int animVar4;

int drawVar1;

//    int renderLeft;
//  int renderRight;
//  int renderTop;
//  int renderBottom;

int currentActorInZoneProcess;

short int twinsenKey;
short int twinsenKey2;

short int moveVar1;

int showTalkVar;

int numOfOptionsInChoice;
short int inGameMenuData[10];

short int choiceTab[18];
int inGameMenuAnswer;

int magicBallNumBounce;
int magicBallParam;

char shadowVar;

short int objectRotation[255];
char inventorySelectedColor;
char currentSelectedObjectInInventory;

// order important !

int reinitVar1;
int reinitVar2;
int reinitVar12;
int renderV22;
int renderV23;
int renderV24;
/*    int destX;
int destZ;
int destY; */

int bufRotate0[9];
int bufRotate1[9];

byte fkeys;
boolean breakmainLoop;

void initVars(void) {
	useSound = 0;
	useSB = 1;
	HQR_Midi = 0;
	unkPtr = 0;     // recheck

	workVideoBuffer = 0;
	frontVideoBuffer = NULL;
	frontVideoBufferbis = 0;

	bufMemoSeek = 0;
	bufText = 0;
	bufOrder = 0;
	bufAni1 = 0;
	bufAni2 = 0;
	menuPal = 0;
	shadowSprite = 0;
	spriteActorData = 0;
	lbaFont = 0;

	bufCube = NULL;

	fntFont = 0;
	interCharSpace = 1;
	spaceLenght = 10;

	setup_lst = 0;

	textVar1 = -1;
	textVar3 = 0;

	voxFileHandle = 0;
	printTextVar5 = 0;
	key1 = 0;
	printTextVar12 = 0;
	printTextVar13 = 0;

	textWindowTop = 0;
	textWindowLeftSave = 0;
	textWindowLeft = 0;
	textWindowTopSave = 0;
	textWindowRight = 639;
	textWindowRightSave = 639;
	textWindowBottom = 479;
	textWindowBottomSave = 479;

	dialogueBoxLeft = 16;
	dialogueBoxTop = 334;
	dialogueBoxRight = 623;
	dialogueBoxBottom = 463;

	largeurEcran = 640;
	hauteurEcran = 480;

	languageCD1 = 0;
	flagDisplayText = 1;

	skipIntro = 0;
	palReseted = 0;

	needChangeRoom = -1;
	chapter = 0;
	newGameVar4 = 1;
	newGameVar5 = 0;

	currentRoom = 0;
	mainMenuVar3 = 'S';
	gameStaffIsDisplayed = 0;
	shadowMode = 2;
	lba_time = 0;
	key = 0;

	buttonDrawVar1 = 550;

#ifdef LANG_FRENSH
	language = 1;
#elif defined LANG_GERMAN
	language = 2;
#elif defined LANG_ESPANOL
	language = 3;
#elif defined LANG_ITALIANO
	language = 4;
#elif defined LANG_PORTUGUESE
	language = 5;	// Portuguese - especial Text/Ress.hqr to use it (not an original game translation)
#else
	language = 0;	// English
#endif

	languageCD1 = 0;

	mainMenu3Var1 = -1;
	mainMenu3Var2 = -1;

	useAlternatePalette = 0;
	drawInGameTransBox = 0;

	setSomethingVar1 = 320;
	setSomethingVar2 = 200;
	setSomethingVar3 = 500;
	isUsingOrhoProjection = 0;
	isUsingOrhoProjection = 0;

	// setSomething2Var1=0;
	// setSomething2Var2=0;
	// setSomething2Var3=0;

	dialogueBoxParam1 = 3;
	dialogueBoxParam2 = 591;

	playMusciVar1 = -2;

	currentlyPlayingCDTrack = -1;

	enterPlayerNameVar1 = 0;

	allowedCharIndex[0] = ' ';
	allowedCharIndex[1] = 'A';
	allowedCharIndex[2] = 'B';
	allowedCharIndex[3] = 'C';
	allowedCharIndex[4] = 'D';
	allowedCharIndex[5] = 'E';
	allowedCharIndex[6] = 'F';
	allowedCharIndex[7] = 'G';
	allowedCharIndex[8] = 'H';
	allowedCharIndex[9] = 'I';
	allowedCharIndex[10] = 'J';
	allowedCharIndex[11] = 'K';
	allowedCharIndex[12] = 'L';
	allowedCharIndex[13] = 'M';
	allowedCharIndex[14] = 0x2E;
	allowedCharIndex[15] = 'N';
	allowedCharIndex[16] = 'O';
	allowedCharIndex[17] = 'P';
	allowedCharIndex[18] = 'Q';
	allowedCharIndex[19] = 'R';
	allowedCharIndex[20] = 'S';
	allowedCharIndex[21] = 'T';
	allowedCharIndex[22] = 'U';
	allowedCharIndex[23] = 'V';
	allowedCharIndex[24] = 'W';
	allowedCharIndex[25] = 'X';
	allowedCharIndex[26] = 'Y';
	allowedCharIndex[27] = 'Z';
	allowedCharIndex[28] = 0x2D;
	allowedCharIndex[29] = 'a';
	allowedCharIndex[30] = 'b';
	allowedCharIndex[31] = 'c';
	allowedCharIndex[32] = 'd';
	allowedCharIndex[33] = 'e';
	allowedCharIndex[34] = 'f';
	allowedCharIndex[35] = 'g';
	allowedCharIndex[36] = 'h';
	allowedCharIndex[37] = 'i';
	allowedCharIndex[38] = 'j';
	allowedCharIndex[39] = 'k';
	allowedCharIndex[40] = 'l';
	allowedCharIndex[41] = 'm';
	allowedCharIndex[42] = 0x3F;
	allowedCharIndex[43] = 'n';
	allowedCharIndex[44] = 'o';
	allowedCharIndex[45] = 'p';
	allowedCharIndex[46] = 'q';
	allowedCharIndex[47] = 'r';
	allowedCharIndex[48] = 's';
	allowedCharIndex[49] = 't';
	allowedCharIndex[50] = 'u';
	allowedCharIndex[51] = 'v';
	allowedCharIndex[52] = 'w';
	allowedCharIndex[53] = 'x';
	allowedCharIndex[54] = 'y';
	allowedCharIndex[55] = 'z';
	allowedCharIndex[56] = 0x21;
	allowedCharIndex[57] = '0';
	allowedCharIndex[58] = '1';
	allowedCharIndex[59] = '2';
	allowedCharIndex[60] = '3';
	allowedCharIndex[61] = '4';
	allowedCharIndex[62] = '5';
	allowedCharIndex[63] = '6';
	allowedCharIndex[64] = '7';
	allowedCharIndex[65] = '8';
	allowedCharIndex[66] = '9';
	allowedCharIndex[67] = 0x20;
	allowedCharIndex[68] = 0x08;
	allowedCharIndex[69] = 0x0D;
	allowedCharIndex[70] = 0x00;

	enterPlayerNameVar2 = 0;

	spaceChar = ' ';

	vertexCoordinates[0] = 0x1234;
	/*
	* backDialogueBoxRight=0; backDialogueBoxBottom=0; back2InitVar3=0; back2DialogueBoxRight=0;
	* back2DialogueBoxBottom=0; backInitVar2=0; backUnk1=0; back3DialogueBoxRight=0; backUnk2=0;
	* back3DialogueBoxBottom=0;
	*/

	pt8s4var2 = pt8s4var1 + 1;
	pt8s4var3 = pt8s4var1 + 2;

	pt8s4var4 = pt8s4var1 + 93;
	pt8s4var5 = pt8s4var1 + 94;
	pt8s4var6 = pt8s4var1 + 95;

	tab1 = &angleTable[0];
	tab2 = &angleTable[256];
	tab3 = &angleTable[384];

	twinsen = &actors[0];

	holomapTraj = -1;
	roomMusic = -1;
	currentlyPlayingMidi = -2;

	twinsenPositionModeInNewCube = 0;

	newTwinsenXByZone = 0;
	newTwinsenZByZone = 0;
	newTwinsenYByZone = 0;

	newTwinsenXByScene = 0;
	newTwinsenYByScene = 0;
	newTwinsenZByScene = 0;

	twinsenZBeforeFall = 0;

	numActorInRoom = 0;

	disableScreenRecenter = 0;

	timeToNextRoomSample = 0;
	newCameraX = 0;
	newCameraZ = 0;
	newCameraY = 0;

	magicBallIdx = -1;
	twinsenMoved = 0;
	useAnotherGrm = -1;
	currentGrid2 = -1;
	requestBackgroundRedraw = 1;
	lockPalette = 0;

	changeRoomVar10 = 1;
	changeRoomVar11 = 0;

	currentPositionInBodyPtrTab = 0;
	numOfZones = 0;
	numFlags = 0;

	file3D0 = NULL;
	file3D1 = NULL;

	file3D2 = NULL;
	file3D3 = NULL;
	file3D4 = NULL;

	TCos0Init = 0;
	TCos1Init = 0;
	TCos2Init = 0;
	TCos3Init = 0;
	TCos4Init = 0;

	fireEffectVar1 = NULL;
	fireEffectVar2 = NULL;

	sceneRoomNumber = 0;

	sceneVar2.field_0 = 0;
	sceneVar2.field_2 = 0;
	sceneVar2.field_4 = 0;
	sceneVar2.field_6 = 0;

	sceneVar3.field_0 = 0;
	sceneVar3.field_2 = 0;
	sceneVar3.field_4 = 0;
	sceneVar3.field_6 = 0;

	sceneVar4.field_0 = 0;
	sceneVar4.field_2 = 0;
	sceneVar4.field_4 = 0;
	sceneVar4.field_6 = 0;

	sceneVar14 = 0;
	sceneVar15 = 0;
	newTwinsenXByScene = 0;
	newTwinsenYByScene = 0;
	newTwinsenZByScene = 0;

	mainLoopVar4 = 0;
	autoAgressivity = 1;

	mainLoopVar5 = 0;
	mainLoopVar6 = 0;
	mainLoopVar7 = 0;

	musicPosition = 0;
	musicLength = 0;

	useSamples = 0;

	time1 = 0;
	time3 = 0;

	counter = 0;

	numOfRedrawBox = 0;

	fullRedrawVar8 = 0;

	scanCodeTab1[0] = 0x48;
	scanCodeTab1[1] = 0x50;
	scanCodeTab1[2] = 0x4B;
	scanCodeTab1[3] = 0x4D;
	scanCodeTab1[4] = 0x47;
	scanCodeTab1[5] = 0x49;
	scanCodeTab1[6] = 0x51;
	scanCodeTab1[7] = 0x4F;

	scanCodeTab1[8] = 0x39;
	scanCodeTab1[9] = 0x1C;
	scanCodeTab1[10] = 0x1D;
	scanCodeTab1[11] = 0x38;
	scanCodeTab1[12] = 0x53;
	scanCodeTab1[13] = 0x2A;
	scanCodeTab1[14] = 0x36;

	scanCodeTab1[15] = 0x3B;
	scanCodeTab1[16] = 0x3C;
	scanCodeTab1[17] = 0x3D;
	scanCodeTab1[18] = 0x3E;
	scanCodeTab1[19] = 0x3F;
	scanCodeTab1[20] = 0x40;
	scanCodeTab1[21] = 0x41;
	scanCodeTab1[22] = 0x42;
	scanCodeTab1[23] = 0x43;
	scanCodeTab1[24] = 0x44;
	scanCodeTab1[25] = 0x57;
	scanCodeTab1[26] = 0x58;
	scanCodeTab1[27] = 0x2A;
	scanCodeTab1[28] = 0x0;

	scanCodeTab2[0] = 0x0100; // up
	scanCodeTab2[1] = 0x0200; // down
	scanCodeTab2[2] = 0x0400; // left
	scanCodeTab2[3] = 0x0800; // right
	scanCodeTab2[4] = 0x0500; // home
	scanCodeTab2[5] = 0x0900; // pageup
	scanCodeTab2[6] = 0x0A00; // pagedown
	scanCodeTab2[7] = 0x0600; // end

	scanCodeTab2[8] = 0x0101; // space bar
	scanCodeTab2[9] = 0x0201; // enter
	scanCodeTab2[10] = 0x0401;  // ctrl
	scanCodeTab2[11] = 0x0801;  // alt
	scanCodeTab2[12] = 0x1001;  // del
	scanCodeTab2[13] = 0x2001;  // left shift
	scanCodeTab2[14] = 0x2001;  // right shift

	scanCodeTab2[15] = 0x0102;  // F1
	scanCodeTab2[16] = 0x0202;  // F2
	scanCodeTab2[17] = 0x0402;  // F3
	scanCodeTab2[18] = 0x0802;  // F4
	scanCodeTab2[19] = 0x1002;  // F5
	scanCodeTab2[20] = 0x2002;  // F6
	scanCodeTab2[21] = 0x4002;  // F7
	scanCodeTab2[22] = 0x8002;  // F8

	scanCodeTab2[23] = 0x0103;  // F9
	scanCodeTab2[24] = 0x0203;  // F10
	scanCodeTab2[25] = 0x0403;  // ?
	scanCodeTab2[26] = 0x0803;  // ?
	scanCodeTab2[27] = 0x00FF;  // left shift
	scanCodeTab2[28] = 0x00FF;
	scanCodeTab2[29] = 0x0;
	scanCodeTab2[30] = 0x0;

	loadCostumeVar = -32000;

	HQR_Flag = 0;

	renderTab3 = &renderTab2[9];

	action = 0;

	useFlaPCX = 1;
	flaVar2 = 1;

	showTalkVar = 1;

	magicBallNumBounce = 1;
	magicBallParam = 1;

	// ADDED
	fkeys = 0;
	breakmainLoop = false;
}

void CoulFont(int i) {
	setTextColorSub(i);
}

void setTextColorSub(int i) {
	textColor = i;
}

void CoulDial(int a, int b, int c) {
	progressiveTextStartColor = b;
	progressiveTextStopColor = a;
	progressiveTextStepSize = c;

	progressiveTextBufferSize = ((b - a) + 1) / c;

}
