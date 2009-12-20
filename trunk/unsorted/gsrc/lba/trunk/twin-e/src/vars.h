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

#ifndef _VARS_
#define _VARS_

#include "decomp.h"
#include <stdlib.h>

#ifdef PCLIKE
#ifdef WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif
#endif //PCLIKE

#ifdef PCLIKE
#ifdef WIN32
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif
#else
//#define PACKED __attribute__((packed))
#define PACKED
#endif

#include "global.h"

#define NUM_MAX_FLAGS 200
#define NUM_MAX_ZONES 100

int threadTimer(void);
void startThreadTimer(void);

struct pointTab {
#ifdef USE_FLOAT
	float x;
	float y;
	float z;
#else
	short int x;
	short int y;
	short int z;
#endif
};

typedef struct pointTab pointTab;

struct pointEntry {
	short int data1;
	short int data2;
	short int data3;
	short int param;
	short int flag;
	short int rotateZ;
	short int rotateY;
	short int rotateX;
	int field_10;
	int field_14;
	int field_18;
	int Y;
	int field_20;
	short int field_24;
} PACKED;

typedef struct pointEntry pointEntry;

struct renderTabEntry {
	short int depth;
	short int renderType;
	unsigned char *dataPtr;
};

typedef struct renderTabEntry renderTabEntry;

//  public:
extern short int *tab1;
extern short int *tab2;
extern short int *tab3;

extern int _angleX;
extern int _angleY;
extern int _angleZ;

extern int _cameraAngleX;
extern int _cameraAngleY;
extern int _cameraAngleZ;

extern int renderLeft;
extern int renderRight;
extern int renderTop;
extern int renderBottom;

extern int _X;
extern int _Y;
extern int _Z;

extern int destX;
extern int destY;
extern int destZ;

extern short int polyTab[960];
extern short int polyTab2[960];

extern int _numOfPrimitives;

extern int isUsingOrhoProjection;

extern pointTab _projectedPointTable[800];
extern pointTab _flattenPointTable[800];
extern short int shadeTable[500];

extern int setSomethingVar1;
extern int setSomethingVar2;
extern short int setSomethingVar3;

extern unsigned char *frontVideoBuffer;

extern int _baseMatrix[3 * 3];

extern int _numOfPoints;
extern int _numOfParts;
extern unsigned char *_pointsPtr;
extern unsigned char *_partsPtr;

extern int _matrixTable[271]; // should be matrixes
extern unsigned char *_currentMatrixTableEntry;

extern int *_shadePtr;

extern int _shadeMatrix[9];
extern int _lightX;
extern int _lightY;
extern int _lightZ;

extern short int primitiveCounter;
extern renderTabEntry *renderTabEntryPtr;
extern renderTabEntry *renderTabEntryPtr2;
extern renderTabEntry *renderTabSortedPtr;

extern renderTabEntry renderTab[1000];
extern renderTabEntry renderTabSorted[1000];
extern unsigned char renderTab7[10000];

extern int renderBottom;
extern int renderLeft;
extern int renderLoop;
extern int renderRight;
extern int renderTop;

extern short int vertexCoordinates[193];

extern int numOfVertex;

extern unsigned char *renderV19;

extern short int FillVertic_AType;

extern short int pRenderV3[96];
extern short int *pRenderV1;
extern short int *pRenderV2;
extern short int numOfVertexRemaining;
extern short int polyCropped;

extern short int vleft;
extern short int vtop;
extern short int vright;
extern short int vbottom;

extern unsigned char oldVertexParam;
extern unsigned char vertexParam1;
extern unsigned char vertexParam2;

extern int textWindowTop;
extern int textWindowLeftSave;
extern int textWindowLeft;
extern int textWindowTopSave;
extern int textWindowRight;
extern int textWindowRightSave;
extern int textWindowBottom;
extern int textWindowBottomSave;

extern unsigned char *_partsPtr2;

void loadPart(int edx, int ecx, int ebx, unsigned char * ptr);
int AffObjetIso(int X, int Y, int Z, int angleX, int angleY, int angleZ, unsigned char *costumePtr);
void SetLightVector(int a, int b, int c);
int renderAnimatedModel(unsigned char *costumePtr);
void RotList(unsigned char *esi, int ecx, pointTab * dest, int *eax);
void RotMatIndex2(int *eax, int *ebp);
void TranslateGroupe(int edx, int ecx, int ebx, pointEntry * esi);
void TransRotList(unsigned char *esi, int ecx, pointTab * dest, int *eax);
int finishRender(unsigned char *esi);
void FillVertic_A(int ecx, int edi);
int ComputePoly_A(void);
void TranslateGroupeS2(short int ax, short int bx, short int cx);
void configureOrthoProjection(int a, int b, int c);
void setSomething3sub(int eax, int ebx, int ecx);
void setCameraAngleSub(int eax, int ebx, int ecx);
void setCameraAngle(int param0, int param1, int param2, int param3, int param4, int param5, int param6);
void setCameraPosition(int X, int Y, int param2, int param3, int param4);
void setSomething2(int a, int b, int c);
void setSomething3(int a, int b, int c);

extern int setSomething3Var12;
extern int setSomething3Var14;
extern int setSomething3Var16;

extern int setSomething2Var1;
extern int setSomething2Var2;
extern int setSomething2Var3;

extern int cameraVar1;
extern int cameraVar2;
extern int cameraVar3;

struct zbufferDataStruct {
	short int y;
	short int z;
	short int x;
	short int drawX;
	short int drawY;
	short int spriteNum;
};

typedef struct zbufferDataStruct zbufferDataStruct;

struct flagDataStruct {
	short int x;
	short int z;
	short int y;
};

typedef struct flagDataStruct flagDataStruct;

/*
struct pointTab
    {
  short int x;
  short int y;
  short int z;
    };*/

struct point2dStruct {
	short int x;
	short int y;
};

typedef struct point2dStruct point2dStruct;

struct point3dStruct {
	short int x;
	short int y;
	short int z;
};

typedef struct point3dStruct point3dStruct;

struct lineCoordinates {
	int data;
	short int x1;
	short int y1;
	short int x2;
	short int y2;
};

typedef struct lineCoordinates lineCoordinates;

struct lineData {
	int data;
	short int p1;
	short int p2;
};

typedef struct lineData lineData;

struct sceneStruct {
	short int field_0;
	short int field_2;
	short int field_4;
	short int field_6;
};

typedef struct sceneStruct sceneStruct;

struct extraListStruct {
	short int field_0;
	short int X;
	short int Y;
	short int Z;

	short int field_8;
	short int field_A;
	short int field_C;

	timeStruct trackActorRotation;

	short int field_E;
	short int field_10;
	short int field_12;
	short int field_14;
	short int field_16;
	int time;
	short int field_1C;
	short int field_1E;
	short int field_20;
};

typedef struct extraListStruct extraListStruct;

struct currentDirtyBoxListStruct {
	unsigned short int left;
	unsigned short int top;
	unsigned short int right;
	unsigned short int bottom;
};

typedef struct currentDirtyBoxListStruct currentDirtyBoxListStruct;

struct drawListStruct {
	short int field_0;
	unsigned short int field_2;
	unsigned short int X;
	unsigned short int Z;
	unsigned short int Y;
	unsigned short int field_A;
	unsigned short int field_C;
	unsigned short int field_E;
	unsigned short int field_10;
};

typedef struct drawListStruct drawListStruct;

struct FLAheaderStruct {
	char version[6];
	int numOfFrames;
	char speed;
	char var1;
	short int var2;
	short int var3;
};

typedef struct FLAheaderStruct FLAheaderStruct;

struct frameDataStruct {
	char videoSize;
	char dummy;
	int frameVar0;
};

typedef struct frameDataStruct frameDataStruct;

#ifdef MEM_DEBUG

extern int currentAllocatedMemory;
extern int staticMemoryUsage;

#endif

extern int useFlaPCX;
extern int flaTime;
extern short int flaVar2;
extern int numOfFrameInFLA;
extern char flaPalette[256*3];
extern char flaPaletteRGBA[256*4];
extern FLAheaderStruct flaHeaderData;
extern byte* workVideoBufferCopy;
extern FILE* dataFileHandle;
extern int flahVar2;
extern int flahVar3;
extern int flaSpeed;
extern int samplesInFla;
extern frameDataStruct frameData;
extern int runFLAscriptVar0;
extern int lastNumOfColor;
extern int lastStartColor;
extern char flaBuffer[320*200];

#ifdef PCLIKE
extern SDL_CD* cdrom;
#endif

extern short int key;
extern uint16 useSound;
extern byte useSB;
extern hqr_entry* HQR_Midi;
extern uint32 unkPtr;   // recheck
extern uint32 cfg_file;

extern byte *workVideoBuffer;
extern byte *frontVideoBufferbis;

extern byte *videoPtr1;
extern byte *videoPtr2;
extern byte *videoPtr3;
extern byte *videoPtr4;
extern byte *videoPtr5;
extern byte *videoPtr6;
extern byte *videoPtr7;
extern byte *videoPtr8;
extern byte *videoPtr9;
extern byte *videoPtr10;
extern byte *videoPtr11;
extern byte *videoPtr12;
extern byte *videoPtr13;

extern byte palette[256 * 3]; // tempvalue
extern byte palette2[256 * 3];  // tempvalue
extern byte paletteRGBA[256 * 4];

extern unsigned char outBuffer[512000];

extern int cropLeft;

extern byte *bufSpeak;
extern byte *bufMemoSeek;
extern char *bufText;
extern char *bufOrder;
extern byte *bufAni1;
extern byte *bufAni2;
extern byte *menuPal;
extern byte menuPalRGBA[1024];
extern byte *shadowSprite;
extern byte *spriteActorData;
extern byte *lbaFont;

#ifdef PRELOAD_ALL
extern hqr_entry *HQR_Fic;
extern hqr_entry *HQR_Bodies;
extern hqr_entry *HQR_Scenes;
extern hqr_entry *HQR_Text;
extern hqr_entry *HQR_Grids;
extern hqr_entry *HQR_Bll;
#endif
extern hqr_entry *HQR_Inventory;
extern hqr_entry *HQR_Sprites;
extern hqr_entry *HQR_Anims;
extern hqr_entry *HQR_Samples;

extern byte *bufCube;
extern byte *bufferBrick;
extern byte *bufferBrick2;

extern byte *fntFont;

extern int interCharSpace;    // espace inter lettre

extern int spaceLenght;   // largeur d'un espace

extern int textColor;
extern int progressiveTextStartColor;
extern int progressiveTextStopColor;
extern int progressiveTextStepSize;
extern int progressiveTextBufferSize;

extern int setup_lst;

extern int samplesLoaded;

extern int textVar1;    // current text bank

extern byte textVar2[256];

extern byte textVar3;

extern int language;

extern int languageCD1;

extern int mainMenu3Var1;
extern int mainMenu3Var2;

extern char mainMenu3Var3[256];

extern FILE *voxFileHandle;
extern int printTextVar5;

extern int dialogueBoxLeft;
extern int dialogueBoxTop;
extern int dialogueBoxRight;
extern int dialogueBoxBottom;

extern short int key1;
extern short int printTextVar12;
extern int printTextVar13;

extern int largeurEcran;
extern int hauteurEcran;

extern int flagDisplayText;

extern short int skipIntro;
extern short int palReseted;

extern short int buttonDrawVar1;

extern char saveGameFileName[143];

extern char currentRoom;
extern char mainMenuVar3;
extern int gameStaffIsDisplayed;
extern short int shadowMode;

extern char mainMenuVar1[60];

extern short int mainMenuData[12];
extern short int subMenu2Data[8];
extern short int soundMenuData[12];
extern short int subMenuData[14];

extern char needChangeRoom;
extern short int chapter;
extern short int newGameVar4;

extern int newGameVar5;

extern int screenLockupTable[2000]; // valeur temporaire...

extern int initVideoVar1;

extern short int newTwinsenX;
extern short int newTwinsenZ;
extern short int newTwinsenY;
extern short int brutalExit;
extern short int numClover;
extern short int numCloverBox;
extern short int currentPingouin;
extern short int magicLevel;
extern short int magicPoint;
extern short int numCoin;
extern short int numKey;
extern short int usingSword;

extern short int currentTextBank;
extern short int fuel;
extern short int cropBottomScreen;
extern short int currentlyFollowedActor;
extern short int startupAngleInCube;

extern short int comportementHero;

extern short int startupComportementHeroInCube;

extern short int numTextEntry;  // nombre d'entree de text dans la bank actuelle

extern int currentTextLength;
extern char *currentTextPtr;

extern int stringLenght;

extern short int useAlternatePalette;
extern short int drawInGameTransBox;

extern char buf1[256];

extern char buf2[256];    // check size

extern char *printText8Ptr1;
extern char *printText8Ptr2;

extern int printText8Var1;
extern int printText8Var2;
extern int printText8Var3;

extern int TEXT_CurrentLetterX;
extern int printText8Var5;
extern int printText8Var6;
extern int TEXT_CurrentLetterY;
extern char *printText8Var8;

extern int printText10Var1;
extern int spaceLength;

extern int dialogueBoxParam1;
extern int dialogueBoxParam2;

extern short int playMusciVar1;

extern int currentlyPlayingCDTrack;

extern char playerName[30];

extern char enterPlayerNameVar1;

extern char allowedCharIndex[71];

extern short int currentKey;

extern short int enterPlayerNameVar2;

extern int addLineBreakX;
extern int printText8PrepareBufferVar2;

extern int wordSizeChar;
extern int wordSizePixel;

extern char spaceChar;
extern short int talkingActor;

extern short int pt8s4var1[96];
extern short int *pt8s4var2;
extern short int *pt8s4var3;
extern short int *pt8s4var4;
extern short int *pt8s4var5;
extern short int *pt8s4var6;

extern int printText8PrepareBufferVar3;

extern actor actors[100];   // yeah, can use up to 100 actor !
extern actor *twinsen;

extern short int holomapTraj;

extern short int roomMusic;
extern short int currentlyPlayingMidi;
extern short int twinsenPositionModeInNewCube;

extern short int newTwinsenXByZone;
extern short int newTwinsenZByZone;

extern short int newTwinsenYByZone;

extern short int twinsenZBeforeFall;

extern timeStruct mainLoopVar1;
extern short int mainLoopVar4;
extern short int disableScreenRecenter;

extern int timeToNextRoomSample;
extern int newCameraX;
extern int newCameraZ;
extern int newCameraY;

extern short int magicBallIdx;
extern short int twinsenMoved;
extern short int useAnotherGrm;

extern short int currentGrid2;
extern short int requestBackgroundRedraw;
extern short int lockPalette;
extern short int changeRoomVar10;
extern short int changeRoomVar11;

extern int numActorInRoom;

extern extraListStruct extraList[50];

extern byte cubeFlags[80];
extern byte itemUsed[28];

extern int currentPositionInBodyPtrTab;
extern short int numOfZones;
extern short int numFlags;

extern byte *file3D0;
extern byte *file3D1;
extern byte *file3D2;
extern byte *file3D3;
extern byte *file3D4;

extern short int TCos0Init;
extern short int TCos1Init;
extern short int TCos2Init;
extern short int TCos3Init;
extern short int TCos4Init;

extern unsigned char *loadTwinsenCostumesVar1;

extern unsigned char *fireEffectVar1;

extern unsigned char *fireEffectVar2;

extern unsigned char *scenePtr;
extern unsigned char *localScenePtr;

extern short int sceneRoomNumber;
extern sceneStruct sceneVar2;
extern sceneStruct sceneVar3;
extern sceneStruct sceneVar4;
extern short int sceneVar14;
extern short int sceneVar15;
extern short int newTwinsenXByScene;
extern short int newTwinsenYByScene;
extern short int newTwinsenZByScene;
extern flagDataStruct flagData[NUM_MAX_FLAGS];
extern ZONE_Box zoneData[NUM_MAX_ZONES];

extern unsigned char *currentGrid;
extern unsigned char *currentBll;

extern int numberOfBll;
extern short int autoAgressivity;

extern short int mainLoopVar5;
extern short int mainLoopVar6;
extern short int mainLoopVar7;

extern int musicPosition;
extern int musicLength;

extern int useSamples;

extern int time1;
extern int time3;

extern unsigned char vars[256];
extern byte GV14[150];
extern int counter;

extern short int fullRedrawVar1;
extern short int fullRedrawVar2;

extern int numOfRedrawBox;

extern currentDirtyBoxListStruct currentDirtyBoxList[300];
extern currentDirtyBoxListStruct nextDirtyBoxList[300];

extern short int fullRedrawVar8;

extern unsigned char scanCodeTab1[29];
extern unsigned short int scanCodeTab2[31];

extern unsigned char *actorScriptPtr;

extern int cameraX;
extern int cameraZ;
extern int cameraY;

#ifdef USE_FLOAT
extern float projectedPositionX;
extern float projectedPositionY;
extern float projectedPositionZ;
#else
extern short int projectedPositionX;
extern short int projectedPositionY;
extern short int projectedPositionZ;
#endif

extern int zbufferVar1;
extern int zbufferVar2;

extern short int manipActorVar1;

extern short int manipActorResult;

extern short int mainLoopVar9;

extern unsigned char *bodyPtrTab[200];

extern short int loadCostumeVar;
extern short int loadCostumeVar2;
extern short int loadCostumeVar3;
extern short int loadCostumeVar4;
extern short int loadCostumeVar5;
extern short int loadCostumeVar6;

extern drawListStruct drawList[150];

extern unsigned char *menuCostumeIndex;
extern short int TCOS[4];

extern timeStruct timeVar;

extern short int winTab[4];

extern char dataString[256];

extern short int currentLineColor;

extern unsigned char *pri2Ptr2;

extern int renderV9;
extern int renderV11;
extern int renderV10;

extern short int numOfPrimitives;
extern unsigned short int costumeHeader;
extern short int numOfPri1;
extern unsigned char *pri1Ptr;

extern short int numOfPri2;
extern unsigned char *pri2Ptr;

extern int renderTab2[271];
extern int *renderTab3;

extern pointTab renderTab5[800];
extern pointTab renderTab6[800];

extern int *renderV21;

extern int rs1v1;
extern int rs1v2;

extern short int rs1s2v1;
extern unsigned char *rs1s2v2;
extern unsigned char *render23;

extern short int render25;

extern unsigned char *render24;

extern int action;

extern int shadowX;
extern int shadowZ;
extern int shadowY;

extern int renderV1;
extern int renderV2;
extern int renderV3;

extern unsigned char *flagModelPtr;

extern zbufferDataStruct zbufferData[28][150];
extern short int zbufferTab[28];

extern char *keyFramePtr;
extern char *animVar1;
extern char *lastKeyFramePtr;

extern short int processActorSub2Var0;
extern short int processActorSub2Var1;

extern int DoTrackVar1;

extern int mainLoopVar17;

extern short int currentlyProcessedActorNum;

extern actor *processActorVar1;
extern short int processActorVar2;
extern short int processActorVar3;
extern short int processActorVar4;

extern short int fieldCauseDamage;
extern short int processActorX;
extern short int processActorY;
extern short int processActorZ;

extern short int processActorVar5;
extern short int processActorVar6;
extern short int currentY;
extern short int currentX;
extern short int currentZ;

extern int getPosVar1;
extern int getPosVar2;
extern int getPosVar3;

extern short int processActorVar11;
extern short int processActorVar12;
extern short int processActorVar13;

extern short int animVar4;

extern int drawVar1;
extern int currentActorInZoneProcess;

extern short int twinsenKey;
extern short int twinsenKey2;

extern short int moveVar1;

extern int showTalkVar;

extern int numOfOptionsInChoice;
extern short int inGameMenuData[10];

extern short int choiceTab[18];
extern int inGameMenuAnswer;

extern int magicBallNumBounce;
extern int magicBallParam;

extern char shadowVar;

extern short int objectRotation[255];
extern char inventorySelectedColor;
extern char currentSelectedObjectInInventory;

// order important !

extern int reinitVar1;
extern int reinitVar2;
extern int reinitVar12;
extern int renderV22;
extern int renderV23;
extern int renderV24;

extern int bufRotate0[9];
extern int bufRotate1[9];

extern byte fkeys;
extern boolean breakmainLoop;

int mainLoopInteration(void);

void MyDial(int index);
void LoadGame(void);
void OpenDialNoWindow(int itemNumber);
void SecondInitDialWindow(void);
void CloseDial(void);
void Rect(int left, int top, int right, int bottom, int param);
void DrawListInventory();
void DrawOneInventory(int objectNumber);
void Inventory(void);
void Draw3dObject(int X, int Y, char* objectPtr, int rotation, int param);
void OpenDial(int textNumber);
void CheckCarrier(int actorNumber);
void ZONE_DrawZones(void);
void GetShadow(int X, int Z, int Y);

void MDL_DrawBoundingBox_ProjectPoints(pointTab* pPoint3d, point3dStruct* pPoint3dProjected);
void MDL_DrawBoundingBoxHiddenPart(actor* pActor);
void MDL_DrawBoundingBoxShownPart(actor* pActor);

int ExtraCheckObjCol(extraListStruct* extra, int param);
void ThrowMagicBall(int X, int Z, int Y, int param1, int angle, int param2, int param4);
int ExtraSearch(int actorNum, int X, int Z, int Y, int spriteIdx, int targetActor, int maxSpeed, int param4);
int ExtraBonus(int X, int Y, int Z, int param, int angle, int type, int param2);

void Aff2DShape(short int* extraData, int X, int Y, int param0, int time, int param1);
void AffSpecial(int extraNum, int X, int Y);
void InitFly(extraListStruct* extraEntry, int var1, int var2, int var3, int var4);
void MixteColonne(unsigned char *gridEntry, unsigned char *dest);
void MixteMapToCube(byte* gridPtr);
void IncrustGrm(int gridNumber);
int WorldColBrickFull(int var0, int var1, int var2, int var3);

void processInGameMenu(int index);

void DrawFrame(char* ptr, int width, int height);
void UpdateFrame(char* ptr, int width);
void AddExt(char* file, char* extention);
int InitFla(char* file);
void ExtInitMcga();
void Mcga_Cls();
void Mcga_Flip();
void DrawNextFrameFla();
void GestionPalette();
void ClearFla();
void ExtInitSvga();

int BoundRegleTrois(int var0, int var1, int var2, int var3);
void CheckZoneSce(actor * lactor, int actorNumber);

void ClearRealAngle(actor * ptr);

void PatchInterAngle(char **ptr, int bp, int bx);
void PatchInterStep(char **ptr, int bp, int bx);

void TransRotList(unsigned char *esi, int ecx, pointTab * dest, int *eax);

int PatchType(char **ptr);

void DoCornerReadjustTwinkel(int var0, int var1, int var2, int var3);
void DoCornerReajust(int var0, int var1, int var2, int var3);
void InitSpecial(int var0, int var1, int var2, int var3);
void ReajustPos(int param);
int CheckObjCol(int param);
void ReceptionObj(void);
int WorldColBrick(int var0, int var1, int var2);
void Rotate(int initialX, int initialY, int angle);
int SetInterDepObjet(int position, char *anim, char *body, animTimerDataStruct* animTimerDataPtr);
int CheckZvOnZv(int var0, int var1);
int ThrowExtra(int actorNum, int X, int Y, int Z, int sprite, int var2, int var3, int var4, int var5, int strength);
void HitObj(int actorAttacking, int actorAttacked, int param, int angle);
void GereExtras(void);
int FullWorldColBrick(int currentX, int currentZ, int currentY, int oldX, int oldZ, int oldY);
void ZoneGiveExtraBonus(ZONE_Box* pZone);

void GiveExtraBonus(actor * lactor);
int Distance3D(int X1, int Z1, int Y1, int X2, int Z2, int Y2);
int StockInterAnim(char *lBufAnim, char *lBody, animTimerDataStruct* animTimerDataPtr);
void GereAnimAction(actor * lactor, int actorNum);

int GetNbFramesAnim(char *ptr);
int GetBouclageAnim(char *ptr);

int GetRealValue(timeStruct * angleData);

void DoAnim(int actorNum);

void ManualRealAngle(int angleFrom, int angleTo, int angleSpeed, timeStruct * angleStruct);

int GetAngle(int X1, int Y1, int X2, int Y2);

void DoDir(int actorNum);

int SetInterAnimObjet2(int animState, char *animData, char *body, animTimerDataStruct* animTimerDataPtr);
int SetInterAnimObjet(int animState, char *animData, char *body, animTimerDataStruct* animTimerDataPtr);

void FlipBoxes(void);

void SmallSort(drawListStruct *list, int listSize, int param);

void CopyMask(int spriteNum, int x, int y, byte * bufferBrick, byte * buffer);

void AddPhysBox(int left, int top, int right, int bottom);

void DrawOverBrick(int X, int Z, int Y);

void InitSprite(int imageNumber, int actorNumber);

void GetDxDyGraph(int arg_0, int *arg_4, int *arg_8, unsigned char *ptr);

int getAnimIndexForBody(byte anim, short int actorNumber);

int Distance2D(int X1, int Y1, int X2, int Y2);

void DoTrack(int actorNumber);

void loadImageCrossFade(int imageNumber);

void convertPalToRGBA(byte * palSource, byte * palDest);

int HQ_3D_MixSample(int param0, int param1, int param2, int param3, int param4, int param5);

int GetRealAngle(timeStruct * arg_0);

void SetClipLBA(int left, int top, int right, int bottom);
#define SetClip SetClipLBA

int finishRender(unsigned char *esi);

void RotList(unsigned char *esi, int ecx, pointTab * edi, int *eax);

void RotMatIndex2(int *eax, int *ebp);

void RotateGroupe(int edx, int ecx, int ebx, pointEntry * esi);
void TranslateGroupe(int edx, int ecx, int ebx, pointEntry * esi);

int ComputePoly_A(void);

int ComputePoly(void);

void FillVertic(int arg0, int arg_4);
void FillVertic_A(int ecx, int edi);

int AnimNuage(unsigned char *ptr);

int RotateNuage(unsigned char *ptr);

int AffObjetIso(int arg_0, int arg_4, int arg_8, int arg_C, int arg_10, int arg_14,
				unsigned char *arg_18);

void DrawObj3D(short int arg_0, short int arg_4, short int arg_8, short int arg_C,
			   short int arg_10, short int arg_14, unsigned char *arg_18);

void CopyInterAnim(unsigned char *arg_0, unsigned char *arg_4);

void DrawComportement(int anim, int arg, int costume);

void Box(int a, int b, int c, int d, unsigned char e);

void DrawInfoMenu(short int arg_0, short int arg_4);

int setAnimAtKeyFrame(int keyframeIdx, unsigned char *anim, unsigned char *body, animTimerDataStruct* animTimerDataPtr);

void drawMenuWin(short int var);

void processComportementMenu(void);
void foundObject(int objectNumber);

int projectPositionOnScreen(int arg_0, int arg_4, int arg_8);

void loadGfxSub(unsigned char *ptr);

int SearchBody(int costumeNum, int actorNum);

int doCalc(void);

void manipActor(actor * lactor);

int InitAnim(char costume, short int arg_4, unsigned char arg_8, short int actorNum);

void InitBody(char arg_0, short int arg_4);

void SetComportement(int newCostume);

void loadHolomapGFX(void);

void AffGraph(int num, int var1, int var2, unsigned char *localBufferBrick);
void drawSprite2(int num, int var1, int var2, unsigned char *localBufferBrick);

void zbufferSub2(int y, int z, int x);

unsigned char *zbufferSub1(int var);

void zbuffer(int var1, int var2, int y, int z, int x);

void redrawCube(void);

void runActorScript(short int actorNumber);

void addToRedrawBox(short int arg_0, short int arg_4, short int arg_8, short int arg_C);

void fullRedrawSub5(void);

void blitBackgroundOnDirtyBoxes(void);

void setActorAngleSafe(short int arg_0, short int arg_4, short int arg_8, timeStruct * timePtr);

void resetActor(int actorNumber);

void reinitAll3(void);

void reinitVars(void);

void unfreezeTime(void);
int makeGiveUpMenu(void);

void freezeTime(void);

void mainLoop2sub1(void);

void fullRedraw(int param);
void waitRetrace(void);
void setActorAngle(short int arg0, short int arg4, short int arg8, timeStruct * ptr);

void createCube(void);
void addCubeEntry(unsigned char *gridEntry, unsigned char *dest);

int CalcGraphMsk(int var, unsigned char *ptr1, unsigned char *ptr2);

int CreateMaskGph(void);

void RazMem(unsigned char *ptr, int size);

int loadBrk(int gridSize);

void makeFireEffect(int top, int param);
void makeFireEffectInit(void);

int getAnimIndexForBody(byte arg_0, short int actorNumber);

void reinitExtraObjectList(void);

void StartInitObj(short int arg_0);

void SaveGame(void);

int InitGrille(short int roomNumber);

void HoloTraj(int arg_0);

void TestRestoreModeSVGA(int arg_0);

void LoadScene(int sceneNumber);

void LoadFicPerso(void);

void ClearScene(void);

void initProgressiveTextBuffer(void);

void drawLetter2(int x, int y, int c);

void drawDoubleLetter(int a, int b, int c, int d);

void printText8Sub4(short int a, short int b, short int c);

void printText10Sub2(void);

void TEXT_GetLetterSize(byte character, int *pLetterWidth, int *pLetterHeight, byte * pFont);

void getWordSize(char *arg1, char *arg2);

short int getKeyboardChar(void);

int optionMenu(void);

void reinitAll1(void);

void newGame4(void);
void newGame2(void);
void TestCoulDial(short int param);

void printText10Sub(void);

void readBufferSpeak(FILE * fileHandle);

void stopCD(void);

int loadVox(int index);

int InitCDR(char *string);

void ChangeCube(void);

void drawCharacter(int X, int Y, unsigned char caractere);

void Font(int X, int Y, char *string);

int SizeFont(char *string);

int findString(int index);

void drawLine(int a, int b, int c, int d, int e);

void LineLBA(int a, int b, int c, int d, int e);
#define Line LineLBA

void CoulDial(int a, int b, int c);

void CoulFont(int i);
void setTextColorSub(int i);
void drawButton(short int *data, int a);

char *Itoa(int valeur);
void reinitAll(int save);
int mainLoop(void);

void checkHeap();

void SetFont(byte * font, int param1, int param2);

// byte *LoadMalloc_HQR(char *fileName, short int imageNumber);

int processMenu(short int *menuData);
int enterPlayerName(short int param);

void InitDial(int index);

void saveTextWindow(void);
void UnSetClip(void);
int setVoxFileAtDigit(int index);
int printText4(FILE * var1);
void loadSavedTextWindow(void);
int printText6(int var);

int printText7(int var);
int initText(int var);
void InitDialWindow(void);
int printText10(void);
int printText11(void);

void drawButtonGFX(int largeur, int posY, int c, int d, int mode);

void SetBackPal(void);
void Cls(void);

void printTextFullScreen(int textIndex);

void copyStringToString(char *a, char *b, int c);

void HQ_StopSample(void);
char *GetMultiText(int a, char *b);

void FadeToBlack(char *palette);
void RessPict(int imageNumber);
void TimerPause(void);
void WaitTime(int time);
void FadeToPal(char *ptr);

void readKeyboard(void);

void blitRectangle(int left, int top, int right, int bottom, char *dest, int right2, int top2, char *source);

void drawBoxInsideTrans(int left, int top, int right, int bottom, int mode);

void blackToWhite(void);
void newGame(void);

void InitBufferCube(void);

// memory management functions
#ifdef MEM_DEBUG
void initMemorySystem();
void* Malloc(size_t size);
void MemFree(void* ptr);
#endif

void fadeIn(byte * palette);
void adjustPalette(byte R, byte G, byte B, byte * palette, int a);

int RegleTrois32(int modifier, int color, int param, int intensity);
void setPalette(byte * palette);

void dumpFile(char *name, char *ptr, int size); //!

void initVideoStuff(void);

void CopyScreen(byte * source, byte * destination);

void AdelineLogo(void);

void InitProgram(void);
void initAll(char *fileName, int a);

void initVars(void);

void MainGameMenu(void);

void PlayAnimFla(char *flaName);

void DrawCadre(int left, int top, int right, int bottom);

void optionMenu2(void);

void drawSelectableLetters(void);

void drawSelectableLetter(int y, int x, int arg);

int chooseSave(int param);

void processTextLine(void);

void CopyMaskLBA(int spriteNum, int x, int y, byte * localBufferBrick, byte * buffer);

#endif
