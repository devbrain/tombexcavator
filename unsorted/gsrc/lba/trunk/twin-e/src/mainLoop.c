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

#ifdef WIN32
#include "sdl.h"
#endif

int cptime = 0;

int mainLoop(void) {
	requestBackgroundRedraw = 1;
	lockPalette = 1;
	setActorAngle(0, -256, 5, &mainLoopVar1);

	osystem_mainLoop();

	return 0;
}

int mainLoopInteration(void) {
	int temp;
	int i;
	int currentTime;
	int frameTime;
	int textBank;

	{
		currentTime = lba_time;
mainLoopStart:

		readKeyboard();
		if (mainLoopVar4 > 500)
			waitRetrace();

		if (needChangeRoom != -1)
			ChangeCube();

		mainLoopVar6 = mainLoopVar5;
		key = printTextVar12;
		mainLoopVar5 = key1;
		mainLoopVar7 = skipIntro;

		if (gameStaffIsDisplayed != 0) {
			if (getCurrentlyPlayingMusic() != 8)
				playCDtrack(8);
			if (skipIntro != 0)

				return (0);
			if (printTextVar12 != 0)
				return (0);
			if (key1 != 0)
				return (0);
		} else {
			if (lockPalette == 0) {
				// debut des inputs

				if (skipIntro == 1 && twinsen->life > 0 && twinsen->costumeIndex != -1 && !twinsen->staticFlagsBF.bNoDisplay) { // press ESC
					TestRestoreModeSVGA(1);
					freezeTime();
					if (!makeGiveUpMenu()) {
						unfreezeTime();
						fullRedraw(1);
					} else {
						unfreezeTime();
						fullRedraw(1);
						freezeTime();
						SaveGame();
						breakmainLoop = true;
						unfreezeTime();
						return (0);
					}
				}
				if (fkeys == 6) { // F6
					temp = languageCD1;
					freezeTime();
					TestRestoreModeSVGA(1);
					soundMenuData[5] = 15;
					languageCD1 = 0;
					InitDial(0);
					optionMenu();
					languageCD1 = temp;
					InitDial(currentTextBank + 3);

					if (currentRoom == 80) {
						if (vars[90] == 1)
							playCDtrack(8);
						else
							PlayMusic(roomMusic);
					} else {
						PlayMusic(roomMusic);
					}
					unfreezeTime();
					fullRedraw(1);
					fkeys = 0;
				}
				mainLoopVar9 = -1;
				if ((byte) mainLoopVar5 & 0x20 && twinsen->costumeIndex != -1 && twinsen->comportement == 1) { // inventory menu
					freezeTime();
					TestRestoreModeSVGA(1);
					Inventory();

					// process object usage
					switch (mainLoopVar9) {
					case 0: { // use holomap
							//processHolomap();
							lockPalette = 1;
							break;
						}
					case 1: { // use magical ball
							if (usingSword == 1) {
								InitBody(0, 0);
							}

							usingSword = 0;

							break;
						}
					case 2: { // use sword
							if (twinsen->body == 2) {
								if (comportementHero == 4) {
									SetComportement(0);
								}

								InitBody(2, 0);
								InitAnim(24, 1, 0, 0);

								usingSword = 1;
							}
							break;
						}
					case 5: { // Bù book
							// todo: implement
							break;
						}
					case 12: { // protopak
							if (vars[6]) {
								twinsen->body = 0;
							} else {
								twinsen->body = 1;
							}

							if (comportementHero == 4) {
								SetComportement(0);
							} else {
								SetComportement(4);
							}
							break;
						}
					case 14: { // pingoin
							break;
						}
					case 26: { // text
							break;
						}
					case 27: { // clover
							if (twinsen->life < 50) {
								if (numClover > 0) {
									twinsen->life = 50;
									magicPoint = magicLevel * 20;

									numClover--;

									addOverlayObject(3, 27, 0, 0, 0, 0, 3);
								}
							}
							break;
						}
					}

					unfreezeTime();
					fullRedraw(1);
				}
				if ((byte) mainLoopVar5 & 4 && twinsen->costumeIndex != -1 && twinsen->comportement == 1) { // comportement menu
					freezeTime();
					TestRestoreModeSVGA(1);
					processComportementMenu();
					unfreezeTime();
					fullRedraw(1);
				}

				if (fkeys >= 1  && fkeys <= 4  && twinsen->costumeIndex != -1 && twinsen->comportement == 1 && !(fkeys == comportementHero + 1)) { // F1-F4 - only if set a diferent behaviour than the current one
//           freezeTime(); // don-t need to freeze the time
					TestRestoreModeSVGA(1);

					if (cptime != 0) { // clear if have something already writed in the screen
						blitRectangle(5, 446, 350, 479, (char*)workVideoBuffer, 5, 446, (char*)frontVideoBuffer);
						osystem_CopyBlockPhys(frontVideoBuffer, 5, 446, 350, 479);
					}
					// Added: Implemented with translations like LBA2 -----
					CoulFont(15);
					textBank = currentTextBank;
					currentTextBank = -1;
					InitDial(0);
					GetMultiText(fkeys - 1, dataString);
					Font(5, 446, dataString);
					osystem_CopyBlockPhys(frontVideoBuffer, 5, 446, 350, 479);
					currentTextBank = textBank;
					InitDial(currentTextBank + 3);
					// ---------------------------------------------------
					cptime = currentTime;
					SetComportement(fkeys - 1);

					// LBA1 type ----- Disabled
					//
					// processComportementMenu();
					// unfreezeTime();

					fkeys = 0;
				}

				// Using J to Enable Proto-Pack
				if (mainLoopVar7 == 'j' && vars[12] == 1) { // only if its in the inventory
					// Not implemented in the origianl version ---------------

					if (cptime != 0) { // clear if have something already writed in the screen
						blitRectangle(5, 446, 350, 479, (char*)workVideoBuffer, 5, 446, (char*)frontVideoBuffer);
						osystem_CopyBlockPhys(frontVideoBuffer, 5, 446, 350, 479);
					}

					CoulFont(15);
					Font(5, 446, "Proto-Pack");
					osystem_CopyBlockPhys(frontVideoBuffer, 5, 446, 200, 479);
					// -------------------------------------------------------
					cptime = currentTime;

					if (vars[6]) {
						twinsen->body = 0;
					} else {
						twinsen->body = 1;
					}

					if (comportementHero == 4) {
#ifdef GAME_DEBUG
						printf("Stop using Proto-Pack!");
#endif
						SetComportement(0);
					} else {
#ifdef GAME_DEBUG
						printf("Now using Proto-Pack!");
#endif
						SetComportement(4);
					}
				}

				// Time to display the behaviour text showed in the previous condition.
				if ((lba_time - cptime) > 100 && cptime) {
					blitRectangle(5, 446, 350, 479, (char*)workVideoBuffer, 5, 446, (char*)frontVideoBuffer);
					osystem_CopyBlockPhys(frontVideoBuffer, 5, 446, 350, 479);
					cptime = 0;
				}


				if (fkeys == 12) { // F12 for FullScreen
					// TODO: Full Screen
					//SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE|SDL_FULLSCREEN);
					fkeys = 0;
				}


				if ((byte) mainLoopVar5 & 2 && disableScreenRecenter == 0) { // recenter screen
					newCameraX = actors[currentlyFollowedActor].X >> 9;
					newCameraZ = actors[currentlyFollowedActor].Y >> 8;
					newCameraY = actors[currentlyFollowedActor].Z >> 9;
					requestBackgroundRedraw = 1;
					//needChangeRoom = 119;
					//needChangeRoom=currentRoom+1;
				}


#ifdef GAME_DEBUG
				if (mainLoopVar7 == 'r') { // changed because proto-pack key
					needChangeRoom = currentRoom + 1;
				}
				if (mainLoopVar7 == 'f') { // changed because prto-pack key
					needChangeRoom = currentRoom - 1;
				}

				if (mainLoopVar7 == 'h') {
					InitAnim(ANIM_static, 0, 255, 0);
				}

				if (mainLoopVar7 == 't') {
					printf("StoryState: %d\n", ++chapter);
				}
				if (mainLoopVar7 == 'g') {
					printf("StoryState: %d\n", --chapter);
				}
#endif

				/***********************************************/
				/*
				 * camera debugger
				 */
				/***********************************************/


				/*      if (printTextVar12 & 2)      // x-- -> bas
				      {
				      newCameraY++;
				      requestBackgroundRedraw = 1;
				      }

				      if (printTextVar12 & 1)      // x++ -> haut
				      {
				      newCameraY--;
				      requestBackgroundRedraw = 1;
				      }

				      if (printTextVar12 & 4)      // y-- -> gauche
				      {
				      newCameraX--;
				      requestBackgroundRedraw = 1;
				      }

				      if (printTextVar12 & 8)      // y++ -> droite
				      {
				      newCameraX++;
				      requestBackgroundRedraw = 1;
				      }
				    */

				/**********************************************/
				// angle debug

				// printf("from %d to %X\n",twinsen->time.from,twinsen->time.to);

				/**********************************************/

				if (mainLoopVar7 == 'h' && vars[0] == 1 && vars[70] == 0) { // draw holomap
					freezeTime();
					TestRestoreModeSVGA(1);
					// processHolomap();
					lockPalette = 1;
					unfreezeTime();
					fullRedraw(1);
				}

				if (mainLoopVar7 == 'p') {
					// pauseSound();
					freezeTime();
					if (!drawInGameTransBox) {
#ifdef GAME_DEBUG
						printf("Game in Pause...");
#endif
						CoulFont(15);
						Font(5, 446, "Pause"); // Don't have an entry in the Text Bank
						osystem_CopyBlockPhys(frontVideoBuffer, 5, 446, 100, 479);
					}
					readKeyboard();
					while (skipIntro) {
						readKeyboard();
					};
					while (!skipIntro) {
						readKeyboard();
					};
					do {
						readKeyboard();
					} while (!skipIntro && !printTextVar12 && key1);
					while (skipIntro) {
						readKeyboard();
					};
					if (!drawInGameTransBox) {
						blitRectangle(5, 446, 100, 479, (char *) workVideoBuffer, 5, 446, (char *) frontVideoBuffer);
						osystem_CopyBlockPhys(frontVideoBuffer, 5, 446, 100, 479);
					}
#ifdef GAME_DEBUG
					printf("Game Resumed!");
#endif
					unfreezeTime();
					// resumeSound();
				}
				// il manque un process des input là pour F5

				// fin des inputs
			}
		}
		mainLoopVar17 = GetRealValue(&mainLoopVar1);
		if (!mainLoopVar17)
			mainLoopVar17 = 1;

		setActorAngle(0, -256, 5, &mainLoopVar1);
		disableScreenRecenter = 0;
		// playRoomSamples();

		for (i = 0; i < numActorInRoom; i++) {
			actors[i].hitBy = -1;
		}

		GereExtras();

		// 2nd boucle de process des acteurs

		assert(frontVideoBufferbis == frontVideoBuffer);

		for (i = 0; i < numActorInRoom; i++) {
			if (!(actors[i].dynamicFlagsBF.bUnk0020)) {
				// printf("Processing actor %d...\n",i);
				if (actors[i].life == 0) {
					if (i == 0) {
						InitAnim(ANIM_landDeath, 4, 0, 0);  // play twinsen death anim
						actors[i].comportement = 0;
					} else {
						HQ_3D_MixSample(37, rand() % 2000 + 3096, 1, actors[i].X, actors[i].Y, actors[i].Z);

						if (i == currentPingouin) {
							printf("Pinguoin exploded ! Implement\n");
							exit(1);
						}
					}

					if (actors[i].field_10 & 0x1F0 && !(actors[i].field_10 & 1)) {
						GiveExtraBonus(&actors[i]);
					}
				}
				DoDir(i);

				actors[i].field_20 = actors[i].X;
				actors[i].field_22 = actors[i].Y;
				actors[i].field_24 = actors[i].Z;

				if (actors[i].positionInMoveScript != -1) {
					DoTrack(i);
				}

				DoAnim(i);

				if (actors[i].staticFlagsBF.bIsZonable) {
					CheckZoneSce(&actors[i], i);
				}

				if (actors[i].positionInActorScript != -1) {
					runActorScript(i);
				}

				//if(brutalExit==-1)
				//      return(-1);

				if (actors[i].staticFlagsBF.bCanDrown) { // drown

				}

				if (actors[i].life <= 0) { // if actor dead...
					if (!i) {
						if (actors[i].dynamicFlagsBF.bUnk0004) {
							if (numClover > 0) { // auto use clover
								twinsen->X = newTwinsenX;
								twinsen->Y = newTwinsenZ;
								twinsen->Z = newTwinsenY;

								needChangeRoom = currentRoom;
								magicPoint = magicLevel * 20;

								newCameraX = (twinsen->X >> 9);
								newCameraZ = (twinsen->Y >> 8);
								newCameraY = (twinsen->Z >> 9);

								twinsenPositionModeInNewCube = 3;

								twinsen->life = 50;
								requestBackgroundRedraw = 1;
								lockPalette = 1;

								numClover--;

								cropBottomScreen = i; // !!!???
							} else { // game over ...
								// TODO: play Game Over anim. Model 20 in Ress file
								breakmainLoop = true;
								printf("Game over...\n");
							}
						}
					} else {
						CheckCarrier(i);
						actors[i].dynamicFlagsBF.bUnk0020 = 1;
						actors[i].costumeIndex = -1;
						actors[i].zone = -1;
					}
				}

				if (needChangeRoom != -1)
					goto mainLoopStart;

			}
		}
#ifdef GAME_DEBUG
#ifdef PCLIKE
		if (debugger_processDebug())
			requestBackgroundRedraw = 1;
#endif
#endif

		assert(frontVideoBufferbis == frontVideoBuffer);

		if (!disableScreenRecenter) {
			projectPositionOnScreen(actors[currentlyFollowedActor].X - (newCameraX << 9), actors[currentlyFollowedActor].Y - (newCameraZ << 8), actors[currentlyFollowedActor].Z - (newCameraY << 9));

			if (projectedPositionX < 80 || projectedPositionX > 539 || projectedPositionY < 80 || projectedPositionY > 429) {
				newCameraX = ((actors[currentlyFollowedActor].X + 0x100) >> 9) + (((actors[currentlyFollowedActor].X + 0x100) >> 9) - newCameraX) / 2;
				newCameraZ = (actors[currentlyFollowedActor].Y) >> 8;
				newCameraY = ((actors[currentlyFollowedActor].Z + 0x100) >> 9) + (((actors[currentlyFollowedActor].Z + 0x100) >> 9) - newCameraY) / 2;

				if (newCameraX >= 64)
					newCameraX = 63;

				if (newCameraY >= 64)
					newCameraY = 63;

				requestBackgroundRedraw = 1;
			}
		}

		fullRedraw(requestBackgroundRedraw);
		requestBackgroundRedraw = 0;
		counter++;

		frameTime = lba_time - currentTime;

#ifndef PCLIKE
		lba_time += 2;
#endif

	}

	return 0;
}

void reinitAll(int save) {
	UnSetClip();
	reinitVar1 = 896;
	reinitVar2 = 950;
	reinitAll1();
	reinitVars();
	reinitAll3();
	newTwinsenX = 0x2000;
	newTwinsenZ = 0x1800;
	newTwinsenY = 0x2000;
	currentRoom = -1;
	brutalExit = -1;
	numClover = 0;
	numCloverBox = 2;
	currentPingouin = -1;
	needChangeRoom = 0;
	magicLevel = 0;
	magicPoint = 0;
	numCoin = 0;
	numKey = 0;
	chapter = 0;
	usingSword = 0;
	currentTextBank = 0;
	fuel = 0;
	cropBottomScreen = 0;
	currentlyFollowedActor = 0;
	startupAngleInCube = 0;
	comportementHero = 0;
	startupComportementHeroInCube = 0;

	if (save == -1) {
		LoadGame();
		if (newTwinsenX == -1) {
			twinsenPositionModeInNewCube = 0;
		}
	}

	/*
	 * FadeToBlack((char*)menuPalRGBA); Cls(); osystem_Flip(frontVideoBuffer);
	 */
}

//S2336.LBA

void LoadGame(void) {
	FILE* fileHandle;
	unsigned char data;
	char* namePtr;

	fileHandle = OpenRead("SAVE.LBA");

	namePtr = playerName;

	Read(fileHandle, (char*)&data, 1);

	do {
		Read(fileHandle, (char*)&data, 1);
		*(namePtr++) = data;
	} while (data);

	Read(fileHandle, (char*)&data, 1);
	Read(fileHandle, (char*)vars, data);
	Read(fileHandle, (char*)&needChangeRoom, 1);
	Read(fileHandle, (char*)&chapter, 1);
	Read(fileHandle, (char*)&comportementHero, 1);
	startupComportementHeroInCube = comportementHero;
	Read(fileHandle, (char*)&(twinsen->life), 1);
	Read(fileHandle, (char*)&numCoin, 2);
	Read(fileHandle, (char*)&magicLevel, 1);
	Read(fileHandle, (char*)&magicPoint, 1);
	Read(fileHandle, (char*)&numCloverBox, 1);
	Read(fileHandle, (char*)&newTwinsenX, 2);
	Read(fileHandle, (char*)&newTwinsenZ, 2);
	Read(fileHandle, (char*)&newTwinsenY, 2);
	Read(fileHandle, (char*)&(twinsen->angle), 2);
	startupAngleInCube = twinsen->angle;
	Read(fileHandle, (char*)&(twinsen->body), 1);
	Read(fileHandle, (char*)&data, 1);
	Read(fileHandle, (char*)GV14, data);
	Read(fileHandle, (char*)&fuel, 1);
	Read(fileHandle, (char*)&data, 1);
	Read(fileHandle, (char*)itemUsed, data);
	Read(fileHandle, (char*)&numClover, 1);
	Read(fileHandle, (char*)&usingSword, 1);

	Close(fileHandle);

	currentRoom = -1;
	twinsenPositionModeInNewCube = 3;
}

void reinitAll1(void) {
	configureOrthoProjection(311, 240, 512);
	setSomething2(0, 0, 0);
	setSomething3(0, 0, 0);
	SetLightVector(reinitVar1, reinitVar2, 0);
}

void TestRestoreModeSVGA(int arg_0) {
	if (!drawInGameTransBox)
		return;

	if (useSamples) {
		// code de son non géré...
	}

	mainLoop2sub1();

	if (useAlternatePalette)
		osystem_setPalette(menuPalRGBA);
	else
		osystem_setPalette(paletteRGBA);

	drawInGameTransBox = 0;

	// code son non gégé...

	if (!arg_0)
		fullRedraw(1);
}

void mainLoop2sub1(void) {
	// code dpmi non géré.

	initVideoVar1 = -1;
}

void waitRetrace(void) {
#ifdef WIN32
	int temp = SDL_GetTicks();

	while (SDL_GetTicks() - temp < 15) {
	}
#endif
}

void freezeTime(void) {
	if (!time1)
		time3 = lba_time;

	time1++;

}

void unfreezeTime(void) {
	--time1;

	if (time1 == 0)
		lba_time = time3;
}

int makeGiveUpMenu(void) { // process le menu "continuer ou abandonner"
	int saveLangue;
	int temp;

	CopyScreen(frontVideoBuffer, workVideoBuffer);
	HQ_StopSample();

	do {
		saveLangue = languageCD1;
		languageCD1 = 0;
		InitDial(0);
		temp = processMenu(subMenu2Data);
		languageCD1 = saveLangue;
		InitDial(currentTextBank + 3);
	} while (temp != 27 && temp != 28);

	if (temp == 27)
		return (1);
	if (temp == 28)
		return (0);

	return (0);
}

void reinitVars(void) {
	int i;

	// reinitExtraObjectList();

	for (i = 0; i < 10; i++)
		overlayObjectList[i].field_0 = -1;

	for (i = 0; i < 80; i++)
		cubeFlags[i] = 0;

	for (i = 0; i < 255; i++)
		vars[i] = 0;

	for (i = 0; i < 28; i++)
		itemUsed[i] = 0;

	sceneVar2.field_0 = -1;
	sceneVar2.field_2 = -1;
	sceneVar2.field_4 = -1;
	sceneVar2.field_6 = -1;

	sceneVar3.field_0 = 1;
	sceneVar3.field_2 = 1;
	sceneVar3.field_4 = 1;
	sceneVar3.field_6 = 1;

	sceneVar4.field_0 = 1;
	sceneVar4.field_2 = 1;
	sceneVar4.field_4 = 1;
	sceneVar4.field_6 = 1;

	for (i = 0; i < 150; i++)
		GV14[i] = 0;

	numActorInRoom = 0;
	currentPositionInBodyPtrTab = 0;
	numOfZones = 0;

	numFlags = 0;
}

void reinitAll3(void) {
	resetActor(0);

	magicBallIdx = -1;
	numCloverBox = 2;
	numClover = 2;
	numCoin = 0;
	numKey = 0;
	magicPoint = 0;
	usingSword = 0;
	twinsen->body = 0;
	twinsen->life = 50;
	twinsen->talkColor = 4;
}

void DrawObj3D(short int arg_0, short int arg_4, short int arg_8, short int arg_C,
			   short int arg_10, short int arg_14, unsigned char *costumePtr) {
	int var_4;
	int temp1;
	int temp2;
	short int temp;

	var_4 = arg_8;

	temp1 = arg_C + arg_4;
	temp1 >>= 1;

	temp2 = arg_8 + arg_0;
	temp2 >>= 1;

	configureOrthoProjection(temp2, temp1, 0);
	SetClip(arg_0, arg_4, var_4, arg_C);

	if (arg_14 == -1) {
		temp = GetRealAngle(&timeVar);
		if (timeVar.numOfStep == 0) {
			setActorAngleSafe(temp, temp - 256, 50, &timeVar);
		}
		AffObjetIso(0, arg_10, 0, 0, temp, 0, costumePtr);
	} else
		AffObjetIso(0, arg_10, 0, 0, arg_14, 0, costumePtr);

}

void SetClipLBA(int left, int top, int right, int bottom) {
	if (left < 0)
		left = 0;

	textWindowLeft = left;

	if (top < 0)
		top = 0;
	textWindowTop = top;

	if (right >= largeurEcran)
		right = largeurEcran - 1;
	textWindowRight = right;

	if (bottom >= hauteurEcran)
		bottom = hauteurEcran - 1;
	textWindowBottom = bottom;
}

void Box(int left, int top, int right, int bottom, unsigned char e) {

	unsigned char *ptr;

	int offset;

	int x;
	int y;

	if (left > textWindowRight)
		return;
	if (right < textWindowLeft)

		return;
	if (top > textWindowBottom)
		return;
	if (bottom < textWindowTop)
		return;

	// cropping

	offset = -((right - left) - largeurEcran);

	ptr = frontVideoBuffer + screenLockupTable[top] + left;

	for (x = top; x < bottom; x++) {
		for (y = left; y < right; y++) {

			*(ptr++) = e;
		}
		ptr += offset;
	}

}

void DoDir(int actorNum) {
	actor *lactor;

	lactor = &actors[actorNum];

	if (lactor->costumeIndex == -1)
		return;

	if (lactor->dynamicFlagsBF.bUnk0100) {
		short int tempAngle = 0;

		if (lactor->comportement != 1)
			return;

		if (key & 4)
			tempAngle = 0x100;

		if (key & 8)
			tempAngle = -0x100;

		ManualRealAngle(lactor->angle, lactor->angle + tempAngle, lactor->speed, &lactor->time);

		twinsenKey = key;
	} else {
		if (!(lactor->staticFlagsBF.bIsSpriteActor)) {
			if (lactor->comportement != 1) {
				lactor->angle = GetRealAngle(&lactor->time);
			}
		}

		if (lactor->comportement > 6)
			return;

		switch (lactor->comportement) {
		case 0: // NO_MOVE
			break;
		case 1: // MOVE_MANUAL
			if (!actorNum) { // if it's twinsen
				action = 0;

				// ADDED: Like LBA2 ----
				if (mainLoopVar7 == 'w') {
					action = 1;
					break;
				}
				//----------------------

				switch (comportementHero) {
				case 0: { // normal
						if (mainLoopVar5 & 1) { // action button
							action = 1;
						}
						break;
					}
				case 1: { // sportif
						if (mainLoopVar5 & 1) { // jump
							InitAnim(ANIM_jump, 1, 0, actorNum);
						}
						break;
					}
				case 2: { // agressive
						if (mainLoopVar5 & 1) {
							if (autoAgressivity) {
								twinsenMoved = 1;

								lactor->angle = GetRealAngle(&lactor->time);

								//   if(mainLoopVar6 == 1 || lactor->anim == 0) // TODO: figure it out. mainLoopVar6 may be related to the fact that the action key was released
								if (lactor->anim == 0) {
									char agressivityMove = (rand() % 3);

									switch (agressivityMove) {
									case 0: {
											InitAnim(ANIM_kick, 1, 0, actorNum);
											break;
										}
									case 1: {
											InitAnim(ANIM_rightPunch, 1, 0, actorNum);
											break;
										}
									case 2: {
											InitAnim(ANIM_leftPunch, 1, 0, actorNum);
											break;
										}
									default: {
											printf("Bad random in agressivity !\n");
											break;
										}
									}
								}

							} else {
								if (key & 8) {
									InitAnim(ANIM_rightPunch, 1, 0, actorNum);
								}

								if (key & 4) {
									InitAnim(ANIM_leftPunch, 1, 0, actorNum);
								}

								if (key & 1) {
									InitAnim(ANIM_kick, 1, 0, actorNum);
								}
							}
						}
						break;
					}
				case 3: { // discret
						if (mainLoopVar5 & 1) {
							InitAnim(ANIM_hide, 0, 255, 0);
						}
						break;
					}
				}

				if (mainLoopVar5 & 8 && !vars[70]) {
					if (usingSword == 0) {
						if (vars[1] == 1) { // have magic ball
							if (magicBallIdx == -1) {
								InitAnim(ANIM_throwBall, 1, 0, actorNum);
							}

							twinsenMoved = 1;
							lactor->angle = GetRealAngle(&lactor->time);
						}
					} else {
						if (vars[2] == 1) { // have sword
							if (lactor->body != 2) {
								InitBody(2, 0);
							}

							InitAnim(ANIM_swordAttack, 1, 0, actorNum);

							twinsenMoved = 1;
							lactor->angle = GetRealAngle(&lactor->time);
						}
					}
				}

			}

			if (mainLoopVar5 == 0 || action != 0) {
				short int tempAngle;
				if (key & 3)  // if continue walking
					twinsenMoved = 0; // don't break animation

				if (key != twinsenKey || mainLoopVar5 != twinsenKey2) {
					if (twinsenMoved != 0) {
						InitAnim(ANIM_static, 0, 255, actorNum);
					}
				}

				twinsenMoved = 0;

				if (key & 1) { // walk forward
					if (currentActorInZoneProcess == 0) {
						InitAnim(ANIM_walk, 0, 255, actorNum);
					}
					twinsenMoved = 1;
				}

				if (key & 2) { // walk backward
					InitAnim(ANIM_walkBackward, 0, 255, actorNum);
					twinsenMoved = 1;
				}

				if (key & 4) { // turn left
					twinsenMoved = 1;
					if (lactor->anim == 0) {
						InitAnim(ANIM_turnLeft, 0, 255, actorNum);
					} else {
						if (!(lactor->dynamicFlagsBF.bUnk0080)) {
							lactor->angle =
								GetRealAngle(&lactor->time);
						}
					}
				}

				if (key & 8) { // turn right
					twinsenMoved = 1;
					if (lactor->anim == 0) {
						InitAnim(ANIM_turnRight, 0, 255, actorNum);
					} else {
						if (!(lactor->dynamicFlagsBF.bUnk0080)) {
							lactor->angle = GetRealAngle(&lactor->time);
						}
					}
				}

				tempAngle = 0;

				if (key & 4) {
					tempAngle = 0x100;
				}

				if (key & 8) {
					tempAngle = -0x100;
				}

				ManualRealAngle(lactor->angle, lactor->angle + tempAngle,
								lactor->speed, &lactor->time);

				twinsenKey = key;
				twinsenKey2 = mainLoopVar5;

			}
			break;
		case 2: { //MOVE_FOLLOW
				int tempAngle;

				assert(lactor->followedActor != -1);

				tempAngle = GetAngle(lactor->X, lactor->Z, actors[lactor->followedActor].X, actors[lactor->followedActor].Z);

				if (lactor->staticFlagsBF.bIsSpriteActor) {
					lactor->angle = tempAngle;
				} else {
					ManualRealAngle(lactor->angle, tempAngle, lactor->speed, &lactor->time);
				}
				break;
			}

		case 3: { //MOVE_TRACK
				if (lactor->positionInMoveScript == -1) {
					lactor->positionInMoveScript = 0;
				}
			}
			break;
		case 4: // MOVE_FOLLOW_2
			break;
		case 5: // MOVE_TRACK_ATTACK
			break;
		case 6: { // MOVE_SAME_XZ
				lactor->X = actors[lactor->followedActor].X;
				lactor->Z = actors[lactor->followedActor].Z;
			}
			break;
		case 7: { // MOVE_RANDOM
				if (!(lactor->dynamicFlagsBF.bUnk0080)) {
					if (lactor->field_3 & 0x80) {
						ManualRealAngle(lactor->angle, (((rand() & 0x100) + (lactor->angle - 0x100)) & 0x3FF), lactor ->speed, &lactor->time);

						lactor->cropLeft = rand() % 300 + lba_time + 300;

						InitAnim(0, 0, 0xFF, actorNum);
					}

					if (lactor->time.numOfStep == 0) {
						InitAnim(1, 0, 0xFF, actorNum);

						if (lba_time > lactor->cropLeft) {
							ManualRealAngle(lactor->angle, (((rand() & 0x100) + (lactor->angle - 0x100)) & 0x3FF) , lactor->speed, &lactor->time);

							lactor->cropLeft = rand() % 300 + lba_time + 300;
						}
					}
				}
			}
			break;
		default:
			printf("Unhandled comportement %d in update actors\n", lactor->comportement);
			exit(1);
		}
	}
}

boolean test = false;

void DoAnim(int actorNum) {
	actor *lactor;
	char *animPtr;
	short int var_10;

	short int var_C;
	short int var_4;
	short int var_8;

	lactor = &actors[actorNum];

	currentlyProcessedActorNum = actorNum;
	processActorVar1 = lactor;

	if (lactor->costumeIndex == -1)
		return;

	processActorVar2 = lactor->field_20;
	processActorVar3 = lactor->field_22;
	processActorVar4 = lactor->field_24;

	if (lactor->staticFlagsBF.bIsSpriteActor) { // is sprite actor
		if (lactor->field_66) {
			lactor->dynamicFlagsBF.bUnk0002 = 1;
		}

		processActorX = lactor->X;
		processActorZ = lactor->Y;
		processActorY = lactor->Z;

		if (!(lactor->dynamicFlagsBF.bUnk0100)) { // actor freeze movement
			if (lactor->speed) {
				int dx;

				dx = GetRealValue(&lactor->time);

				if (!dx) {
					if (lactor->time.to > 0) {
						dx = 1;
					} else {
						dx = -1;
					}
				}

				Rotate(dx, 0, lactor->field_78);

				processActorZ = lactor->Y - destZ;

				Rotate(0, destX, lactor->angle);

				processActorX = lactor->X + destX;
				processActorY = lactor->Z + destZ;

				setActorAngle(0, lactor->speed, 50, &lactor->time);

				if (lactor->dynamicFlagsBF.bIsMoving) {
					if (lactor->doorStatus) { // is oppening
						var_10 = lactor->doorStatus;
						if (Distance2D(processActorX, processActorY, lactor->lastX, lactor->lastY) >= var_10) {
							if (lactor->angle == 0) {
								processActorY = lactor->lastY + lactor->doorStatus;
							} else if (lactor->angle == 0x100) {
								processActorX = lactor->lastX + lactor->doorStatus;
							} else if (lactor->angle == 0x200) {
								processActorY = lactor->lastY - lactor->doorStatus;
							} else if (lactor->angle == 0x300) {
								processActorX = lactor->lastX - lactor->doorStatus;
							}

							lactor->dynamicFlagsBF.bIsMoving = 0;
							lactor->speed = 0;
						}
					} else { // is closing
						int tempVar = 0;

						if (lactor->angle == 0) {
							if (processActorY <= lactor->lastY)
								tempVar = 1;
						} else if (lactor->angle == 0x100) {
							if (processActorX <= lactor->lastX)
								tempVar = 1;
						} else if (lactor->angle == 0x200) {
							if (processActorY >= lactor->lastY)
								tempVar = 1;
						} else if (lactor->angle == 0x300) {
							if (processActorX >= lactor->lastX)
								tempVar = 1;
						}

						if (tempVar) {
							processActorX = lactor->lastX;
							processActorZ = lactor->lastZ;
							processActorY = lactor->lastY;

							lactor->dynamicFlagsBF.bIsMoving = 0;
							lactor->speed = 0;
						}
					}
				}
			}

			if (lactor->staticFlagsBF.bIsPushable) {
				processActorX += lactor->lastX;
				processActorZ += lactor->lastZ;
				processActorY += lactor->lastY;

				if (lactor->staticFlagsBF.bIsUsingMiniZv) {
					processActorX = ((processActorX / 128) * 128);
					processActorY = ((processActorY / 128) * 128);
				}

				lactor->lastX = 0;
				lactor->lastZ = 0;
				lactor->lastY = 0;

			}
		}

	} else {   // not sprite actor
		if (lactor->previousAnimIndex != -1) {
			int keyFramePassed;
			animPtr = (char *) HQR_Get(HQR_Anims, lactor->previousAnimIndex);

			keyFramePassed = SetInterDepObjet(lactor->animPosition, animPtr, (char*)bodyPtrTab[lactor->costumeIndex], &lactor->animTimerData);

			if (processActorVar5) {
				lactor->dynamicFlagsBF.bUnk0080 = 1;
			} else {
				lactor->dynamicFlagsBF.bUnk0080 = 0;
			}

			lactor->angle = (lactor->angle + processActorVar6 - lactor->lastRotationSpeed) & 0x3FF;
			lactor->lastRotationSpeed = processActorVar6;

			Rotate(currentX, currentY, lactor->angle);

			currentX = destX; // dest
			currentY = destZ;

			processActorX = lactor->X + currentX - lactor->lastX;
			processActorZ = lactor->Y + currentZ - lactor->lastZ;
			processActorY = lactor->Z + currentY - lactor->lastY;

			lactor->lastX = currentX;
			lactor->lastZ = currentZ;
			lactor->lastY = currentY;

			lactor->dynamicFlagsBF.bUnk0004 = 0;
			lactor->dynamicFlagsBF.bUnk0008 = 0;

			if (keyFramePassed) { // if keyFrame
				lactor->animPosition++;
				if (lactor->animExtraData) { // if actor play sound attached to animation
					GereAnimAction(lactor, actorNum);
				}

				var_10 = lactor->animPosition;
				if (var_10 == GetNbFramesAnim(animPtr)) { // end of animation ?
					lactor->dynamicFlagsBF.bUnk0002 = 0;

					if (lactor->field_78 == 0) {
						lactor->animPosition = GetBouclageAnim(animPtr);
					} else {
						var_C = actorNum;
						lactor->anim = lactor->field_2;
						lactor->previousAnimIndex = getAnimIndexForBody(lactor->anim, actorNum);

						if (lactor->previousAnimIndex == -1) {
							lactor->previousAnimIndex = getAnimIndexForBody(0, var_C);
							lactor->anim = 0;
						}

						lactor->animExtraData = loadTwinsenCostumesVar1;

						lactor->field_78 = 0;
						lactor->animPosition = 0;
						lactor->field_66 = 0;
					}

					if (lactor->animExtraData) {
						GereAnimAction(lactor, actorNum);
					}

					lactor->dynamicFlagsBF.bUnk0004 = 1;
				}

				lactor->lastRotationSpeed = 0;

				lactor->lastX = 0;
				lactor->lastZ = 0;
				lactor->lastY = 0;
			}
		}
	}

	if (lactor->standOn != -1) { // standing on object
		processActorX -= actors[lactor->standOn].field_20;  // apply under object movement
		processActorZ -= actors[lactor->standOn].field_22;
		processActorY -= actors[lactor->standOn].field_24;

		processActorX += actors[lactor->standOn].X;
		processActorZ += actors[lactor->standOn].Y;
		processActorY += actors[lactor->standOn].Z;

		if (!CheckZvOnZv(actorNum, lactor->standOn))  // is actor still standing on another actor ?
			lactor->standOn = -1; // actor fall from the object
	}

	if (lactor->dynamicFlagsBF.bUnk0100) { // if falling, then no modification...
		processActorX = processActorVar2;
		processActorZ = processActorVar3 + mainLoopVar17; // apply fall speed
		processActorY = processActorVar4;
	}

	if (lactor->staticFlagsBF.bComputeCollisionWithBricks) { // if wall collision is enabled
		int position;

		getPosVar2 = 0;
		position = WorldColBrick(processActorVar2, processActorVar3, processActorVar4);

		if (position) { // process slope
			if (position == 1) { // if this occure, that means the actor is already in the wall... Not good!
				printf("Warping !!!\n");
				lactor->Y = processActorZ = processActorZ / 256 * 256 + 256;  // warp upper...
			} else {
				ReajustPos(position); // handle slope
			}
		}

		if (lactor->staticFlagsBF.bComputeCollisionWithObj) // if we check collision with other objects
			CheckObjCol(actorNum);  //check collision and see if actor fall on an object

		if ((lactor->standOn != -1) && (lactor->dynamicFlagsBF.bUnk0100)) // if actor felt on another an object
			ReceptionObj(); // stop falling

		fieldCauseDamage = 0;

		processActorVar11 = processActorX;
		processActorVar12 = processActorZ;
		processActorVar13 = processActorY;

		if (!actorNum && !(lactor->staticFlagsBF.bIsDead)) { // check for wall collision
			DoCornerReadjustTwinkel(lactor->boudingBox.X.bottomLeft, lactor->boudingBox.Y.bottomLeft, lactor->boudingBox.Z.bottomLeft, 1);  // twinsen wall collision code
			DoCornerReadjustTwinkel(lactor->boudingBox.X.topRight, lactor->boudingBox.Y.bottomLeft, lactor->boudingBox.Z.bottomLeft, 2);
			DoCornerReadjustTwinkel(lactor->boudingBox.X.topRight, lactor->boudingBox.Y.bottomLeft, lactor->boudingBox.Z.topRight, 4);
			DoCornerReadjustTwinkel(lactor->boudingBox.X.bottomLeft, lactor->boudingBox.Y.bottomLeft, lactor->boudingBox.Z.topRight, 8);
		} else {
			DoCornerReajust(lactor->boudingBox.X.bottomLeft, lactor->boudingBox.Y.bottomLeft, lactor->boudingBox.Z.bottomLeft, 1);  // other objects wall collision code
			DoCornerReajust(lactor->boudingBox.X.topRight, lactor->boudingBox.Y.bottomLeft, lactor->boudingBox.Z.bottomLeft, 2);
			DoCornerReajust(lactor->boudingBox.X.topRight, lactor->boudingBox.Y.bottomLeft, lactor->boudingBox.Z.topRight, 4);
			DoCornerReajust(lactor->boudingBox.X.bottomLeft, lactor->boudingBox.Y.bottomLeft, lactor->boudingBox.Z.topRight, 8);
		}

		if (fieldCauseDamage && !(lactor->dynamicFlagsBF.bUnk0100) && !currentlyProcessedActorNum && (comportementHero == 1) && (lactor->anim == 1)) { // wall hit while running
			Rotate(lactor->boudingBox.X.bottomLeft, lactor->boudingBox.Z.bottomLeft, lactor->angle + 0x580);

			destX += processActorX;
			destZ += processActorY;

			if (destX >= 0 && destZ >= 0 && destX <= 0x7E00 && destZ <= 0x7E00) {
				if (WorldColBrick(destX, processActorZ + 0x100, destY)) {
					InitSpecial(lactor->X, lactor->Y + 1000, lactor->Z, 0);
					InitAnim(ANIM_hitBig, 2, 0, currentlyProcessedActorNum);

					printf("Wall hit 2\n");

					if (currentlyProcessedActorNum == 0) {
						twinsenMoved = 1; // twinsenHit
					}

					lactor->life--; // damage caused by field
				}
			}
		}

		position = WorldColBrick(processActorX, processActorZ, processActorY);
		var_4 = position;
		lactor->field_3 = position;

		if (position) { // if standing on floor
			if (position == 1) { // if next step is in wall...
				if (lactor->dynamicFlagsBF.bUnk0100) { // if was falling
					ReceptionObj();
					processActorZ = (getPosVar2 << 8) + 0x100;
				} else {
					if (!actorNum && comportementHero == 1 && lactor->anim == var_4) {
						InitSpecial(lactor->X, lactor->Y + 1000, lactor->Z, 0);
						InitAnim(ANIM_hitBig, 2, 0, currentlyProcessedActorNum);

						printf("Wall hit !\n");

						if (!actorNum) {
							twinsenMoved = 1;
						}

						lactor->life--;
					}

					if (!WorldColBrick(processActorX, processActorZ, processActorVar4)) { // no Y problem
						processActorY = processActorVar4;
					}

					if (!WorldColBrick(processActorVar2, processActorZ, processActorY)) { // no X problem
						processActorX = processActorVar2; // no X Collision
					}

					if (WorldColBrick(processActorX, processActorZ, processActorVar4) && WorldColBrick(processActorVar2, processActorZ, processActorY)) {
						return; // both X/Y problem -> can't move !
					}
				}
			} else { // not standing on flat floor
				if (lactor->dynamicFlagsBF.bUnk0100)
					ReceptionObj();

				ReajustPos(var_4);
			}

			lactor->dynamicFlagsBF.bUnk0100 = 0;
		} else { // not standing on floor
			if (lactor->staticFlagsBF.bIsFallable && lactor->standOn == -1) { // if fallable and actor not standing on another actor
				var_8 = WorldColBrick(processActorX, processActorZ - 1, processActorY); // what is 1 step under ?

				if (var_8) { // under is the floor
					if (lactor->dynamicFlagsBF.bUnk0100) { // if was falling...
						ReceptionObj();
					}

					ReajustPos(var_8);
				} else { // start falling
					if (!(lactor->dynamicFlagsBF.bUnk0080 == 1)) {
						lactor->dynamicFlagsBF.bUnk0100 = 1;

						if ((!actorNum) && twinsenZBeforeFall == 0) {
							twinsenZBeforeFall = processActorZ;
						}

						InitAnim(ANIM_fall, 0, 255, actorNum);

					}
				}
			}
		}

		if (getPosVar2 == -1) { // under the map...
			lactor->life = 0; // die...
		}
	} else {   // no wall collision
		if (lactor->staticFlagsBF.bComputeCollisionWithObj) //if actor collision
			CheckObjCol(actorNum);
	}

	if (fieldCauseDamage)
		lactor->field_3 |= 0x80;

	if (processActorX < 0)
		processActorX = 0;

	if (processActorY < 0)
		processActorY = 0;

	if (processActorZ < 0)
		processActorZ = 0;

	if (processActorX > 0x7E00)
		processActorX = 0x7E00;

	if (processActorY > 0x7E00)
		processActorY = 0x7E00;

	lactor->X = processActorX;
	lactor->Y = processActorZ;
	lactor->Z = processActorY;
}

void DoCornerReadjustTwinkel(int X, int Y, int Z, int mask) { // twinsen wall colision
	int pos;

	pos = WorldColBrick(processActorX, processActorZ, processActorY);

	processActorX += X;
	processActorZ += Y;
	processActorY += Z;

	if (processActorX >= 0 && processActorY >= 0 && processActorX <= 0x7E00 && processActorY <= 0x7E00) {
		ReajustPos(pos); //handle slope
		pos = WorldColBrickFull(processActorX, processActorZ, processActorY, processActorVar1->boudingBox.Y.topRight);

		if (pos == 1) { // next position is a wall
			fieldCauseDamage |= mask;
			pos = WorldColBrickFull(processActorX, processActorZ, processActorVar4 + Z, processActorVar1->boudingBox.Y.topRight);
			if (pos == 1) {
				pos = WorldColBrickFull(X + processActorVar2, processActorZ, processActorY, processActorVar1->boudingBox.Y.topRight);

				if (pos != 1) {
					processActorVar11 = processActorVar2;
				}
			} else {
				processActorVar13 = processActorVar4;
			}
		}
	}

	processActorX = processActorVar11;
	processActorZ = processActorVar12;
	processActorY = processActorVar13;
}

void DoCornerReajust(int var0, int var1, int var2, int var3) {
	int pos;

	pos = WorldColBrick(processActorX, processActorZ, processActorY);

	processActorX += var0;
	processActorZ += var1;
	processActorY += var2;

	if (processActorX >= 0 && processActorY >= 0 && processActorX <= 0x7E00 && processActorY <= 0x7E00) {
		ReajustPos(pos);
		pos = WorldColBrick(processActorX, processActorZ, processActorY);

		if (pos == 1) { // next position is a wall
			fieldCauseDamage |= var3;
			pos = WorldColBrick(processActorX, processActorZ, processActorVar4 + var2);
			if (pos == 1) {
				pos = WorldColBrick(var0 + processActorVar2, processActorZ, processActorY);

				if (pos != 1) {
					processActorVar11 = processActorVar2;
				}
			} else {
				processActorVar13 = processActorVar4;
			}
		}
	}

	processActorX = processActorVar11;
	processActorZ = processActorVar12;
	processActorY = processActorVar13;
}

void ReajustPos(int param) {
	int localGetPosVar1;
	int localGetPosVar2;
	int localGetPosVar3;

	if (!param)
		return;

	localGetPosVar1 = (getPosVar1 << 9) - 0x100;
	localGetPosVar2 = (getPosVar2 << 8);
	localGetPosVar3 = (getPosVar3 << 9) - 0x100;

	//****************** special collisions ******************//
	if (param >= 6 && param <= 13) {
		switch (param) {
		case 6: {
				if ((processActorY - getPosVar3) <= (processActorX - getPosVar1)) {
					param = 2;
				} else {
					param = 3;
				}
				break;
			}
		case 7: {
				if ((processActorY - getPosVar3) <= (processActorX - getPosVar1)) {
					param = 4;
				} else {
					param = 5;
				}
				break;
			}
		case 8: {
				if ((512 - (processActorX - getPosVar1)) <= (processActorY - getPosVar3)) {
					param = 2;
				} else {
					param = 4;
				}
				break;
			}
		case 9: {
				if ((512 - (processActorX - getPosVar1)) <= (processActorY - getPosVar3)) {
					param = 3;
				} else {
					param = 5;
				}
				break;
			}
		case 10: {
				if ((processActorX - getPosVar1) >= (processActorY - getPosVar3)) {
					param = 3;
				} else {
					param = 2;
				}
				break;
			}
		case 11: {
				if ((processActorY - getPosVar3) <= (processActorX - getPosVar1)) {
					param = 5;
				} else {
					param = 4;
				}
				break;
			}
		case 12: {
				if ((512 - processActorX - getPosVar1) <= (processActorY - getPosVar3)) {
					param = 4;
				} else {
					param = 2;
				}
				break;
			}
		case 13: {
				if ((512 - processActorX - getPosVar1) <= (processActorY - getPosVar3)) {
					param = 5;
				} else {
					param = 3;
				}
				break;
			}
		default: {
				printf("collision %d\n", param);
				//      exit(1);
			}
		}
	}

	if (param >= 2 && param <= 5) {
		switch (param) {
			//****************** slope collisions *****************//
		case 2: {
				processActorZ =
					localGetPosVar2 + BoundRegleTrois(0, 0x100, 0x200,
													  processActorX - localGetPosVar1);
				break;
			}
		case 3: {
				processActorZ =
					localGetPosVar2 + BoundRegleTrois(0, 0x100, 0x200,
													  processActorY - localGetPosVar3);
				break;
			}
		case 4: {
				processActorZ =
					localGetPosVar2 + BoundRegleTrois(0x100, 0, 0x200,
													  processActorY - localGetPosVar3);
				break;
			}
		case 5: {
				processActorZ =
					localGetPosVar2 + BoundRegleTrois(0x100, 0, 0x200,
													  processActorX - localGetPosVar1);
				return;
			}
		}
	}

}

// fait la moyenne pour la hauteur.
// min max ? var
int BoundRegleTrois(int var0, int var1, int var2, int var3) {
	if (var3 <= 0)
		return (var0);

	if (var3 >= var2)
		return (var1);

	return ((((var1 - var0) * var3) / var2) + var0);
}

int CheckObjCol(int actorNum) {
	int X1;
	int X2;
	int Z1;
	int Z2;
	int Y1;
	int Y2;
	int currentlyTestedActor = 0;
	int var_60;

	actor* lactor;
	lactor = &actors[actorNum];

	X1 = processActorX + lactor->boudingBox.X.bottomLeft;
	X2 = processActorX + lactor->boudingBox.X.topRight;

	Z1 = processActorZ + lactor->boudingBox.Y.bottomLeft;
	Z2 = processActorZ + lactor->boudingBox.Y.topRight;

	Y1 = processActorY + lactor->boudingBox.Z.bottomLeft;
	Y2 = processActorY + lactor->boudingBox.Z.topRight;

	lactor->collision = -1;

	while (currentlyTestedActor < numActorInRoom) {
		if (currentlyTestedActor != actorNum && actors[currentlyTestedActor].costumeIndex != -1 && !(lactor->staticFlagsBF.bIsDead) && actors[currentlyTestedActor].standOn != actorNum) { // is actor valid (not self and defined)
			actor* lactor2;

			int X1_2;
			int X2_2;
			int Z1_2;
			int Z2_2;
			int Y1_2;
			int Y2_2;

			lactor2 = &actors[currentlyTestedActor];

			X1_2 = lactor2->X + lactor2->boudingBox.X.bottomLeft;
			X2_2 = lactor2->X + lactor2->boudingBox.X.topRight;

			Z1_2 = lactor2->Y + lactor2->boudingBox.Y.bottomLeft;
			Z2_2 = lactor2->Y + lactor2->boudingBox.Y.topRight;

			Y1_2 = lactor2->Z + lactor2->boudingBox.Z.bottomLeft;
			Y2_2 = lactor2->Z + lactor2->boudingBox.Z.topRight;

			if (X1<X2_2 && X2>X1_2 && Z1<Z2_2 && Z2>Z1_2 && Y1<Y2_2 && Y2>Y1_2) {

				lactor->collision = currentlyTestedActor;

				if (lactor2->staticFlagsBF.bIsCarrier) { // if carrier
					if (lactor->dynamicFlagsBF.bUnk0100) { // if can stand on object
						processActorZ = Z2_2 - lactor->boudingBox.Y.bottomLeft + 1; // new Z

						lactor->standOn = currentlyTestedActor;

						printf("Actor %d fall on %d\n", actorNum, currentlyTestedActor);
					} else {
						if (CheckZvOnZv(actorNum, currentlyTestedActor)) {
							processActorZ = Z2_2 - lactor->boudingBox.Y.bottomLeft + 1; // new Z

							lactor->standOn = currentlyTestedActor;
						} else {
							goto lab12AC5;
						}
					}
				} else {
					int newAngle;
					var_60 = actorNum;
					if (CheckZvOnZv(actorNum, currentlyTestedActor)) {
						HitObj(var_60, currentlyTestedActor, 1, -1);
					}

lab12AC5:
					newAngle = GetAngle(processActorX, processActorY, lactor2->X, lactor2->Z);

					if (lactor2->staticFlagsBF.bIsPushable && !(lactor->staticFlagsBF.bIsPushable)) { // should be pushed ?
						lactor2->lastZ = 0;

						if (lactor2->staticFlagsBF.bIsUsingMiniZv) {
							if (newAngle >= 0x80 && newAngle < 0x180 && lactor->angle > 0x80 && lactor->angle < 0x180)
								lactor2->lastX = 192;

							if (newAngle >= 0x180 && newAngle < 0x280 && lactor->angle > 0x180 && lactor->angle < 0x280)
								lactor2->lastY = -64;

							if (newAngle >= 0x280 && newAngle < 0x380 && lactor->angle > 0x280 && lactor->angle < 0x380)
								lactor2->lastX = -64;

							if ((newAngle >= 0x380 || newAngle < 0x80) && (lactor->angle >= 0x380 || lactor->angle < 0x80))
								lactor2->lastY = 192;

						} else {
							lactor2->lastX = processActorX - lactor->field_20;
							lactor2->lastY = processActorY - lactor->field_24;
						}
					}

					if (((lactor2->boudingBox.X.topRight - lactor2->boudingBox.X.bottomLeft) == (lactor2->boudingBox.Z.topRight - lactor2->boudingBox.Z.bottomLeft)) && ((lactor->boudingBox.X.topRight - lactor->boudingBox.X.bottomLeft) == (lactor->boudingBox.Z.topRight - lactor->boudingBox.Z.bottomLeft))) {
						if (newAngle < 0x180)
							processActorX = X1_2 - lactor->boudingBox.X.topRight;

						if (newAngle >= 0x180 && newAngle < 0x280)
							processActorY = Y2_2 - lactor->boudingBox.Z.bottomLeft;

						if (newAngle >= 0x280 && newAngle < 0x380)
							processActorX = X2_2 - lactor->boudingBox.X.bottomLeft;

						if (newAngle >= 0x380 || (newAngle < 0x380 && newAngle < 0x80))
							processActorY = Y1_2 - lactor->boudingBox.Z.topRight;
					} else {
						if (!(lactor->dynamicFlagsBF.bUnk0100)) {
							processActorX = processActorVar2;
							processActorZ = processActorVar3;
							processActorY = processActorVar4;
						}
					}
				}
			}
		}

		currentlyTestedActor++;
	}

	if (lactor->dynamicFlagsBF.bUnk0002) { // lactor is attacking
		int i;

		Rotate(0, 200, lactor->angle);

		X1 = destX + processActorX + lactor->boudingBox.X.bottomLeft;
		X2 = destX + processActorX + lactor->boudingBox.X.topRight;

		Z1 = processActorZ + lactor->boudingBox.Y.bottomLeft;
		Z2 = processActorZ + lactor->boudingBox.Y.topRight;

		Y1 = destZ + processActorY + lactor->boudingBox.Z.bottomLeft;
		Y2 = destZ + processActorY + lactor->boudingBox.Z.topRight;

		for (i = 0;i < numActorInRoom;i++) {
			if (i != actorNum && actors[i].costumeIndex != -1 && !(actors[i].staticFlagsBF.bNoDisplay) && actors[i].standOn != actorNum) { // is actor valid (not self and defined)
				actor* lactor2;

				int X1_2;
				int X2_2;
				int Z1_2;
				int Z2_2;
				int Y1_2;
				int Y2_2;

				lactor2 = &actors[i];

				X1_2 = lactor2->X + lactor2->boudingBox.X.bottomLeft;
				X2_2 = lactor2->X + lactor2->boudingBox.X.topRight;

				Z1_2 = lactor2->Y + lactor2->boudingBox.Y.bottomLeft;
				Z2_2 = lactor2->Y + lactor2->boudingBox.Y.topRight;

				Y1_2 = lactor2->Z + lactor2->boudingBox.Z.bottomLeft;
				Y2_2 = lactor2->Z + lactor2->boudingBox.Z.topRight;

				if (X1<X2_2 && X2>X1_2 && Z1<Z2_2 && Z2>Z1_2 && Y1<Y2_2 && Y2>Y1_2) {
					HitObj(actorNum, i, lactor->field_66, lactor->angle + 0x200);
					lactor->dynamicFlagsBF.bUnk0002 = 0;
				}
			}
		}
	}

	return(lactor->collision);

}

void ReceptionObj(void) { // stop falling
	int fall;

	if (currentlyProcessedActorNum == 0) {
		fall = twinsenZBeforeFall - processActorZ;

		if (fall >= 0x1000) {
			InitSpecial(processActorVar1->X, processActorVar1->Y + 1000, processActorVar1->Z, 0);
			processActorVar1->life = 0;
			InitAnim(ANIM_landHit, 2, 0, currentlyProcessedActorNum);
		} else if (fall >= 0x800) {
			InitSpecial(processActorVar1->X, processActorVar1->Y + 1000, processActorVar1->Z, 0);
			processActorVar1->life--;
			InitAnim(ANIM_landHit, 2, 0, currentlyProcessedActorNum);
		} else if (fall > 10) {
			InitAnim(ANIM_land, 2, 0, currentlyProcessedActorNum);
		} else {
			InitAnim(ANIM_static, 0, 0, currentlyProcessedActorNum);
		}

		twinsenZBeforeFall = 0;
	} else {
		InitAnim(ANIM_land, 2, processActorVar1->field_2, currentlyProcessedActorNum);
	}

	processActorVar1->dynamicFlagsBF.bUnk0100 = 0;
}

int WorldColBrickFull(int var0, int var1, int var2, int var3) {
	unsigned char *ptr;
	char *ptr2;
	unsigned char temp0;
	unsigned char temp3;

	ptr = (unsigned char *) bufCube;

	getPosVar1 = (var0 + 0x100) >> 9;
	getPosVar2 = var1 >> 8;
	getPosVar3 = (var2 + 0x100) >> 9;

	if (getPosVar1 < 0 || getPosVar1 >= 64)
		return (0);

	ptr += getPosVar1 * 50;

	if (getPosVar2 <= -1)
		return (1);

	if (getPosVar2 < 0 || getPosVar2 >= 24)
		return (0);

	ptr += getPosVar2 * 2;

	if (getPosVar3 < 0 || getPosVar3 >= 64)
		return (0);

	ptr += (getPosVar3 << 7) * 25;

	temp0 = *ptr;

	if (temp0) {
		int i;
		int j;
		char al;
		int edx;

		ptr2 = (char *) currentBll;

		ptr2 += READ_LE_U32(ptr2 + temp0 * 4 - 4);
		ptr2 += 3;

		temp3 = *(ptr + 1);

		ptr2 = ptr2 + temp3 * 4;

		al = *ptr2;

		edx = ((var3 + 255) >> 8);

		j = getPosVar2;

		for (i = 0;i < edx;i++) { // search from bottom to top
			if (j > 24) { // check if we are not out of the cube
				return(al);
			}

			ptr += 2;
			j++;

			if (READ_LE_S16(ptr) != 0) {
				return(1);
			}
		}

		return(al);
	} else {
		int i;
		int j;
		char al;
		int edx;

		al = *(ptr + 1);
		edx = ((var3 + 255) >> 8);

		j = getPosVar2;

		for (i = 0;i < edx;i++) {
			if (j > 24) {
				return(al);
			}

			ptr += 2;
			j++;

			if (READ_LE_S16(ptr) != 0) {
				return(1);
			}
		}
	}

	return(0);
}

int WorldColBrick(int var0, int var1, int var2) {
	char *ptr;
	char *ptr2;
	unsigned char temp0;
	unsigned char temp3;

	ptr = (char *) bufCube;

	getPosVar1 = (var0 + 0x100) >> 9;
	getPosVar2 = var1 >> 8;
	getPosVar3 = (var2 + 0x100) >> 9;

	if (getPosVar1 < 0 || getPosVar1 >= 64)
		return (0);

	ptr += getPosVar1 * 50;

	if (getPosVar2 <= -1)
		return (1);

	if (getPosVar2 < 0 || getPosVar2 >= 24)
		return (0);

	ptr += getPosVar2 * 2;

	if (getPosVar3 < 0 || getPosVar3 >= 64)
		return (0);

	ptr += (getPosVar3 << 7) * 25;

	temp0 = *(unsigned char *) ptr;

	if (temp0) {
		ptr2 = (char *) currentBll;

		ptr2 += READ_LE_U32(ptr2 + temp0 * 4 - 4);
		ptr2 += 3;

		temp3 = *(ptr + 1);

		ptr2 = ptr2 + temp3 * 4;

		return (*ptr2);
	} else {
		return (*(ptr + 1));
	}
}

void Rotate(int initialX, int initialY, int angle) {
	int angle1;
	int angle2;

	if (!angle) {
		destX = initialX;
		destZ = initialY;
	} else {
		angle1 = tab1[angle & 0x3FF];
		angle2 = tab1[(angle + 0x100) & 0x3FF];

		destX = (initialX * angle2 + initialY * angle1) >> 14;
		destZ = (initialY * angle2 - initialX * angle1) >> 14;
	}
}

int CheckZvOnZv(int var0, int var1) { // is actor still standing on object ?
	actor *lactor1;
	actor *lactor2;

	int bx;
	int si;
	int di;
	int var_1C;
	int var_4;
	int var_20;
	int var_18;
	int cx;
	int var_8;
	int var_10;
	int var_C;
	int var_14;

	lactor1 = &actors[var0];
	lactor2 = &actors[var1];

	// ------------- actor1

	bx = processActorX + lactor1->boudingBox.X.bottomLeft;
	si = processActorX + lactor1->boudingBox.X.topRight;

	di = processActorZ + lactor1->boudingBox.Y.bottomLeft;
	var_1C = processActorZ + lactor1->boudingBox.Y.topRight;

	var_4 = processActorY + lactor1->boudingBox.Z.bottomLeft;
	var_20 = processActorY + lactor1->boudingBox.Z.topRight;

	// ------------- actor2

	var_18 = lactor2->X + lactor2->boudingBox.X.bottomLeft;
	cx = lactor2->X + lactor2->boudingBox.X.topRight;

	var_8 = lactor2->Y + lactor2->boudingBox.Y.bottomLeft;
	var_10 = lactor2->Y + lactor2->boudingBox.Y.topRight;

	var_C = lactor2->Z + lactor2->boudingBox.Z.bottomLeft;
	var_14 = lactor2->Z + lactor2->boudingBox.Z.topRight;

	if (bx >= cx)   // X1
		return (0);

	if (si <= var_18)   // X2
		return (0);

	if (di > (var_10 + 1))  // Z1a
		return (0);

	if (di <= (var_10 - 0x100)) // Z1b
		return (0);

	if (var_1C <= var_8)  // Z2
		return (0);

	if (var_4 >= var_14)  // Y1
		return (0);

	if (var_20 <= var_C)  // Y2
		return (0);

	return (1);
}

void HitObj(int actorAttacking, int actorAttacked, int param, int angle) {
//    actor* pActorAttacking = &actors[actorAttacking];
	actor* pActorAttacked = &actors[actorAttacked];

	if (pActorAttacked->life <= 0)
		return;

	pActorAttacked->hitBy = actorAttacking;

	if (pActorAttacked->field_14 <= param) { //armure ?
		if (pActorAttacked->anim == ANIM_hitBig || pActorAttacked->anim == ANIM_hit2) {
			int temp;
			temp = pActorAttacked->animPosition;
			pActorAttacked->animPosition = 1;

			if (pActorAttacked->animExtraData) {
				GereAnimAction(pActorAttacked, actorAttacked);
			}

			pActorAttacked->animPosition = temp;
		} else {
			if (angle != -1) {
				setActorAngleSafe(angle, angle, 0, &pActorAttacked->time); // force angle without transition
			}

			if (rand() & 1) {
				InitAnim(ANIM_hit2, 3, 255, actorAttacked);
			} else {
				InitAnim(ANIM_hitBig, 3, 255, actorAttacked);
			}
		}

		InitSpecial(pActorAttacked->X, pActorAttacked->Y + 1000, pActorAttacked->Z, 0);

		if (!actorAttacked) { // if twisen is attacked
			twinsenMoved = 1;
		}

		pActorAttacked->life -= param;

		if (pActorAttacked->life < 0)
			pActorAttacked->life = 0;
	} else {
		InitAnim(ANIM_hit, 3, 255, actorAttacked);
	}
}
