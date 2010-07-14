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

#define NUM_MAX_BRICKS 9000

unsigned char* brickTable[NUM_MAX_BRICKS];
unsigned char* brickMaskTable[NUM_MAX_BRICKS];
unsigned int brickSizeTable[NUM_MAX_BRICKS];
unsigned char brickUsageTable[NUM_MAX_BRICKS];


static void RestartPerso(void) {
	twinsen->comportement = 1;
	memset(&twinsen->dynamicFlagsBF, 0, 2);
	memset(&twinsen->staticFlagsBF, 0, 2);

	twinsen->staticFlagsBF.bComputeCollisionWithObj = true;
	twinsen->staticFlagsBF.bComputeCollisionWithBricks = true;
	twinsen->staticFlagsBF.bIsZonable = true;
	twinsen->staticFlagsBF.bCanDrown = true;
	twinsen->staticFlagsBF.bIsFallable = true;

	twinsen->field_14 = 1;
	twinsen->positionInMoveScript = -1;
	twinsen->label = -1;
	twinsen->positionInActorScript = 0;
	twinsen->zone = -1;
	twinsen->angle = startupAngleInCube;
	setActorAngleSafe(twinsen->angle, twinsen->angle, 0, &twinsen->time);
	SetComportement(startupComportementHeroInCube);
	cropBottomScreen = 0;
}

void ChangeCube(void) {
	int i;
	int oldRoom;

	if (needChangeRoom == 4 && vars[30] != 0) // if twinsen house has been destroyed, manualy patch the room number
		needChangeRoom = 118;

	oldRoom = currentRoom;

	currentRoom = needChangeRoom;
	HQ_StopSample();

	/*
	 * if(useAlternatePalette!=0) FadeToBlack((char*)paletteRGBA); else FadeToBlack((char*)menuPalRGBA);
	 */

	if (drawInGameTransBox == 0) {
		// Cls();
		// osystem_Flip(frontVideoBuffer);
	}

	//FreeGrille();
	ClearScene();
	LoadFicPerso();

	twinsen->comportement = 1;
	twinsen->zone = -1;
	twinsen->positionInActorScript = 0;
	twinsen->positionInMoveScript = -1;
	twinsen->label = -1;

	LoadScene(needChangeRoom);

	if (holomapTraj != -1) {
		TestRestoreModeSVGA(0);
		SetBackPal();
		HoloTraj(holomapTraj);
		holomapTraj = -1;
	}

	if (currentRoom == 116 || currentRoom == 117)
		currentTextBank = 10;

	InitDial(currentTextBank + 3);

	if (roomMusic != -1) {
		if (currentlyPlayingMidi != roomMusic) {
			if (IsMidiPlaying())
				FadeMusicMidi(1);
		}
	}

	InitGrille(needChangeRoom);

	if (twinsenPositionModeInNewCube == 1) {
		newTwinsenX = newTwinsenXByZone;
		newTwinsenZ = newTwinsenZByZone;
		newTwinsenY = newTwinsenYByZone;
	}

	if (twinsenPositionModeInNewCube == 2 || twinsenPositionModeInNewCube == 0) {
		newTwinsenX = newTwinsenXByScene;
		newTwinsenZ = newTwinsenYByScene;
		newTwinsenY = newTwinsenZByScene;
	}

	twinsen->X = newTwinsenX;
	twinsen->Y = twinsenZBeforeFall = newTwinsenZ;
	twinsen->Z = newTwinsenY;

	SetLightVector(reinitVar1, reinitVar2, 0);

	if (oldRoom != needChangeRoom) { // backup comportement and angle in case of gameover/reload
		startupComportementHeroInCube = comportementHero;
		startupAngleInCube = twinsen->angle;
		SaveGame();
	}

	RestartPerso();

	//StartInitAllObjs

#ifdef LBA_STUDIO
	twinsen->timeDelay = 0;
	twinsen->faceTwinkenVar = -1; // probably unused for twinsen
#endif

	for (i = 1; i < numActorInRoom; i++) {
		StartInitObj(i);
#ifdef LBA_STUDIO
		actors[i].timeDelay = 0;
		actors[i].faceTwinkenVar = -1; // probably unused for twinsen
#endif
	}

	// fin

	numKey = 0;
	disableScreenRecenter = 0;
	twinsenPositionModeInNewCube = 0;
	timeToNextRoomSample = 0;

	newCameraX = actors[currentlyFollowedActor].X >> 9;
	newCameraZ = actors[currentlyFollowedActor].Y >> 8;
	newCameraY = actors[currentlyFollowedActor].Z >> 9;

	magicBallIdx = -1;
	twinsenMoved = 1;
	useAnotherGrm = -1;
	currentGrid2 = -1;
	requestBackgroundRedraw = 1;
	lockPalette = 1;

	needChangeRoom = -1;
	changeRoomVar10 = 1;
	changeRoomVar11 = 14;

	SetLightVector(reinitVar1, reinitVar2, 0);

	if (roomMusic != -1)
		PlayMusic(roomMusic);

	printf("ChangeCube done\n");
}

void ClearScene(void) {
	int i;

	reinitExtraObjectList();

	for (i = 0; i < 80; i++)
		cubeFlags[i] = 0;

	for (i = 0; i < 10; i++)
		overlayObjectList[i].field_0 = -1;

#ifndef PRELOAD_ALL
	HQR_Reset_Ressource(HQR_Anims);
	HQM_Free_All();
#endif

	currentPositionInBodyPtrTab = 0;
	useAlternatePalette = 0;
}

#ifdef TXT_SCRIPTS

const char unpackedDataPath[] = "dataDump";
const char sceneDataSubdir[] = "SCENE";

void getSceneFileName(char* buffer, int sceneNumber) {
	FILE* fileHandle;
	int i;

	sprintf(buffer, "%s\\sceneList.txt", unpackedDataPath);
	fileHandle = fopen(buffer, "r");

	for (i = 0;i < sceneNumber;i++) {
		fgets(buffer, 256, fileHandle);
	}

	fgets(buffer, 256, fileHandle);

	*strchr(buffer, 0xA) = 0;

	fclose(fileHandle);
}

char* readTextMoveScript(FILE* fHandle) {
	int position = ftell(fHandle);
	int position2;
	int size = 0;
	char buffer[256];
	int numLine = 0;

	char* ptr = (char*)malloc(1);

	*ptr = 0;

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	assert(!strcmp(buffer, "--> TRACK_PROG <--"));

	while (1) {
		numLine++;
		fgets(buffer, 256, fHandle);

		size += strlen(buffer) + 1;

		ptr = (char*)realloc(ptr, size);

		strcat(ptr, buffer);

		*strchr(buffer, 0xA) = 0;
		if (!strcmp(buffer, "END"))
			break;
	}

	return ptr;
}

char* readTextLifeScript(FILE* fHandle) {
	int position = ftell(fHandle);
	int position2;
	int size = 0;
	char buffer[256];
	int numLine = 0;

	char* ptr = (char*)malloc(1);

	*ptr = 0;

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	assert(!strcmp(buffer, "--> LIFE_PROG <--"));

	while (1) {
		numLine++;
		fgets(buffer, 256, fHandle);

		size += strlen(buffer) + 1;

		ptr = (char*)realloc(ptr, size);

		strcat(ptr, buffer);

		*strchr(buffer, 0xA) = 0;
		if (!strcmp(buffer, "END"))
			break;
	}

	return ptr;
}

void LoadScene(int sceneNumber) {
	unsigned char *temp;
	short int temp3;
	int i;

	short int currentActor;

	int modelNumber;
	int size;
	char sceneFileName[256];

	FILE* fHandle;
	char buffer[256];

	getSceneFileName(sceneFileName, sceneNumber);

	sprintf(buffer, "%s\\%s\\%s.sce", unpackedDataPath, sceneDataSubdir, sceneFileName);

	fHandle = fopen(buffer, "rt");

	assert(fHandle);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	assert(!strcmp(buffer, "--> TEXT <--"));

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "textBank: %d", &currentTextBank) == 1);

	needChangeRoom = sceneNumber;

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	assert(!strcmp(buffer, "--> MAP_FILE <--"));

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "cube: %d", &sceneRoomNumber) == 1);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	assert(!strcmp(buffer, "--> AMBIANCE <--"));

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "AlphaLight: %d", &reinitVar1) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "BetaLight: %d", &reinitVar2) == 1);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb0_1: %d", &sceneVar2.field_0) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb0_2: %d", &sceneVar3.field_0) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb0_3: %d", &sceneVar4.field_0) == 1);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb1_1: %d", &sceneVar2.field_2) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb1_2: %d", &sceneVar3.field_2) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb1_3: %d", &sceneVar4.field_2) == 1);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb2_1: %d", &sceneVar2.field_4) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb2_2: %d", &sceneVar3.field_4) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb2_3: %d", &sceneVar4.field_4) == 1);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb3_1: %d", &sceneVar2.field_6) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb3_2: %d", &sceneVar3.field_6) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "amb3_3: %d", &sceneVar4.field_6) == 1);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "Second_Min: %d", &sceneVar14) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "Second_Ecart: %d", &sceneVar15) == 1);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "Jingle: %d", &roomMusic) == 1);

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	assert(!strcmp(buffer, "--> HERO_START <--"));

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "X: %d", &newTwinsenXByScene) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "Y: %d", &newTwinsenYByScene) == 1);
	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;
	verify(sscanf(buffer, "Z: %d", &newTwinsenZByScene) == 1);

	strcpy(actors[0].name, "Twinsen");

	twinsen->moveScript = readTextMoveScript(fHandle);
	twinsen->actorScript = readTextLifeScript(fHandle);

	currentActor = 1;

	while (1) {
		unsigned int staticFlags;
		char buffer2[256];
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;

		sprintf(buffer2, "--> OBJECT %d <--", currentActor);
		if (strcmp(buffer2, buffer))
			break;

		resetActor(currentActor);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		sscanf(buffer, "Name: %s", actors[currentActor].name);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "StaticFlags: %d", &staticFlags) == 1);

		if (staticFlags & 0x1) {
			actors[currentActor].staticFlagsBF.bComputeCollisionWithObj = 1;
		}
		if (staticFlags & 0x2) {
			actors[currentActor].staticFlagsBF.bComputeCollisionWithBricks = 1;
		}
		if (staticFlags & 0x4) {
			actors[currentActor].staticFlagsBF.bIsZonable = 1;
		}
		if (staticFlags & 0x8) {
			actors[currentActor].staticFlagsBF.bIsUsingClipping = 1;
		}
		if (staticFlags & 0x10) {
			actors[currentActor].staticFlagsBF.bIsPushable = 1;
		}
		if (staticFlags & 0x20) {
			actors[currentActor].staticFlagsBF.bIsDead = 1;
		}
		if (staticFlags & 0x40) {
			actors[currentActor].staticFlagsBF.bCanDrown = 1;
		}
		if (staticFlags & 0x80) {
			actors[currentActor].staticFlagsBF.bUnk80 = 1;
		}

		if (staticFlags & 0x100) {
			actors[currentActor].staticFlagsBF.bUnk0100 = 1;
		}
		if (staticFlags & 0x200) {
			actors[currentActor].staticFlagsBF.bNoDisplay = 1;
		}
		if (staticFlags & 0x400) {
			actors[currentActor].staticFlagsBF.bIsSpriteActor = 1;
		}
		if (staticFlags & 0x800) {
			actors[currentActor].staticFlagsBF.bIsFallable = 1;
		}
		if (staticFlags & 0x1000) {
			actors[currentActor].staticFlagsBF.bDoesntCastShadow = 1;
		}
		if (staticFlags & 0x2000) {
			//actors[currentActor].staticFlagsBF.bIsBackgrounded = 1;
		}
		if (staticFlags & 0x4000) {
			actors[currentActor].staticFlagsBF.bIsCarrier = 1;
		}
		if (staticFlags & 0x8000) {
			actors[currentActor].staticFlagsBF.bIsUsingMiniZv = 1;
		}

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "File3D: %d", &actors[currentActor].modelNumber) == 1);

		if (!(actors[currentActor].staticFlagsBF.bIsSpriteActor)) { // if not sprite actor
			HQRM_Load("file3d.hqr", actors[currentActor].modelNumber, &actors[currentActor].entityDataPtr);
		}

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Body: %d", &actors[currentActor].body) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Anim: %d", &actors[currentActor].anim) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Sprite: %d", &actors[currentActor].field_8) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "X: %d", &actors[currentActor].X) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Y: %d", &actors[currentActor].Y) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Z: %d", &actors[currentActor].Z) == 1);

		actors[currentActor].field_20 = actors[currentActor].X;
		actors[currentActor].field_22 = actors[currentActor].Y;
		actors[currentActor].field_24 = actors[currentActor].Z;

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "HitForce: %d", &actors[currentActor].field_66) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Bonus: %d", &actors[currentActor].field_10) == 1);
		actors[currentActor].field_10 &= 0xFE;

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Beta: %d", &actors[currentActor].angle) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "SpeedRot: %d", &actors[currentActor].speed) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Move: %d", &actors[currentActor].comportement) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "CropLeft: %d", &actors[currentActor].cropLeft) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "CropTop: %d", &actors[currentActor].cropTop) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "CropRight: %d", &actors[currentActor].cropRight) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "CropBottom: %d", &actors[currentActor].cropBottom) == 1);

		actors[currentActor].followedActor = actors[currentActor].cropBottom;

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "ExtraBonus: %d", &actors[currentActor].field_12) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Color: %d", &actors[currentActor].talkColor) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Armure: %d", &actors[currentActor].field_14) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "LifePoint: %d", &actors[currentActor].life) == 1);

		actors[currentActor].moveScript = readTextMoveScript(fHandle);
		actors[currentActor].actorScript = readTextLifeScript(fHandle);

		currentActor++;
	}

	numActorInRoom = currentActor;

	fgets(buffer, 256, fHandle);
	*strchr(buffer, 0xA) = 0;

	numOfZones = 0;

	while (!strcmp(buffer, "--> ZONE <--")) {
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "X0: %d", &zoneData[numOfZones].bottomLeft.X) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Y0: %d", &zoneData[numOfZones].bottomLeft.Y) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Z0: %d", &zoneData[numOfZones].bottomLeft.Z) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "X1: %d", &zoneData[numOfZones].topRight.X) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Y1: %d", &zoneData[numOfZones].topRight.Y) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Z1: %d", &zoneData[numOfZones].topRight.Z) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Type: %d", &zoneData[numOfZones].zoneType) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Info0: %d", &zoneData[numOfZones].data.generic.data1) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Info1: %d", &zoneData[numOfZones].data.generic.data2) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Info2: %d", &zoneData[numOfZones].data.generic.data3) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Info3: %d", &zoneData[numOfZones].data.generic.data4) == 1);

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Snap: %d", &zoneData[numOfZones].dummy) == 1);

		numOfZones++;

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
	}

	numFlags = 0;

	while (!strcmp(buffer, "--> TRACK <--")) {
		int flagNum;

		int x;
		int y;
		int z;

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "X: %d", &x) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Y: %d", &y) == 1);
		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Z: %d", &z) == 1);

		flagData[numFlags].x = x;
		flagData[numFlags].y = y;
		flagData[numFlags].z = z;

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
		verify(sscanf(buffer, "Num: %d", &flagNum) == 1);

		assert(numFlags == flagNum);

		numFlags++;

		fgets(buffer, 256, fHandle);
		*strchr(buffer, 0xA) = 0;
	}
}
#else
void LoadScene(int sceneNumber) {
	unsigned char *temp;
	short int temp3;
	int i;

	short int currentActor;

	int modelNumber;
	int size;

	size = HQRM_Load("scene.hqr", sceneNumber, &scenePtr);
	localScenePtr = scenePtr;

	// todo:faire la gestion d'erreur de chargement

	temp = (unsigned char*)scenePtr;

	currentTextBank = READ_LE_BYTE(temp);
	temp++;
	needChangeRoom = sceneNumber;
	sceneRoomNumber = READ_LE_BYTE(temp);
	temp++;
	temp += 2;
	temp += 2;

	reinitVar1 = READ_LE_U16(temp);
	temp += 2;
	reinitVar2 = READ_LE_U16(temp);
	temp += 2;

	sceneVar2.field_0 = READ_LE_U16(temp);
	temp += 2;
	sceneVar3.field_0 = READ_LE_U16(temp);
	temp += 2;
	sceneVar4.field_0 = READ_LE_U16(temp);
	temp += 2;
	sceneVar2.field_2 = READ_LE_U16(temp);
	temp += 2;
	sceneVar3.field_2 = READ_LE_U16(temp);
	temp += 2;
	sceneVar4.field_2 = READ_LE_U16(temp);
	temp += 2;
	sceneVar2.field_4 = READ_LE_U16(temp);
	temp += 2;
	sceneVar3.field_4 = READ_LE_U16(temp);
	temp += 2;
	sceneVar4.field_4 = READ_LE_U16(temp);
	temp += 2;
	sceneVar2.field_6 = READ_LE_U16(temp);
	temp += 2;
	sceneVar3.field_6 = READ_LE_U16(temp);
	temp += 2;
	sceneVar4.field_6 = READ_LE_U16(temp);
	temp += 2;

	sceneVar14 = READ_LE_U16(temp);
	temp += 2;
	sceneVar15 = READ_LE_U16(temp);
	temp += 2;
	roomMusic = READ_LE_BYTE(temp);
	temp++;
	newTwinsenXByScene = READ_LE_U16(temp);
	temp += 2; // ok jusque la
	newTwinsenYByScene = READ_LE_U16(temp);
	temp += 2;
	newTwinsenZByScene = READ_LE_U16(temp);
	temp += 2;

	temp3 = READ_LE_U16(temp);
	temp += 2;
	twinsen->moveScript = temp;
	temp += temp3;

	currentActor = 1;

	temp3 = READ_LE_U16(temp);
	temp += 2;
	twinsen->actorScript = temp;
	temp += temp3;

	numActorInRoom = READ_LE_U16(temp);
	temp += 2;

	while (currentActor < numActorInRoom) {
		unsigned short int staticFlags;
		resetActor(currentActor);
		staticFlags = READ_LE_U16(temp);
		temp += 2;

		if (staticFlags & 0x1) {
			actors[currentActor].staticFlagsBF.bComputeCollisionWithObj = 1;
		}
		if (staticFlags & 0x2) {
			actors[currentActor].staticFlagsBF.bComputeCollisionWithBricks = 1;
		}
		if (staticFlags & 0x4) {
			actors[currentActor].staticFlagsBF.bIsZonable = 1;
		}
		if (staticFlags & 0x8) {
			actors[currentActor].staticFlagsBF.bIsUsingClipping = 1;
		}
		if (staticFlags & 0x10) {
			actors[currentActor].staticFlagsBF.bIsPushable = 1;
		}
		if (staticFlags & 0x20) {
			actors[currentActor].staticFlagsBF.bIsDead = 1;
		}
		if (staticFlags & 0x40) {
			actors[currentActor].staticFlagsBF.bCanDrown = 1;
		}
		if (staticFlags & 0x80) {
			actors[currentActor].staticFlagsBF.bUnk80 = 1;
		}

		if (staticFlags & 0x100) {
			actors[currentActor].staticFlagsBF.bUnk0100 = 1;
		}
		if (staticFlags & 0x200) {
			actors[currentActor].staticFlagsBF.bNoDisplay = 1;
		}
		if (staticFlags & 0x400) {
			actors[currentActor].staticFlagsBF.bIsSpriteActor = 1;
		}
		if (staticFlags & 0x800) {
			actors[currentActor].staticFlagsBF.bIsFallable = 1;
		}
		if (staticFlags & 0x1000) {
			actors[currentActor].staticFlagsBF.bDoesntCastShadow = 1;
		}
		if (staticFlags & 0x2000) {
			//actors[currentActor].staticFlagsBF.bIsBackgrounded = 1;
		}
		if (staticFlags & 0x4000) {
			actors[currentActor].staticFlagsBF.bIsCarrier = 1;
		}
		if (staticFlags & 0x8000) {
			actors[currentActor].staticFlagsBF.bIsUsingMiniZv = 1;
		}

		modelNumber = READ_LE_U16(temp);
		temp += 2;

		if (!(actors[currentActor].staticFlagsBF.bIsSpriteActor)) { // if not sprite actor
#ifdef PRELOAD_ALL
			actors[currentActor].entityDataPtr = HQR_GetCopy(HQR_Fic, modelNumber);
#else
			HQRM_Load("file3d.hqr", modelNumber, &actors[currentActor].entityDataPtr);
#endif
		}

		actors[currentActor].body = READ_LE_BYTE(temp);
		temp++;
		actors[currentActor].anim = READ_LE_BYTE(temp);
		temp++;

		actors[currentActor].field_8 = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].field_20 = actors[currentActor].X = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].field_22 = actors[currentActor].Y = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].field_24 = actors[currentActor].Z = READ_LE_U16(temp);
		temp += 2;

		actors[currentActor].field_66 = READ_LE_BYTE(temp);
		temp++;

		actors[currentActor].field_10 = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].field_10 &= 0xFE;
		actors[currentActor].angle = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].speed = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].comportement = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].cropLeft = READ_LE_S16(temp);
		temp += 2;
		actors[currentActor].cropTop = READ_LE_S16(temp);
		temp += 2;
		actors[currentActor].cropRight = READ_LE_S16(temp);
		temp += 2;
		actors[currentActor].cropBottom = READ_LE_S16(temp);
		temp += 2;
		actors[currentActor].followedActor = actors[currentActor].cropBottom;

		actors[currentActor].field_12 = READ_LE_BYTE(temp);
		temp++;
		actors[currentActor].talkColor = READ_LE_BYTE(temp);
		temp++;
		actors[currentActor].field_14 = READ_LE_BYTE(temp);
		temp++;
		actors[currentActor].life = READ_LE_BYTE(temp);
		temp++;

		temp3 = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].moveScript = temp;
		temp += temp3;

		temp3 = READ_LE_U16(temp);
		temp += 2;
		actors[currentActor].actorScript = temp;
		temp += temp3;

		currentActor++;
	};

	numOfZones = READ_LE_U16(temp);
	temp += 2;
	assert(numOfZones <= NUM_MAX_ZONES);
	for (i = 0; i < numOfZones; i++) {
		zoneData[i].bottomLeft.X = READ_LE_S16(temp);
		temp += 2;
		zoneData[i].bottomLeft.Y = READ_LE_S16(temp);
		temp += 2;
		zoneData[i].bottomLeft.Z = READ_LE_S16(temp);
		temp += 2;

		zoneData[i].topRight.X = READ_LE_S16(temp);
		temp += 2;
		zoneData[i].topRight.Y = READ_LE_S16(temp);
		temp += 2;
		zoneData[i].topRight.Z = READ_LE_S16(temp);
		temp += 2;

		zoneData[i].zoneType = READ_LE_S16(temp);
		temp += 2;

		zoneData[i].data.generic.data1 = READ_LE_S16(temp);
		temp += 2;
		zoneData[i].data.generic.data2 = READ_LE_S16(temp);
		temp += 2;
		zoneData[i].data.generic.data3 = READ_LE_S16(temp);
		temp += 2;
		zoneData[i].data.generic.data4 = READ_LE_S16(temp);
		temp += 2;

		zoneData[i].dummy = READ_LE_S16(temp);
		temp += 2;
	}

	numFlags = READ_LE_U16(temp);
	temp += 2;
	assert(numFlags <= NUM_MAX_FLAGS);
	// copy data to alligned space
	for (i = 0; i < numFlags; i++) {
		flagData[i].x = READ_LE_S16(temp);
		temp += 2;
		flagData[i].y = READ_LE_S16(temp);
		temp += 2;
		flagData[i].z = READ_LE_S16(temp);
		temp += 2;
	}
}
#endif

void HoloTraj(int arg_0) {
	int reinitVar1Copy;
	int progressiveTextStartColorCopy;

// int i;
	short int var_14;
	byte *localmakeHolomapTrajectoryVar;
	int j;
	unsigned short int var;

	freezeTime();

	var_14 = 1;
	reinitVar1Copy = reinitVar1;
	progressiveTextStartColorCopy = progressiveTextStartColor;

	/*
	 * if(!useAlternatePalette) FadeToBlack((char*)menuPalRGBA); else FadeToBlack((char*)paletteRGBA);
	 */

	UnSetClip();
	Cls();
	osystem_Flip(frontVideoBuffer);
	loadHolomapGFX();

	localmakeHolomapTrajectoryVar = videoPtr12;

	j = 0;

	while (j < arg_0) {    /* implementer la suite */
		localmakeHolomapTrajectoryVar += 12;
		var = READ_LE_U16(localmakeHolomapTrajectoryVar);
		var += var;
		localmakeHolomapTrajectoryVar += 2;
		j++;
		localmakeHolomapTrajectoryVar += var;
	}

	/*
	 * implementer la suite
	 */

}

void loadHolomapGFX(void) {
	int i;
	int j;

	videoPtr1 = workVideoBuffer;
	videoPtr2 = workVideoBuffer + 4488;
	videoPtr3 = workVideoBuffer + 7854;
	videoPtr4 = workVideoBuffer + 8398;

	videoPtr5 = workVideoBuffer + 73934;

	Load_HQR("ress.hqr", videoPtr3, 6);
	Load_HQR("ress.hqr", videoPtr4, 7);
	videoPtr6 = videoPtr5 + Load_HQR("ress.hqr", videoPtr5, 9);
	videoPtr7 = videoPtr6 + Load_HQR("ress.hqr", videoPtr6, 10);
	videoPtr8 = videoPtr7 + Load_HQR("ress.hqr", videoPtr7, 11);
	videoPtr11 = videoPtr8 + Load_HQR("ress.hqr", videoPtr8, 29);

// loadGfxSub(videoPtr5);
// loadGfxSub(videoPtr6);
// loadGfxSub(videoPtr7);

// loadGfxSub(videoPtr8);

	videoPtr10 = videoPtr11 + 4488;
	videoPtr12 = videoPtr10 + Load_HQR("ress.hqr", videoPtr10, 8);
	videoPtr13 = videoPtr12 + Load_HQR("ress.hqr", videoPtr12, 30);

	Load_HQR("ress.hqr", (byte *) & palette, 5);

	j = 576;
	for (i = 0; i < 96; i += 3, j += 3) {
		palette2[i] = palette[j];
		palette2[i + 1] = palette[j + 1];
		palette2[i + 2] = palette[j + 2];
	}

	j = 576;
	for (i = 96; i < 189; i += 3, j += 3) {
		palette2[i] = palette[j];
		palette2[i + 1] = palette[j + 1];
		palette2[i + 2] = palette[j + 2];
	}

// loadGFXSub1();
// loadGFXSub2();

// needToLoadHolomapGFX=0;
}

void SetComportement(int newComportement) {
	int temp;

	switch (newComportement) {
	case 0:
		comportementHero = 0;
		twinsen->entityDataPtr = file3D0;
		break;
	case 1:
		comportementHero = 1;
		twinsen->entityDataPtr = file3D1;
		break;
	case 2:
		comportementHero = 2;
		twinsen->entityDataPtr = file3D2;
		break;
	case 3:
		comportementHero = 3;
		twinsen->entityDataPtr = file3D3;
		break;
	case 4:
		comportementHero = 4;
		twinsen->entityDataPtr = file3D4;
		break;
	};

	temp = twinsen->body;

	twinsen->costumeIndex = -1;
	twinsen->body = -1;

	InitBody(temp, 0);

	twinsen->anim = -1;
	twinsen->field_78 = 0;

	InitAnim(ANIM_static, 0, 255, 0);
}

int loadBrk(int gridSize) {
	unsigned int firstBrick = 60000; // should be MAX_UINT
	unsigned int lastBrick = 0; // should be MIN_UINT
	unsigned char* bufferPtr;
	unsigned char* currentPositionInBuffer;
	unsigned char* ptrToBllBits;
	unsigned int i;
	unsigned int j;
	unsigned int numUsedBricks;
	unsigned int currentBllEntryIdx = 0;
	unsigned char* destinationBrickPtr;
	unsigned char* localBufferBrick;
	unsigned char* ptrTempDecompression;
	unsigned int currentBrickIdx;
	unsigned int brickSize;
	unsigned int finalSize = 0;

	memset(brickTable, 0, sizeof(brickTable));
	memset(brickSizeTable, 0, sizeof(brickSizeTable));
	memset(brickUsageTable, 0, sizeof(brickUsageTable));

	ptrToBllBits = currentGrid + (gridSize - 32);

	// build up a map of all used bricks in the room
	for (i = 1;i < 256;i++) {
		unsigned char currentBitByte = *(ptrToBllBits + (i / 8));
		unsigned char currentBitMask = 1 << (7 - (i & 7));

		if (currentBitByte & currentBitMask) {
			unsigned int currentBllOffset = READ_LE_U32(currentBll + currentBllEntryIdx);
			unsigned char* currentBllPtr = currentBll + currentBllOffset;

			unsigned int bllSizeX = currentBllPtr[0];
			unsigned int bllSizeY = currentBllPtr[1];
			unsigned int bllSizeZ = currentBllPtr[2];

			unsigned int bllSize = bllSizeX * bllSizeY * bllSizeZ;

			unsigned char* bllDataPtr = currentBllPtr + 5;

			for (j = 0;j < bllSize;j++) {
				unsigned int brickIdx = READ_LE_U16(bllDataPtr);

				if (brickIdx) {
					brickIdx--;

					if (brickIdx <= firstBrick)
						firstBrick = brickIdx;

					if (brickIdx > lastBrick)
						lastBrick = brickIdx;

					brickUsageTable[brickIdx] = 1;
				}
				bllDataPtr += 4;
			}
		}
		currentBllEntryIdx += 4;
	}

	// compute the number of bricks to load
	currentPositionInBuffer = bufferPtr + firstBrick * 2;
	numUsedBricks = 0;
	for (i = firstBrick;i <= lastBrick;i++) {
		if (brickUsageTable[i]) { // was brick noted as used ?
			numUsedBricks++;
		}
	}

	for (i = firstBrick;i <= lastBrick;i++) {
		if (brickUsageTable[i]) {
			brickSizeTable[i] = Size_HQR("LBA_BRK.HQR", i);
			brickTable[i] = (unsigned char*)malloc(brickSizeTable[i]);
			Load_HQR("LBA_BRK.HQR", brickTable[i], i);
		}
	}
	/*
	currentBllEntryIdx = 0;
	for(i=1;i<256;i++)
	{
	  unsigned char currentBitByte = *(ptrToBllBits + (i/8));
	  unsigned char currentBitMask = 1 << (7-(i&7));

	  if(currentBitByte & currentBitMask)
	  {
	    unsigned int currentBllOffset = READ_LE_U32(currentBll + currentBllEntryIdx);
	    unsigned char* currentBllPtr = currentBll + currentBllOffset;

	    unsigned int bllSizeX = currentBllPtr[0];
	    unsigned int bllSizeY = currentBllPtr[1];
	    unsigned int bllSizeZ = currentBllPtr[2];

	    unsigned int bllSize = bllSizeX * bllSizeY * bllSizeZ;

	    unsigned char* bllDataPtr = currentBllPtr + 5;

	    for(j=0;j<bllSize;j++)
	    {
	      unsigned int brickIdx = READ_LE_U16(bllDataPtr);

	      if(brickIdx)
	      {
	        WRITE_LE_U16(bllDataPtr, READ_LE_U16(bufferPtr + brickIdx * 2 - 2));
	      }
	      bllDataPtr += 4;
	    }
	  }
	  currentBllEntryIdx += 4;
	}*/

	return (0);
}
/*
loadBrk(int gridSize)
{
  int firstBrick;
  int lastBrick;
  int currentBrick;

 // int tempSize2;
  int counter;
  int counter2;
  int counter3;
  int counter6;
  int counter7;
  int offset;
  int offset2;

  unsigned char *endOfGridPtr;
  unsigned char *endOfGridPtr2;
  unsigned char *endOfGridPtr3;
  unsigned char *outPtr;
  unsigned char *outPtr2;
  unsigned char *outPtr3;
  unsigned char *outPtr4;
  unsigned char *destPtr;
//    unsigned char *compressedPtr;
  unsigned char *ptrUnk;

  byte temp;
  byte temp2;
  int temp3;
  int temp4;
  unsigned char *ptr1;
  unsigned char *ptr2;
  int val1;
  int val2;
  int val3;
  unsigned short int val4;
  int finalSize;
  unsigned char *localBufferBrick;

//    int headerSize;
  int dataSize;
//    int compressedSize;

//    short int mode;

  printf("GridSize=%d\n", gridSize);

  firstBrick = 60000;
  lastBrick = 0;
  counter = 1;

  outPtr = workVideoBuffer + 153800;
  outPtr2 = outPtr;

  RazMem(outPtr, 20000);

  offset = 4;

  endOfGridPtr2 = endOfGridPtr = currentGrid + (gridSize - 32);

  do
  {
    temp = *(endOfGridPtr2 + (counter >> 3));
    temp2 = 7 - (counter & 7);

    temp3 = 1 << temp2;

    if (temp & temp3)
    {
      temp4 = READ_LE_U32(currentBll + offset - 4);
      ptr1 = currentBll + temp4;

      val1 = *ptr1;
      val2 = *(ptr1 + 1);

      val2 *= val1;

      val3 = *(char *) (ptr1 + 2);

      val3 *= val2;

      ptr2 = ptr1 + 5;

      counter2 = 0;

      while (counter2++ < val3)
      {
        val4 = READ_LE_U16( ptr2 );
        if (val4 != 0)
        {
          val4--;

          if (val4 <= firstBrick)
            firstBrick = val4;

          if (val4 > lastBrick)
            lastBrick = val4;

          WRITE_LE_U16(outPtr2 + val4*2,1);
        }
        ptr2 += 4;
      }
    }

    offset += 4;
  }while (++counter < 256);

  outPtr3 = (outPtr2 + firstBrick*2);

  currentBrick = firstBrick;

  counter3 = 0;

  while (currentBrick <= lastBrick)
  {
    if (READ_LE_U16(outPtr3) != 0)
      counter3++;
    outPtr3+=2;
    currentBrick++;
  }

  printf("Need to load %d bricks\n", counter3);

  counter3 *= 4;
  counter3 += 4;

  finalSize = counter3; // car on doit au moins avoir 1 ptr par brique

  outPtr4 = (outPtr2 + firstBrick*2);

  localBufferBrick = bufferBrick;

  destPtr = bufferBrick + counter3;

  WRITE_LE_U32(localBufferBrick,counter3);

  localBufferBrick+=4;

  currentBrick = firstBrick;

  ptrUnk = (firstBrick * 4 + workVideoBuffer);

  counter6 = 0;

#ifdef USE_GL
  osystem_startBricks();
#endif

  while (currentBrick <= lastBrick)
  {
    if (READ_LE_U16(outPtr4))
    {
      counter6++;
      WRITE_LE_U16(outPtr4, counter6);

      Load_HQR("LBA_BRK.HQR",destPtr,currentBrick);
      dataSize = Size_HQR("LBA_BRK.HQR",currentBrick);

#ifdef USE_GL
      osystem_addBrickToBuffer((char*)destPtr);
#endif

      finalSize += dataSize;
      destPtr += dataSize;
      WRITE_LE_U32(localBufferBrick,finalSize);
      localBufferBrick+=4;
    }

    outPtr4+=2;
    ptrUnk+=4;
    currentBrick++;
  }
#ifdef USE_GL
  osystem_finishBricks();
#endif

  counter6 = 1;

  offset2 = 4;

  endOfGridPtr3 = endOfGridPtr = currentGrid + (gridSize - 32);

  do
  {
    temp = *(endOfGridPtr3 + (counter6 >> 3));
    temp2 = 7 - (counter6 & 7);

    temp3 = 1 << temp2;

    if (temp & temp3)
    {
      temp4 = READ_LE_U32(currentBll + offset2 - 4);
      ptr1 = currentBll + temp4;

      val1 = *ptr1;
      val2 = *(ptr1 + 1);

      val2 *= val1;

      val3 = *(char *) (ptr1 + 2);

      val3 *= val2;

      ptr2 = ptr1 + 5;

      counter7 = 0;

      while (counter7++ < val3)
      {
        val4 = READ_LE_U16(ptr2);
        if (val4 != 0)
        {
          WRITE_LE_U16(ptr2, READ_LE_U16(outPtr + val4 * 2 - 2));
        }
        ptr2 += 4;
      }
    }
    offset2 += 4;
  }while (++counter6 < 256);

 return (finalSize);
}
*/

int CreateMaskGph() {
	int i;

	for (i = 0;i < NUM_MAX_BRICKS;i++) {
		if (brickUsageTable[i]) {
			brickMaskTable[i] = (unsigned char*)malloc(brickSizeTable[i]);

			CalcGraphMsk(i, brickTable[i], brickMaskTable[i]);
		}
	}
}


void RazMem(unsigned char *ptr, int size) {
	int i;

	for (i = 0; i < size; i++) {
		*(ptr++) = 0;
	}
}
