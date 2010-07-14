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

#ifdef MEM_DEBUG // only for windows
#include<windows.h>
#include<Psapi.h>
#pragma comment(lib,"Psapi.lib")
#endif // MEM_DEBUG

#include "lba.h"


static void init(void) {
#ifdef MEM_DEBUG
	initMemorySystem();
#endif

	soundInit();

	InitProgram();

	if (useSound) {
		if (useSB)
			HQR_Midi = HQR_Init_Ressource("midi_sb.hqr",32000,2);
		else
			HQR_Midi = HQR_Init_Ressource("midi_mi.hqr",32000,2);

		if (!HQR_Midi)
			useSound = 0;
	}


	workVideoBuffer = Malloc(307700 * sizeof(byte));

	// check screenPtr

//  osystem_Flip(frontVideoBuffer);

#ifndef FASTDEBUG

#ifndef LBASTUDIO
	AdelineLogo();
#endif

#endif

	InitCDR("CD_LBA");

// todo: mettre les detection de problemes d'allocations...

	bufSpeak = Malloc(BUF_SPEAK_SIZE);   // was allocated in dos memory

	bufMemoSeek = Malloc(BUF_MEMOSEEK_SIZE);

	bufText = Malloc(BUF_TEXT_SIZE);

	bufOrder = Malloc(BUF_ORDER_SIZE);

	bufAni1 = bufAni2 = Malloc(BUF_ANIM_SIZE);

	InitBufferCube();

#if 0
#ifndef PRELOAD_ALL
	HQM_Init_Memory(BUF_HQM_SIZE);
#endif
#endif

	menuPal = LoadMalloc_HQR("ress.hqr", 0);
	convertPalToRGBA(menuPal, menuPalRGBA);

	shadowSprite = LoadMalloc_HQR("ress.hqr", 4);
	spriteActorData = LoadMalloc_HQR("ress.hqr", 3);

	lbaFont = LoadMalloc_HQR("ress.hqr", 1);
	SetFont(lbaFont, 2, 8);
	CoulFont(14);
	CoulDial(136, 143, 2);

#ifdef PRELOAD_ALL
	HQR_Fic = HQR_Init_RessourcePreload("file3d.hqr");
	HQR_Inventory = HQR_Init_RessourcePreload("invobj.hqr");
	//  HQR_Text = HQR_Init_RessourcePreload("text.hqr");
	HQR_Scenes = HQR_Init_RessourcePreload("scene.hqr");
	HQR_Sprites = HQR_Init_RessourcePreload("sprites.hqr");
	HQR_Anims = HQR_Init_RessourcePreload("anim.hqr");
	HQR_Bodies = HQR_Init_RessourcePreload("body.hqr");
	HQR_Samples = HQR_Init_RessourcePreload("samples.hqr");
	/*HQR_Grids = HQR_Init_RessourcePreload("lba_gri.hqr");
	HQR_Bll = HQR_Init_RessourcePreload("lba_bll.hqr");*/
#else
	HQR_Inventory = HQR_Init_Ressource("invobj.hqr", BUF_INVENTORY_SIZE, 30);
	HQR_Sprites = HQR_Init_Ressource("sprites.hqr", 300000, 120); // enough to hold all the sprites in mem
	HQR_Samples = HQR_Init_Ressource("samples.hqr", 4500000, 4500000 / 5000);
	HQR_Anims = HQR_Init_Ressource("anim.hqr", 450000, 600); // should be able to hold all the anims of the game
#endif

#ifdef GAME_DEBUG
#ifdef PRELOAD_ALL
	flagModelPtr = HQR_GetCopy(HQR_Bodies, 121);
	HQRM_Load("body.hqr", 121, &flagModelPtr); // load the flag model used to display tracks
#else
	HQRM_Load("body.hqr", 121, &flagModelPtr); // load the flag model used to display tracks
#endif
	loadGfxSub(flagModelPtr);
#endif

#ifdef GAME_DEBUG
	flagActor = &actors[99];

	flagActor->staticFlagsBF.bCanDrown = 0;
	flagActor->staticFlagsBF.bComputeCollisionWithBricks = 0;
	flagActor->staticFlagsBF.bComputeCollisionWithObj = 0;
	flagActor->staticFlagsBF.bDoesntCastShadow = 0;
	flagActor->staticFlagsBF.bIsCarrier = 0;
	flagActor->staticFlagsBF.bIsDead = 0;
	flagActor->staticFlagsBF.bIsFallable = 0;
	flagActor->staticFlagsBF.bIsPushable = 0;
	flagActor->staticFlagsBF.bIsSpriteActor = 0;
	flagActor->staticFlagsBF.bIsUsingClipping = 0;
	flagActor->staticFlagsBF.bIsUsingMiniZv = 0;
	flagActor->staticFlagsBF.bIsZonable = 0;
	flagActor->staticFlagsBF.bNoDisplay = 0;

	HQRM_Load("file3d.hqr", 0, &flagActor->entityDataPtr);
// InitBody(99,121);
#endif

	samplesLoaded = 1;

#ifdef LBASTUDIO
	reinitAll(1);
#else
#ifdef TXT_SCRIPTS
	reinitAll(1);
//  newGame();
	mainLoop();
#else
	FadeToBlack((char *) paletteRGBA);

	if (setup_lst == 0)   // switch pour les 2 version de l'ecran titre de LBA
		RessPict(49);
	else
		RessPict(12);

	WaitTime(3000);

	loadImageCrossFade(52); // Electronic Arts Logo

	WaitTime(1000);

	FadeToBlack((char *) paletteRGBA);

	PlayAnimFla("DRAGON3");

	Load_HQR("ress.hqr", workVideoBuffer, 14);

	CopyScreen(workVideoBuffer, frontVideoBuffer);

	osystem_Flip(frontVideoBuffer);

	FadeToPal((char *) menuPalRGBA);

	MainGameMenu();
#endif
#endif
}


static int lbaMain(int argc, char *argv[]) { // hello world !
//    LBA_engine *engine;

#ifdef MEM_DEBUG
	PROCESS_MEMORY_COUNTERS memInfo;
	GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(PROCESS_MEMORY_COUNTERS));

	staticMemoryUsage = memInfo.WorkingSetSize;
#endif

	osystem_init(argc, argv);
	printf("Success !\n");

#ifdef GAME_DEBUG
	debugger_init();
#endif

//  engine->osystem = osystem;
//  engine->_debugger.osystem = osystem;
//  engine->_debugger.engine = engine;
//  debugger_osystem = osystem;
	initVars(); // init the vars (move to the object constructor ?)

#ifdef PCLIKE
	startThreadTimer(); // the little trouble maker ! This one is responsible for the game timming
#endif

	init(); // startup the game engine !
//  }

//    delete(osystem);

	return(0); // that's all folks !
}

// main for the SDL part
int main(int argc, char *argv[]) {
	lbaMain(argc, argv);

	return 0;
}


void newGame(void) {
	int flagDisplayTextSave;

	stopMusic();
	if (needChangeRoom)
		return;
	if (chapter)
		return;

	flagDisplayTextSave = flagDisplayText;

	flagDisplayText = 1;

	Load_HQR("ress.hqr", workVideoBuffer, 15);  // Ecran de Twinsun (ecran 1 de l'intro)
	CopyScreen(workVideoBuffer, frontVideoBuffer);
	Load_HQR("ress.hqr", palette, 16);
	convertPalToRGBA(palette, paletteRGBA);

	osystem_crossFade((char *) frontVideoBuffer, (char *) paletteRGBA);
	osystem_setPalette(paletteRGBA);
// osystem_Flip(frontVideoBuffer);
// FadeToPal((char*)paletteRGBA);

	newGameVar4 = 0;
	newGameVar5 = 1;
	InitDial(2);
	newGame2();
	TestCoulDial(15);

#ifndef TXT_SCRIPTS
	printTextFullScreen(150);
	readKeyboard();

	if (skipIntro != 1) {
		// SetBackPal();
		Load_HQR("ress.hqr", workVideoBuffer, 17);  // Ecran de la citadelle (ecran 2 de l'intro)
		CopyScreen(workVideoBuffer, frontVideoBuffer);
		Load_HQR("ress.hqr", palette, 18);
		convertPalToRGBA(palette, paletteRGBA);

		osystem_crossFade((char *) frontVideoBuffer, (char *) paletteRGBA);
		osystem_setPalette(paletteRGBA);

		// osystem_Flip(frontVideoBuffer);
		// osystem_setPalette(paletteRGBA);
		printTextFullScreen(151);

		readKeyboard();
		if (skipIntro != 1) {
			// SetBackPal();
			Load_HQR("ress.hqr", workVideoBuffer, 19);  // Ecran du reve de Twisen (ecran 3 de l'intro)
			CopyScreen(workVideoBuffer, frontVideoBuffer);
			Load_HQR("ress.hqr", palette, 20);

			convertPalToRGBA(palette, paletteRGBA);

			osystem_crossFade((char *) frontVideoBuffer, (char *) paletteRGBA);
			osystem_setPalette(paletteRGBA);

			// osystem_Flip(frontVideoBuffer);
			// osystem_setPalette(paletteRGBA);

			printTextFullScreen(152);
		}
	}
#endif
	newGameVar5 = 0;
	newGame4();
	newGameVar4 = 1;
	FadeToBlack((char *) paletteRGBA);
	Cls();
	osystem_Flip(frontVideoBuffer);
	playMidi(1);

#ifndef TXT_SCRIPTS
	PlayAnimFla("INTROD");
#endif
	SetBackPal();
	Cls();
	osystem_Flip(frontVideoBuffer);

	flagDisplayText = flagDisplayTextSave;  // on remet le flag comme il était au debut
}

void newGame2(void) {      /* ok */
	dialogueBoxLeft = 8;
	dialogueBoxTop = 8;
	dialogueBoxRight = 631;

	dialogueBoxBottom = 471;
	dialogueBoxParam1 = 11;
	dialogueBoxParam2 = 607;
}

void newGame4(void) {      /* ok */
	dialogueBoxLeft = 16;
	dialogueBoxTop = 334;
	dialogueBoxRight = 623;
	dialogueBoxBottom = 463;
	dialogueBoxParam1 = 3;
	dialogueBoxParam2 = 591;
}

void TestCoulDial(short int param) {
	progressiveTextStepSize = -1;
	progressiveTextBufferSize = 14;
	progressiveTextStartColor = param << 4;
	progressiveTextStopColor = (param << 4) + 12;
}
