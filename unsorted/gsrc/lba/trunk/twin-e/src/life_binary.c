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
#include "life_common.h"

#ifndef TXT_SCRIPTS

void runActorScript(short int actorNumber) {
	char string[256];
	short int OPbreak;
	actor *lactor;
	unsigned char opcode;
	unsigned char *localScriptPtr;
	unsigned char *opcodePtr;
	int temp;

#ifdef MACOSX
	return;
#endif

	lactor = &actors[actorNumber];

	OPbreak = 0;

	actorScriptPtr = lactor->positionInActorScript + lactor->actorScript;

	while (OPbreak != -1) {
		opcodePtr = actorScriptPtr++;
		opcode = *(opcodePtr);
		//printf("(%d)opcode: %d\n",actorNumber,opcode);
		if (opcode <= 105) {
			localScriptPtr = actorScriptPtr + 1;
			switch (opcode) {
			case 0: {
					OPbreak = -1;
					lactor->positionInActorScript = -1;
					break;
				}

			case 2: {
					manipActor(lactor);
					if (!doCalc()) {
						*opcodePtr = 13;
					}
					actorScriptPtr = lactor->actorScript + READ_LE_S16(actorScriptPtr);
					break;
				}

			case 3: {
					actorScriptPtr = lactor->actorScript + READ_LE_S16(actorScriptPtr);
					break;
				}

			case 4: {
					manipActor(lactor);
					doCalc();
					actorScriptPtr = lactor->actorScript + READ_LE_S16(actorScriptPtr);
					break;
				}
			case 10: {
					actorScriptPtr++;
					break;
				}

			case 11: {
					OPbreak = -1;
					break;
				}

			case 12: {
					manipActor(lactor);
					if (!doCalc()) {
						actorScriptPtr =
							lactor->actorScript + READ_LE_S16(actorScriptPtr);
					} else {
						actorScriptPtr += 2;
					}
					break;
				}

			case 13: {
					manipActor(lactor);
					if (!doCalc()) {
						actorScriptPtr =
							lactor->actorScript + READ_LE_S16(actorScriptPtr);
					} else {
						actorScriptPtr += 2;
						*opcodePtr = 2;
					}
					break;
				}

			case 14: {
					manipActor(lactor);
					if (!doCalc()) {
						actorScriptPtr =
							lactor->actorScript + READ_LE_S16(actorScriptPtr);

					} else {
						actorScriptPtr += 2;
						*opcodePtr = 4;  // le met en always branch

					}
					break;
				}

			case 15: {
					actorScriptPtr = lactor->actorScript + READ_LE_S16(actorScriptPtr);
					break;
				}

			case 17: { // loadCostume
					InitBody(*(actorScriptPtr++), actorNumber);
					break;
				}

			case 18: { // loadCostumeAnother
					int param1;
					int param2;

					param1 = *(actorScriptPtr++);
					param2 = *(actorScriptPtr++);

					InitBody(param2, param1);
					break;
				}

			case 19: { // InitAnim
					InitAnim(*(actorScriptPtr++), 0, 0, actorNumber);
					break;
				}

			case 20: { // playAnimAnother
					InitAnim(*(actorScriptPtr + 1), 0, 0, *actorScriptPtr);
					actorScriptPtr += 2;
					break;
				}
			case 21: {
					lactor->positionInActorScript = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;
					break;
				}
			case 22: {
					actor *tempActor;

					tempActor = &actors[*(actorScriptPtr++)];

					tempActor->positionInActorScript = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;
					break;
				}

			case 23: { // SET_TRACK
					lactor->positionInMoveScript = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;
					break;
				}

			case 24: { // setTrackAnother
					int actorNumTemp;

					actorNumTemp = *(actorScriptPtr++);
					actors[actorNumTemp].positionInMoveScript = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;
					break;
				}

			case 25: {
					freezeTime();
					TestRestoreModeSVGA(1);
					//                             if(showTalkIcon)
					//                             DrawBulle(actorNumber);
					TestCoulDial(lactor->talkColor); // CoulFont
					talkingActor = actorNumber;
					printTextFullScreen(READ_LE_S16(actorScriptPtr));
					actorScriptPtr += 2;
					unfreezeTime();
					fullRedraw(1);
					//                             waitForKey();
					break;
				}

			case 26: { // LM_FALLABLE
					int ltemp = *(actorScriptPtr++);

					lactor->staticFlagsBF.bIsFallable = ltemp & 1;
					break;
				}

			case 27: { // SET_COMPORTEMENT
					temp = *(actorScriptPtr++);
					lactor->comportement = temp;
					if (temp == 2) {
						lactor->followedActor = *(actorScriptPtr++);
					}
					break;
				}

			case 28: {
					actor *tempActor;

					tempActor = &actors[*(actorScriptPtr++)];
					temp = *(actorScriptPtr++);
					tempActor->comportement = temp;

					if (temp == 2) {
						tempActor->followedActor = *(actorScriptPtr++);
					}

					break;
				}

			case 29: { // setCameraActor
					int newActorToFollow;

					newActorToFollow = *(actorScriptPtr++);

					if (currentlyFollowedActor != newActorToFollow) {
						newCameraX = (actors[newActorToFollow].X >> 9);
						newCameraZ = (actors[newActorToFollow].Y >> 8);
						newCameraY = (actors[newActorToFollow].Z >> 9);

						currentlyFollowedActor = newActorToFollow;
						requestBackgroundRedraw = 1;
					}

					break;
				}

			case 30: {
					InitAnim(ANIM_static, 0, 255, 0);
					SetComportement(*(actorScriptPtr++));
					break;
				}

			case 31: {
					unsigned char temp1;
					unsigned char temp2;

					temp1 = *(actorScriptPtr++);
					temp2 = *(actorScriptPtr++);

					cubeFlags[temp1] = temp2;
					break;
				}

			case 32: {
					actorScriptPtr++;
					break;
				}

			case 33: {
					lactor->positionInActorScript = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;
					break;
				}

			case 34: {
					int tempActorNum;

					tempActorNum = *(actorScriptPtr++);
					actors[tempActorNum].positionInActorScript = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;
					break;
				}

			case 35: {
					OPbreak = -1;
					break;
				}

			case 36: { // SET_VAR_GAME
					unsigned char temp1;
					char temp2;

					temp1 = *(actorScriptPtr++);
					temp2 = *(actorScriptPtr++);

					vars[temp1] = temp2;
					break;
				}
			case 37: {
					int temp;

					temp = *(actorScriptPtr++);

					CheckCarrier(temp);

					actors[temp].dynamicFlagsBF.bUnk0020 = 1;
					actors[temp].costumeIndex = -1;
					actors[temp].zone = -1;
					actors[temp].life = 0;

					break;
				}

			case 38: {
					CheckCarrier(actorNumber);
					lactor->dynamicFlagsBF.bUnk0020 = 1;
					lactor->costumeIndex = -1;
					lactor->zone = -1;
					lactor->life = 0;
					break;
				}

			case 39: {
					numKey--;

					if (numKey < 0)
						numKey = 0;

					addOverlayObject(0, 6, 0, 0, 0, 1, 1);
					break;
				}
			case 40: {
					int eax;
					int ecx;
					overlayObjectListStruct *edi;
					int oldNumCoin;

					short int cost;

					cost = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;

					oldNumCoin = numCoin;
					numCoin -= cost;
					if (numCoin < 0)
						numCoin = 0;

					addOverlayObject(0, 3, 10, 30, 0, 0, 3); // display the coin

					eax = 0;
					ecx = 0;
					edi = overlayObjectList;

					while (eax < 10) {
						if (edi->field_0 != -1 && edi->type == 2) {
							edi->field_0 = BoundRegleTrois(edi->followedActor, edi->field_0, 100, edi->timeToDie - lba_time - 50);
							edi->followedActor = numCoin;
							edi->timeToDie = lba_time + 150;
							ecx = 1;
							break;
						}
						edi++;
						eax++;
					}

					if (!ecx) {
						addOverlayObject(2, oldNumCoin, 60, 40, numCoin, ecx, 3);
					}

					break;

				}
			case 41: {
					OPbreak = -1;
					lactor->positionInActorScript = -1;
					break;
				}
			case 42: { // stopTrack
					lactor->pausedTrackPtr = lactor->currentLabelPtr;
					lactor->positionInMoveScript = -1;
					break;
				}
			case 43: { // resumeTrack
					lactor->positionInMoveScript = lactor->pausedTrackPtr;
					break;
				}
			case 44: {
					int temp;
					int textNum;

					freezeTime();
					TestRestoreModeSVGA(1);

					temp = *(actorScriptPtr++);

					/*
					   if(showTalkVar)
					   {
					   DrawBulle(temp);
					   }
					 */

					TestCoulDial(actors[temp].talkColor);
					talkingActor = temp;

					textNum = READ_LE_S16(actorScriptPtr);

					printTextFullScreen(textNum);

					actorScriptPtr += 2;

					unfreezeTime();

					fullRedraw(1);

					//                             waitForKey();

					break;
				}

			case 45: {
					chapter++;
					break;
				}
			case 46: {
					int temp;

					freezeTime();
					TestRestoreModeSVGA(1);

					temp = *(actorScriptPtr++);
					foundObject(temp);

					unfreezeTime();
					fullRedraw(1);
					break;
				}
			case 47: {
					lactor->angle = 0x300;
					lactor->X = lactor->lastX - READ_LE_S16(actorScriptPtr);
					lactor->dynamicFlagsBF.bIsMoving = 0;
					lactor->speed = 0;
					actorScriptPtr += 2;
					break;
				}
			case 48: {
					lactor->angle = 0x100;
					lactor->X = lactor->lastX + READ_LE_S16(actorScriptPtr);
					lactor->dynamicFlagsBF.bIsMoving = 0;
					lactor->speed = 0;
					actorScriptPtr += 2;
					break;
				}
			case 49: {
					lactor->angle = 0x200;
					lactor->Z = lactor->lastY - READ_LE_S16(actorScriptPtr);
					lactor->dynamicFlagsBF.bIsMoving = 0;
					lactor->speed = 0;
					actorScriptPtr += 2;
					break;
				}
			case 50: {
					lactor->angle = 0;
					lactor->Z = lactor->lastY + READ_LE_S16(actorScriptPtr);
					lactor->dynamicFlagsBF.bIsMoving = 0;
					lactor->speed = 0;
					actorScriptPtr += 2;
					break;
				}
			case 51: {
					char temp;

					if (lactor->field_10 & 0x1F0) {
						GiveExtraBonus(lactor);
					}

					temp = *(actorScriptPtr++);

					if (temp != 0) {
						lactor->field_10 |= 1;
					}

					break;
				}
			case 52: {
					twinsenPositionModeInNewCube = 2;
					needChangeRoom = *(actorScriptPtr++);
					break;
				}
			case 53: { // OBJ_COL
					if (*(actorScriptPtr++) != 0) {
						lactor->staticFlagsBF.bComputeCollisionWithObj = 1;
					} else {
						lactor->staticFlagsBF.bComputeCollisionWithObj = 0;
					}
					break;
				}
			case 54: { // BRICK_COL
					char temp;

					temp = *(actorScriptPtr++);

					lactor->staticFlagsBF.bComputeCollisionWithBricks = false;
					lactor->staticFlagsBF.bIsDead = false;

					if (temp == 1) {
						lactor->staticFlagsBF.bComputeCollisionWithBricks = true;
					} else if (temp == 2) {
						lactor->staticFlagsBF.bComputeCollisionWithBricks = true;
						lactor->staticFlagsBF.bIsDead = true;
					}
					break;
				}
			case 55: {
					manipActor(lactor);
					if (doCalc()) {
						actorScriptPtr = lactor->actorScript + READ_LE_S16(actorScriptPtr);
					} else {
						actorScriptPtr += 2;
					}
					break;
				}
			case 56: { // INVISIBLE
					lactor->staticFlagsBF.bNoDisplay = *(actorScriptPtr++);
					break;
				}
			case 57: {
					char temp;

					temp = *(actorScriptPtr++);

					if (temp != 0) {
						printf("Enter zoom\n");
						if (drawInGameTransBox == 0) {
							/*
							   if(zoomMode!=0)
							   {
							   FadeToBlack(menuPal);
							   //                                                           ExtInitMcga();
							   SetBackPal();
							   lockPalette=1;
							   }
							 */
						}
					} else {
						printf("Exit zoom\n");
						if (drawInGameTransBox != 0) {
							//                                                      FadeToBlack(menuPal);
							//                                                      ExtInitSvga();
							SetBackPal();
							lockPalette = 1;
							requestBackgroundRedraw = 1;
						}
					}
					break;
				}
			case 58: {
					manipActorResult = *(actorScriptPtr++);  // position flag number

					destX = flagData[manipActorResult].x;
					destZ = flagData[manipActorResult].z;
					destY = flagData[manipActorResult].y;

					lactor->X = destX;
					lactor->Z = destZ;
					lactor->Y = destY;
					break;
				}
			case 59: {
					magicLevel = *(actorScriptPtr++);
					magicPoint = magicLevel * 20;
					break;
				}
			case 60: {
					magicPoint = *(actorScriptPtr++);
					if (magicPoint < 0)
						magicPoint = 0;

					break;
				}
			case 61: {
					unsigned char temp1;
					char temp2;

					temp1 = *(actorScriptPtr++);
					temp2 = *(actorScriptPtr++);

					actors[temp1].life = temp2;
					break;
				}
			case 62: { // LM_SUB_LIFE_POINT_OBJ
					unsigned char localActorNumber;
					char subLife;

					localActorNumber = *(actorScriptPtr++);
					subLife = *(actorScriptPtr++);

					actors[localActorNumber].life -= subLife;

					if (actors[localActorNumber].life < 0)
						actors[localActorNumber].life = 0;

				}
			case 63: { //LM_HIT_OBJ
					unsigned char tempActorNumber;
					char temp;

					tempActorNumber = *(actorScriptPtr++);
					temp = *(actorScriptPtr++);
					HitObj(actorNumber, tempActorNumber, temp, actors[tempActorNumber].angle);
					break;
				}
			case 64: { //LM_PLAY_FLA
					int length = strlen((char*)actorScriptPtr);
					PlayAnimFla((char*)actorScriptPtr);
					actorScriptPtr += length + 1;
					break;
				}
			case 65: { //LM_PLAY_MIDI
					PlayMusic(*(actorScriptPtr++));
					break;
				}
			case 66: { //LM_INC_CLOVER_BOX
					if (numCloverBox < 10)
						numCloverBox++;

					break;
				}
			case 67: { //LM_SET_USED_INVENTORY
					int entryTemp;

					entryTemp = *(actorScriptPtr++);

					if (entryTemp < 24)
						itemUsed[entryTemp] = 1;

					break;
				}
			case 68: { //LM_ADD_CHOICE
					inGameMenuData[numOfOptionsInChoice++] = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;
					break;
				}
			case 69: { //LM_ASK_CHOICE
					short int choiceNum;

					freezeTime();
					TestRestoreModeSVGA(1);

					if (showTalkVar) {
						//DrawBulle(actorNumber);
					}

					TestCoulDial(lactor->talkColor);

					choiceNum = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;

					processInGameMenu(choiceNum);

					numOfOptionsInChoice = 0;

					unfreezeTime();

					fullRedraw(1);

					//waitForKey();
					break;
				}
			case 70: { //LM_BIG_MESSAGE
					int textNumber;

					freezeTime();
					TestRestoreModeSVGA(1);
					newGame2();

					/*
					 * if(showTalkVar) { DrawBulle(actorNumber); }
					 */

					TestCoulDial(lactor->talkColor);

					talkingActor = actorNumber;

					textNumber = READ_LE_S16(actorScriptPtr);

					printTextFullScreen(textNumber);

					actorScriptPtr += 2;

					newGame4();

					fullRedraw(1);

					unfreezeTime();

					// waitForKey();
					break;
				}

			case 71: { //LM_INIT_PINGOUIN
					byte newActor;
					newActor = *(actorScriptPtr++);
					actors[newActor].dynamicFlagsBF.bUnk0020 = 1;
					currentPingouin = newActor;
					actors[newActor].costumeIndex = -1;
					actors[newActor].zone = -1;
					break;
				}
			case 72: { //LM_SET_HOLO_POS
					char position;

					position = *(actorScriptPtr++);

					//setHolomapPosition(*(actorScriptPtr++));

					if (vars[0]) {
						addOverlayObject(3, 0, 0, 0, 0, 0, 3);
					}

					break;
				}
			case 73: { //LM_CLR_HOLO_POS
					char position;

					position = *(actorScriptPtr++);

					printf("Clear holomap position %d\n", position);
					break;
				}
			case 74: { //LM_ADD_FUEL
					fuel += *(actorScriptPtr++);
					if (fuel > 100) {
						fuel = 100;
					}
					break;
				}
			case 75: { //LM_SUB_FUEL
					fuel -= *(actorScriptPtr++);
					if (fuel < 0) {
						fuel = 0;
					}
					break;
				}
			case 76: { //LM_SET_GRM
					currentGrid2 = *(actorScriptPtr++);
					IncrustGrm(currentGrid2);
					break;
				}
			case 77: { //LM_SAY_MESSAGE
					short int messageNumber;

					messageNumber = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;

					addOverlayObject(4, messageNumber, 0, 0, actorNumber, 1, 2);

					freezeTime();
					setVoxFileAtDigit(messageNumber);
					unfreezeTime();

					break;
				}
			case 78: { //LM_SAY_MESSAGE_OBJ
					char character;
					short int messageNumber;

					character = *(actorScriptPtr++);
					messageNumber = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;

					addOverlayObject(4, messageNumber, 0, 0, character, 1, 2);

					freezeTime();
					setVoxFileAtDigit(messageNumber);
					unfreezeTime();

					break;
				}
			case 79: { //LM_FULL_POINT
					twinsen->life = 50;
					magicPoint = magicLevel * 20;
					break;
				}
			case 80: { //LM_BETA
					short int newAngle;

					newAngle = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;
					lactor->angle = newAngle;
					ClearRealAngle(lactor);
					break;
				}
			case 81: { //LM_GRM_OFF
					if (currentGrid2 != -1) {
						useAnotherGrm = -1;
						currentGrid2 = -1;
						createCube();
						fullRedraw(1);
						break;
					}
				}
			case 82: { //LM_FADE_PAL_RED
					printf("fade pal red\n");
					break;
				}
			case 83: { //LM_FADE_ALARM_RED
					printf("fade alarm red\n");
					break;
				}
			case 84: { //LM_FADE_ALARM_PAL
					printf("fade alarm pal\n");
					break;
				}
			case 85: { //LM_FADE_RED_PAL
					printf("fade red pal\n");
					break;
				}
			case 86: { //LM_FADE_RED_ALARM
					printf("fade red alarm\n");
					break;
				}
			case 87: { //LM_FADE_PAL_ALARM
					printf("fade pal alarm\n");
					break;
				}
			case 88: { //LM_EXPLODE_OBJ
					actorScriptPtr++;
					printf("Ignoring opcode 88 in runActorScript\n");
					break;
				}
			case 89: { //LM_BULLE_ON
					showTalkVar = 1;
					break;
				}
			case 90: { //LM_BULLE_OFF
					showTalkVar = 0;
					break;
				}
			case 91: { //LM_ASK_CHOICE_OBJ
					unsigned char currentTalkingActor;
					short int choiceNumber;

					freezeTime();
					currentTalkingActor = *(actorScriptPtr++);
					TestRestoreModeSVGA(1);

					if (showTalkVar) {
						//DrawBulle(currentTalkingActor);
					}

					TestCoulDial(actors[currentTalkingActor].talkColor);

					choiceNumber = READ_LE_S16(actorScriptPtr);
					actorScriptPtr += 2;

					processInGameMenu(choiceNumber);

					numOfOptionsInChoice = 0;

					unfreezeTime();
					fullRedraw(1);
					//waitForKey();
					break;
				}
			case 92: { //LM_SET_DARK_PAL
					freezeTime();
					Load_HQR("ress.hqr", (byte*)palette, 24);
					convertPalToRGBA(palette, paletteRGBA);
					if (!lockPalette) {
						osystem_setPalette(paletteRGBA);
					}
					useAlternatePalette = 1;
					unfreezeTime();
					break;
				}
			case 93: { //LM_SET_NORMAL_PAL
					useAlternatePalette = 0;
					if (!lockPalette) {
						osystem_setPalette(menuPalRGBA);
					}
					break;
				}
			case 94: { //LM_MESSAGE_SENDELL
					int backupFlag;

					freezeTime();
					TestRestoreModeSVGA(1);
					FadeToBlack((char*) paletteRGBA);
					Load_HQR("ress.hqr", workVideoBuffer, 25);
					CopyScreen(workVideoBuffer, frontVideoBuffer);
					Load_HQR("ress.hqr", (byte *) & palette, 26);
					convertPalToRGBA(palette, paletteRGBA);
					osystem_Flip(frontVideoBuffer);
					FadeToPal((char*)paletteRGBA);
					newGame2();
					TestCoulDial(15);
					newGameVar4 = 0;
					backupFlag = flagDisplayText;
					flagDisplayText = 1;
					printTextFullScreen(6);
					newGameVar4 = 1;
					newGame4();
					FadeToBlack((char*)paletteRGBA);
					Cls();
					osystem_setPalette(menuPalRGBA);
					flagDisplayText = backupFlag;

					do {
						readKeyboard();
					} while (skipIntro || key1);

					unfreezeTime();
					break;

				}
			case 95: // LM_ANIM_SET
				{ // play new anim, no transition
					lactor->anim = -1;
					lactor->previousAnimIndex = -1;
					InitAnim(*(actorScriptPtr++), 0, 0, actorNumber);
					break;
				}
			case 96: { // LM_HOLOMAP_TRAJ
					holomapTraj = *(actorScriptPtr++);
					break;
				}
			case 97: { // LM_GAME_OVER
					OPbreak = -1;
					twinsen->dynamicFlagsBF.bUnk0004 = 1;
					twinsen->life = 0;
					numClover = 0;
					break;
				}
			case 98: { // LM_THE_END
					OPbreak = -1;
					numClover = 0;
					twinsen->life = 50;
					magicPoint = 80;
					currentRoom = 113;
					comportementHero = startupComportementHeroInCube;
					newTwinsenX = -1;
					twinsen->angle = startupAngleInCube;
					SaveGame();
					brutalExit = 1;
					break;
				}
			case 99: { // LM_MIDI_OFF
					printf("Stop music!\n");
					break;
				}
			case 100: {
					int temp;

					temp = *(actorScriptPtr++);
					playCDtrack(temp);
					break;
				}
			case 101: { //LM_PROJ_ISO
					configureOrthoProjection(311, 240, 512);
					setSomething2(0, 0, 0);
					setSomething3(0, 0, 0);
					SetLightVector(reinitVar1, reinitVar2, 0);
					break;
				}

			case 102: //LM_PROJ_3D
				Cls();
				CopyScreen(frontVideoBuffer, workVideoBuffer);
				osystem_Flip(frontVideoBuffer);
				changeRoomVar10 = 0;

				setCameraPosition(320, 240, 128, 1024, 1024);
				setCameraAngle(0, 1500, 0, 25, -128, 0, 13000);
				SetLightVector(896, 950, 0);

				InitDial(1);
				break;
			case 103: {
					int temp;
					int esi;
					int edi;

					if (drawVar1 < 440) {
						temp = READ_LE_S16(actorScriptPtr);
						if (setup_lst != 0) {
							if (!temp)
								temp = 16;
						}

						GetMultiText(temp, string);

						actorScriptPtr += 2;

						esi = edi = SizeFont(string);

						CoulFont(15);

						Font(0, drawVar1, string);

						if (esi > 639)
							edi = 639;

						osystem_CopyBlockPhys(frontVideoBuffer, 0, drawVar1, edi, drawVar1 + 40);
						drawVar1 += 40;
					}

					break;
				}
			case 104:
				drawVar1 = 0;
				Box(0, 0, 639, 240, 0);
				osystem_CopyBlockPhys(frontVideoBuffer, 0, 0, 639, 240);
				break;
			case 105:
				brutalExit = 0;
				OPbreak = -1;
				break;
			default: {
					printf("Unhandled actorscript opcode %d\n", opcode);
					exit(1);
					break;
				}
			}
		} else {
			printf("Warning: opcode too big: %d !\n", opcode);
			exit(1);
		}
	}

}

void manipActor(actor * lactor) {
	unsigned char opcode;
	unsigned char *localScriptPtr;
	actor *lactor2;
	int temp;

	manipActorVar1 = 0;
	opcode = *(actorScriptPtr++);

	//printf("manip opcode:%d\n",opcode);

	if (opcode > 29) {
		printf("Manip actor too big!\n");
		return;
	}

	localScriptPtr = actorScriptPtr + 1;

	switch (opcode) {
	case 0:
		manipActorResult = getCOL(lactor);
		break;
	case 1:
		manipActorResult = getCOL(&actors[*(actorScriptPtr++)]);
		break;
	case 2:
		manipActorVar1 = 1;
		manipActorResult = getDISTANCE(lactor, &actors[*(actorScriptPtr++)]);
		break;
	case 3:
		manipActorResult = lactor->zone;
		break;
	case 4:
		manipActorResult = actors[*(actorScriptPtr++)].zone;
		break;
	case 5:
		manipActorResult = lactor->body;
		break;
	case 6:
		manipActorResult = actors[*(actorScriptPtr++)].body;
		break;
	case 7:
		manipActorResult = lactor->anim;
		break;
	case 8:
		manipActorResult = actors[*(actorScriptPtr++)].anim;
		break;
	case 9:
		manipActorResult = lactor->label;
		break;
	case 10:
		manipActorResult = actors[*(actorScriptPtr++)].label;
		break;
	case 11:
		manipActorResult = cubeFlags[*(actorScriptPtr++)];
		break;
	case 12: {
			short int angle;
			int newActor;

			angle = 0;    // todo: not supposed to have that

			newActor = *actorScriptPtr;
			lactor2 = &actors[newActor];
			manipActorVar1 = 1;
			actorScriptPtr = localScriptPtr;
			if (!(lactor2->dynamicFlagsBF.bUnk0020)) {
				if (lactor2->Z - lactor->Z < 1500) {
					angle = GetAngle(lactor->X, lactor->Z, lactor2->X, lactor2->Z);
					if (DoTrackVar1 > 32000)
						DoTrackVar1 = 32000;
				} else {
					DoTrackVar1 = 32000;
				}

				if (!newActor) {
					int newAngle;

					newAngle = (lactor->angle + 0x480) - (angle + 0x400);
					newAngle &= 0x3FF;

					if (newAngle >= 0x100) {
						manipActorResult = 32000;
					} else {
						manipActorResult = DoTrackVar1;
					}
				}
			} else {
				manipActorResult = 32000;
			}
			break;
		}
	case 13:
		manipActorResult = lactor->hitBy;
		break;
	case 14:
		manipActorResult = action;
		break;
	case 15:    // VAR_GAME
		temp = *(actorScriptPtr++);
		if (!vars[70] || (vars[70] && temp >= 28)) {
			manipActorResult = vars[temp];
		} else {
			if (temp == 70)
				manipActorResult = vars[temp];
			else
				manipActorResult = 0;
		}
		break;
	case 16:
		manipActorResult = lactor->life;
		break;
	case 17:
		manipActorResult = actors[*(actorScriptPtr++)].life;
		break;
	case 18:
		manipActorResult = numKey;
		break;
	case 19:
		manipActorVar1 = 1;
		manipActorResult = numCoin;
		break;
	case 20:
		manipActorResult = comportementHero;
		break;
	case 21:
		manipActorResult = chapter;
		break;
	case 22:
		lactor2 = &actors[*actorScriptPtr];
		manipActorVar1 = 1;
		actorScriptPtr = localScriptPtr;
		if (!(lactor2->dynamicFlagsBF.bUnk0020)) {
			manipActorResult =
				Distance3D(lactor->X, lactor->Z, lactor->Y, lactor2->X, lactor2->Z,
						   lactor2->Y);

			if (manipActorResult > 32000)
				manipActorResult = 32000;
		} else {
			manipActorResult = 32000;
		}
		break;
	case 25: {
			int temp;

			temp = *actorScriptPtr;
			actorScriptPtr = localScriptPtr;
			if (vars[70] == 0) {
				if (temp == mainLoopVar9) {
					manipActorResult = 1;
				} else {
					if (itemUsed[temp] == 1 && vars[temp] == 1) {
						manipActorResult = 1;
					} else {
						manipActorResult = 0;
					}
				}

				if (manipActorResult == 1)
					addOverlayObject(3, temp, 0, 0, 0, 0, 3);
			} else {
				manipActorResult = 0;
			}
			break;
		}
	case 26: {
			manipActorVar1 = 1;
			manipActorResult = inGameMenuAnswer;
			break;
		}
	case 27: {
			manipActorResult = fuel;
			break;
		}

	case 28:
		manipActorResult = lactor->standOn;
		break;

	case 29: {
			manipActorResult = 1;
			break;
		}

	default:
		printf("Unhandled manipActor opcode %d\n", opcode);
		exit(1);
	}
}

int doCalc(void) {
	unsigned char *localActorScript = actorScriptPtr;
	unsigned char opcode;
	int opcode2 = -1;
	int localManipActorResult = manipActorResult;
	unsigned char result = 0;

	opcode = *(localActorScript++);

	switch (manipActorVar1) {
	case 0: {
			opcode2 = *(localActorScript++);
			break;
		}
	case 1: {
			opcode2 = READ_LE_S16(localActorScript);
			localActorScript += 2;
			break;
		}
	default: {
			printf("Unsupported manipActorVar1 in docalc!\n");
			exit(1);
		}
	}
	switch (opcode) {
	case 0:
		if (localManipActorResult == opcode2)
			result = 1;
		break;
	case 1:
		if (localManipActorResult > opcode2)
			result = 1;
		break;
	case 2:
		if (localManipActorResult < opcode2)
			result = 1;
		break;
	case 3:
		if (localManipActorResult >= opcode2)
			result = 1;
		break;
	case 4:
		if (localManipActorResult <= opcode2)
			result = 1;
		break;
	case 5:
		if (localManipActorResult != opcode2)
			result = 1;
		break;
	default:
		printf("Unhandled doCalc %d\n", opcode);
		exit(1);
	}

	manipActorResult = localManipActorResult;
	actorScriptPtr = localActorScript;

	return (result);

}
#endif
