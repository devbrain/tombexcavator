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

#ifndef TXT_SCRIPTS

void DoTrack(int actorNumber) {
	int continueMove;
	int positionInScript;
	char currentOpcode;
	char *scriptPtr;

	int newAngle;

	actor *lactor;
	timeStruct *timePtr;

	lactor = &actors[actorNumber];
	continueMove = 1;
	timePtr = &lactor->time;

	do {
		positionInScript = lactor->positionInMoveScript;
		scriptPtr = (char *)(lactor->positionInMoveScript + lactor->moveScript);
		currentOpcode = *(scriptPtr++);
		lactor->positionInMoveScript++;

		switch (currentOpcode) {
		case 0: { // stop
				continueMove = 0;
				lactor->positionInMoveScript = -1;
				break;
			}
		case 2:
			InitBody(*scriptPtr, actorNumber);
			lactor->positionInMoveScript++;
			break;
		case 3: // ANIM
			if (InitAnim(*(scriptPtr++), 0, 0, actorNumber)) {
				lactor->positionInMoveScript++;
			} else {
				lactor->positionInMoveScript = positionInScript;
				continueMove = 0;
			}
			break;
		case 4: { // GOTO_POINT
				lactor->positionInMoveScript++;
				manipActorResult = *scriptPtr;

				destX = flagData[manipActorResult].x;
				destY = flagData[manipActorResult].y;
				destZ = flagData[manipActorResult].z;

				newAngle = GetAngle(lactor->X, lactor->Z, destX, destZ);

				if (lactor->staticFlagsBF.bIsSpriteActor) {
					lactor->angle = newAngle;
				} else {
					ManualRealAngle(lactor->angle, newAngle, lactor->speed, &lactor->time);
				}

				if (DoTrackVar1 > 500) {
					continueMove = 0;
					lactor->positionInMoveScript -= 2;
				}

				break;
			}
		case 5:
			if (!(lactor->dynamicFlagsBF.bUnk0004)) {
				continueMove = 0;
				lactor->positionInMoveScript--;
			} else {
				continueMove = 0;
				ClearRealAngle(lactor);
			}
			break;
		case 7: { // set angle
				lactor->positionInMoveScript += 2;
				if (!(lactor->staticFlagsBF.bIsSpriteActor)) {
					manipActorResult = READ_LE_S16(scriptPtr);
					if (lactor->time.numOfStep == 0) {
						ManualRealAngle(lactor->angle, manipActorResult, lactor->speed, timePtr);
					}

					if (lactor->angle != manipActorResult) {
						continueMove = 0;
						lactor->positionInMoveScript -= 3;
					}
				}
				break;
			}

		case 8: // warp to flag
			lactor->positionInMoveScript++;
			manipActorResult = *scriptPtr;

			printf("Actor %d warp to flag %d\n", actorNumber, manipActorResult);

			destX = flagData[manipActorResult].x;
			destY = flagData[manipActorResult].y;
			destZ = flagData[manipActorResult].z;

			if (lactor->staticFlagsBF.bIsSpriteActor) {
				lactor->speed = 0;
			}

			lactor->X = destX;
			lactor->Z = destZ;
			lactor->Y = destY;

			break;

		case 9: // LABEL
			lactor->label = *scriptPtr;
			lactor->positionInMoveScript++;
			lactor->currentLabelPtr = lactor->positionInMoveScript - 2;
			break;
		case 10:
			lactor->positionInMoveScript = READ_LE_S16(scriptPtr);
			break;
		case 11:  // STOP
			continueMove = 0;
			lactor->positionInMoveScript = -1;
			break;

		case 12:  // move (used in backward walk)
			lactor->positionInMoveScript++;
			manipActorResult = *scriptPtr;

			destX = flagData[manipActorResult].x;
			destY = flagData[manipActorResult].y;
			destZ = flagData[manipActorResult].z;

			newAngle = 0x200 + GetAngle(lactor->X, lactor->Z, destX, destZ);

			if (lactor->staticFlagsBF.bIsSpriteActor) {
				lactor->angle = newAngle;
			} else {
				ManualRealAngle(lactor->angle, newAngle, lactor->speed, &lactor->time);
			}

			if (DoTrackVar1 > 500) {
				continueMove = 0;
				lactor->positionInMoveScript -= 2;
			}

			break;
		case 13:
			lactor->positionInMoveScript += 2;
			if (lactor->dynamicFlagsBF.bUnk0004) {
				(*(scriptPtr + 1))++;

				if (*(scriptPtr + 1) == *scriptPtr) {
					(*(scriptPtr + 1)) = 0;
				} else {
					continueMove = 0;
				}
			} else {
				continueMove = 0;
			}

			if (continueMove == 0) {
				lactor->positionInMoveScript -= 3;
			}
			break;
		case 14:
			HQ_3D_MixSample(READ_LE_S16(scriptPtr), 0x1000, 1, lactor->X, lactor->Z, lactor->Y);
			lactor->positionInMoveScript += 2;
			break;
		case 15:
			lactor->positionInMoveScript++;
			if (lactor->staticFlagsBF.bIsSpriteActor) { // if can move
				manipActorResult = *scriptPtr;

				destX = flagData[manipActorResult].x;
				destY = flagData[manipActorResult].y;
				destZ = flagData[manipActorResult].z;

				lactor->angle = GetAngle(lactor->X, lactor->Z, destX, destZ); // X-Z move
				lactor->field_78 = GetAngle(lactor->Y, 0, destY, DoTrackVar1);  // Y (vertical) move

				if (DoTrackVar1 > 100) {
					continueMove = 0;
					lactor->positionInMoveScript -= 2;
				} else {
					lactor->X = destX;
					lactor->Z = destZ;
					lactor->Y = destY;
				}
			}
			break;
		case 16:
			lactor->positionInMoveScript += 2;
			lactor->speed = READ_LE_S16(scriptPtr);
			if (lactor->staticFlagsBF.bIsSpriteActor) {
				setActorAngle(0, lactor->speed, 50, timePtr);
			}
			break;
		case 17: // move actor to background
			lactor->positionInMoveScript += 1;
			if (*scriptPtr != 0) {
				if (!(lactor->staticFlagsBF.bIsBackgrounded)) { //if actor wasn't already in background
					//  lactor->staticFlagsBF.bIsBackgrounded = true; // set him to background
					if (lactor->dynamicFlagsBF.wasDrawn) {
						requestBackgroundRedraw = 1;
					}
				}
			} else {
				if (lactor->staticFlagsBF.bIsBackgrounded) {
					lactor->staticFlagsBF.bIsBackgrounded = false;
					if (lactor->dynamicFlagsBF.wasDrawn) {
						requestBackgroundRedraw = 1;
					}
				}
			}
			break;
		case 18: { // wait
				lactor->positionInMoveScript += 5;

				if (READ_LE_S32(scriptPtr + 1) == 0) {
					WRITE_LE_S32(scriptPtr + 1, lba_time + *(unsigned char *) scriptPtr * 50);
				}

				if (lba_time < READ_LE_S32(scriptPtr + 1)) {
					continueMove = 0;
					lactor->positionInMoveScript -= 6;
				} else {
					WRITE_LE_S32(scriptPtr + 1, 0);
				}
				break;
			}
		case 19: {
				InitBody(-1, actorNumber);
				break;
			}
		case 20: {
				short int beta;

				beta = READ_LE_S16(scriptPtr);
				lactor->positionInMoveScript += 2;

				lactor->angle = beta;

				if (lactor->staticFlagsBF.bIsSpriteActor) {
					ClearRealAngle(lactor);
				}

				break;
			}
		case 21:
		case 22:
		case 23:
		case 24:
			lactor->positionInMoveScript += 2;
			if (lactor->staticFlagsBF.bIsSpriteActor && lactor->staticFlagsBF.bIsUsingClipping) {
				switch (currentOpcode - 21) {
				case 0:
					lactor->angle = 0x300;
					break;
				case 1:
					lactor->angle = 0x100;
					break;
				case 2:
					lactor->angle = 0x200;
					break;
				case 3:
					lactor->angle = 0;
					break;
				default:
					printf("Unsupported subopcode of actor move opcode 21-24!\n");
					exit(1);
				}

				lactor->doorStatus = READ_LE_S16(scriptPtr);
				lactor->dynamicFlagsBF.bIsMoving = 1;
				lactor->speed = 1000;
				setActorAngle(0, 1000, 50, timePtr);
			}
			break;
		case 25: { // TM_CLOSE
				if (lactor->staticFlagsBF.bIsSpriteActor && lactor->staticFlagsBF.bIsUsingClipping) {
					lactor->dynamicFlagsBF.bIsMoving = 1;
					lactor->doorStatus = 0;
					lactor->speed = -1000;
					setActorAngle(0, -1000, 50, timePtr);
				}
				break;
			}
		case 26: { // TM_WAIT_DOOR
				if (lactor->staticFlagsBF.bIsSpriteActor && lactor->staticFlagsBF.bIsUsingClipping) {
					if (lactor->speed) {
						continueMove = 0;
						lactor->positionInMoveScript--;
					}
				}
				break;
			}
		case 27: {
				printf("skipping actor move opcode 27\n");
				lactor->positionInMoveScript += 2;
				break;
			}

		case 28:
			HQ_3D_MixSample(READ_LE_S16(scriptPtr), 0x1000, 0, lactor->X, lactor->Z, lactor->Y);
			lactor->positionInMoveScript += 2;
			break;
		case 29:
			HQ_3D_MixSample(READ_LE_S16(scriptPtr), 0x1000, 0, lactor->X, lactor->Z, lactor->Y);
			lactor->positionInMoveScript += 2;
			break;
		case 31: {
				moveVar1 = READ_LE_S16(scriptPtr);
				lactor->positionInMoveScript += 2;
				break;
			}
		case 32: {
				printf("skipping actor move opcode 31 (playSound at moveVar1)\n");
				lactor->positionInMoveScript += 2;
				break;
			}
		case 33: { // look at twinsen
				lactor->positionInMoveScript += 2;
				if (!(lactor->staticFlagsBF.bIsSpriteActor)) {
					manipActorResult = READ_LE_S16(scriptPtr);
					if (manipActorResult == -1 && lactor->time.numOfStep == 0) {
						manipActorResult = GetAngle(lactor->X, lactor->Z, twinsen->X, twinsen->Z);
						ManualRealAngle(lactor->angle, manipActorResult, lactor->speed, &lactor->time);
						WRITE_LE_S16(scriptPtr, manipActorResult);
					}

					if (lactor->angle != manipActorResult) {
						continueMove = 0;
						lactor->positionInMoveScript -= 3;
					} else {
						ClearRealAngle(lactor);
						WRITE_LE_S16(scriptPtr, -1);
					}
				}
				break;
			}
		case 34: { // angle random
				int var_10;

				lactor->positionInMoveScript += 4 ;
				if (!(lactor->staticFlagsBF.bIsSpriteActor)) {
					manipActorResult = READ_LE_S16(scriptPtr + 2);

					if (manipActorResult == -1  && lactor->time.numOfStep == 0) {
						if (rand()&1) {
							manipActorResult = READ_LE_S16(scriptPtr);
							var_10 = lactor->angle + 0x100 + (abs(manipActorResult) >> 1);

							manipActorResult = (var_10 - (rand() % manipActorResult)) & 0x3FF;
						} else {
							manipActorResult = READ_LE_S16(scriptPtr);
							var_10 = lactor->angle - 0x100 + (abs(manipActorResult) >> 1);

							manipActorResult = (var_10 - (rand() % manipActorResult)) & 0x3FF;
						}

						ManualRealAngle(lactor->angle, manipActorResult, lactor->speed, &lactor->time);
						WRITE_LE_S16(scriptPtr + 2, manipActorResult);
					}

					if (lactor->angle != manipActorResult) {
						continueMove = 0;
						lactor->positionInMoveScript -= 5;
					} else {
						ClearRealAngle(lactor);
						WRITE_LE_S16(scriptPtr + 2, -1);
					}
				}
				break;
			}
		default:
			printf("Unsupported move opcode %d\n", currentOpcode);
			exit(1);
		}
	} while (continueMove);
}

#else

char trackMacroList[][40] = {
	"END",
	"NOP",
	"BODY",
	"ANIM",
	"GOTO_POINT",
	"WAIT_ANIM", // 5
	"LOOP",
	"ANGLE",
	"POS_POINT",
	"LABEL",
	"GOTO", // 10
	"STOP",
	"GOTO_SYM_POINT",
	"WAIT_NB_ANIM",
	"SAMPLE",
	"GOTO_POINT_3D", // 15
	"SPEED",
	"BACKGROUND",
	"WAIT_NB_SECOND",
	"NO_BODY",
	"BETA", // 20
	"OPEN_LEFT",
	"OPEN_RIGHT",
	"OPEN_UP",
	"OPEN_DOWN",
	"CLOSE", // 25
	"WAIT_DOOR",
	"SAMPLE_RND",
	"SAMPLE_ALWAYS",
	"SAMPLE_STOP",
	"PLAY_FLA", // 30
	"REPEAT_SAMPLE",
	"SIMPLE_SAMPLE",
	"FACE_TWINKEL",
	"ANGLE_RND"
};

int resolveTrackMacroIdx(char* lineBuffer) {
	int i;
	char buffer[256];
	char* ptr;

	strcpy(buffer, lineBuffer);

	ptr = strchr(buffer, ' ');

	if (ptr) {
		*ptr = 0;
	}

	for (i = 0;i <= (sizeof(trackMacroList) / 40);i++) {
		if (strlen(trackMacroList[i])) {
			if (!strcmp(trackMacroList[i], buffer))
				return i;
		}
	}

	return -1;
}

actor* currentTrackActor;

int resolveTrackLineFromLabel(actor* actorPtr, int trackPosition) {
	char buffer[256];
	int lineIdx = 0;

	do {
		getLine(actorPtr->moveScript, buffer, lineIdx);

		if (strstr(buffer, "LABEL ")) {
			int labelIdx;

			sscanf(buffer, "LABEL %d", &labelIdx);

			if (labelIdx == trackPosition)
				return lineIdx;
		}

		lineIdx++;
	} while (!strstr(buffer, "END "));

	return -1;
}

void DoTrack(int actorNumber) {
	int breakTrack = 0;
	char lineBuffer[256];
	int macroIdx;
	int currentLine = actors[actorNumber].positionInMoveScript;

	currentTrackActor = &actors[actorNumber];

	while (!breakTrack) {
		char* tempPtr;

		currentLine = currentTrackActor->positionInMoveScript;

		getLine(actors[actorNumber].moveScript, lineBuffer, currentLine);

		tempPtr = lineBuffer;

		while (*tempPtr == ' ') { // remove indentation
			tempPtr++;
		}

		macroIdx = resolveTrackMacroIdx(tempPtr);

		assert(macroIdx != -1);

		switch (macroIdx) {
		case 2: { // BODY
				int bodyIdx;

				verify(sscanf(tempPtr, "BODY %d", &bodyIdx) == 1);

				InitBody(bodyIdx, actorNumber);
				currentTrackActor->positionInMoveScript++;
				break;
			}
		case 3: { // ANIM
				int animIdx;

				verify(sscanf(tempPtr, "ANIM %d", &animIdx) == 1);

				if (InitAnim(animIdx, 0, 0, actorNumber)) {
					currentTrackActor->positionInMoveScript++;
				} else {
					breakTrack = 1;
				}
				break;
			}
		case 4: { // GOTO_POINT
				int newAngle;
				int pointIdx;

				verify(sscanf(tempPtr, "GOTO_POINT %d", &pointIdx) == 1);

				destX = flagData[pointIdx].x;
				destY = flagData[pointIdx].y;
				destZ = flagData[pointIdx].z;

				newAngle = GetAngle(currentTrackActor->X, currentTrackActor->Z, destX, destZ);

				if (currentTrackActor->staticFlagsBF.bIsSpriteActor) {
					currentTrackActor->angle = newAngle;
				} else {
					ManualRealAngle(currentTrackActor->angle, newAngle, currentTrackActor->speed, &currentTrackActor->time);
				}

				if (DoTrackVar1 > 500) {
					breakTrack = 1;
				} else {
					currentTrackActor->positionInMoveScript++;
				}

				break;
			}
		case 5: { // WAIT_ANIM
				if (!(currentTrackActor->dynamicFlagsBF.bUnk0004)) {
					breakTrack = 1;
				} else {
					breakTrack = 1;
					ClearRealAngle(currentTrackActor);
					currentTrackActor->positionInMoveScript++;
				}
				break;
			}
		case 7: { // set angle
				int angle;

				verify(sscanf(tempPtr, "ANGLE %d", &angle) == 1);

				if (!(currentTrackActor->staticFlagsBF.bIsSpriteActor)) {
					if (currentTrackActor->time.numOfStep == 0) {
						ManualRealAngle(currentTrackActor->angle, angle, currentTrackActor->speed, &currentTrackActor->time);
					}

					if (currentTrackActor->angle != angle) {
						breakTrack = 1;
					} else {
						currentTrackActor->positionInMoveScript++;
					}
				}
				break;
			}
		case 8: { // POS_POINT
				int flagIdx;

				verify(sscanf(tempPtr, "POS_POINT %d", &flagIdx) == 1);

				currentTrackActor->positionInMoveScript++;

				destX = flagData[flagIdx].x;
				destY = flagData[flagIdx].y;
				destZ = flagData[flagIdx].z;

				if (currentTrackActor->staticFlagsBF.bIsSpriteActor) {
					currentTrackActor->speed = 0;
				}

				currentTrackActor->X = destX;
				currentTrackActor->Y = destY;
				currentTrackActor->Z = destZ;

				break;
			}
		case 9: { // LABEL
				int labelIdx;

				verify(sscanf(tempPtr, "LABEL %d", &labelIdx) == 1);

				currentTrackActor->label = labelIdx;
				currentTrackActor->positionInMoveScript++;
				currentTrackActor->currentLabelPtr = currentLine;

				break;
			}
		case 10: { // GOTO
				int labelIdx;

				verify(sscanf(tempPtr, "GOTO %d", &labelIdx) == 1);

				currentTrackActor->positionInMoveScript = resolveTrackLineFromLabel(currentTrackActor, labelIdx);

				break;
			}
		case 11: { // STOP
				breakTrack = 1;
				currentTrackActor->positionInMoveScript = -1;
				break;
			}
		case 13: { // WAIT_NB_ANIM
				int waitNumber;

				verify(sscanf(tempPtr, "WAIT_NB_ANIM %d", &waitNumber) == 1);

				if (currentTrackActor->dynamicFlagsBF.bUnk0004) {
					currentTrackActor->timeDelay++;

					if (currentTrackActor->timeDelay == waitNumber) {
						currentTrackActor->timeDelay = 0;
						currentTrackActor->positionInMoveScript++;
					} else {
						breakTrack = 1;
					}
				} else {
					breakTrack = 1;
				}
				break;
			}
		case 14: { // SAMPLE
				// TODO
				currentTrackActor->positionInMoveScript++;
				break;
			}
		case 15: { // GOTO_POINT_3D
				int flagIdx;

				verify(sscanf(tempPtr, "GOTO_POINT_3D %d", &flagIdx) == 1);

				if (currentTrackActor->staticFlagsBF.bIsSpriteActor) { // if can move
					destX = flagData[flagIdx].x;
					destZ = flagData[flagIdx].z;
					destY = flagData[flagIdx].y;

					currentTrackActor->angle = GetAngle(currentTrackActor->X, currentTrackActor->Z, destX, destZ);  // X-Y move
					currentTrackActor->field_78 = GetAngle(currentTrackActor->Y, 0, destY, DoTrackVar1);  // Z (vertical) move

					if (DoTrackVar1 > 100) {
						breakTrack = 1;
					} else {
						currentTrackActor->X = destX;
						currentTrackActor->Z = destZ;
						currentTrackActor->Y = destY;

						currentTrackActor->positionInMoveScript++;
					}
				}
				break;
			}
		case 16: { // SPEED
				int speed;

				verify(sscanf(tempPtr, "SPEED %d", &speed) == 1);

				currentTrackActor->positionInMoveScript ++;
				currentTrackActor->speed = speed;

				if (currentTrackActor->staticFlagsBF.bIsSpriteActor) {
					setActorAngle(0, currentTrackActor->speed, 50, &currentTrackActor->time);
				}
				break;
			}
		case 17: { // BACKGROUND
				int backgroundFlag;

				verify(sscanf(tempPtr, "BACKGROUND %d", &backgroundFlag) == 1);

				currentTrackActor->positionInMoveScript ++;
				/*  if (backgroundFlag != 0)
				  {
				    if (!(currentTrackActor->staticFlagsBF.bIsBackgrounded)) //if actor wasn't already in background
				    {
				      currentTrackActor->staticFlagsBF.bIsBackgrounded = true; // set him to background
				      if (currentTrackActor->dynamicFlagsBF.wasDrawn)
				      {
				        requestBackgroundRedraw = 1;
				      }
				    }
				  }
				  else
				  {
				    if ( currentTrackActor->staticFlagsBF.bIsBackgrounded )
				    {
				      currentTrackActor->staticFlagsBF.bIsBackgrounded = false;
				      if (currentTrackActor->dynamicFlagsBF.wasDrawn)
				      {
				        requestBackgroundRedraw = 1;
				      }
				    }
				  }*/
				break;
			}
		case 18: { // WAIT_NB_SECOND
				int numSecondes;

				verify(sscanf(tempPtr, "WAIT_NB_SECOND %d", &numSecondes) == 1);

				if (currentTrackActor->timeDelay == 0) {
					currentTrackActor->timeDelay = lba_time + numSecondes * 50;
				}

				if (lba_time < currentTrackActor->timeDelay) {
					breakTrack = 1;
				} else {
					currentTrackActor->timeDelay = 0;
					currentTrackActor->positionInMoveScript++;
				}

				break;
			}
		case 21:
		case 22:
		case 23:
		case 24: {
				int doorStatus;

				currentTrackActor->positionInMoveScript++;

				if (currentTrackActor->staticFlagsBF.bIsSpriteActor && currentTrackActor->staticFlagsBF.bIsUsingClipping) {
					switch (macroIdx - 21) {
					case 0:
						verify(sscanf(tempPtr, "OPEN_LEFT %d", &doorStatus) == 1);
						currentTrackActor->angle = 0x300;
						break;
					case 1:
						verify(sscanf(tempPtr, "OPEN_RIGHT %d", &doorStatus) == 1);
						currentTrackActor->angle = 0x100;
						break;
					case 2:
						verify(sscanf(tempPtr, "OPEN_UP %d", &doorStatus) == 1);
						currentTrackActor->angle = 0x200;
						break;
					case 3:
						verify(sscanf(tempPtr, "OPEN_DOWN %d", &doorStatus) == 1);
						currentTrackActor->angle = 0;
						break;
					default:
						printf("Unsupported subopcode of actor move opcode 21-24!\n");
						exit(1);
					}

					currentTrackActor->doorStatus = doorStatus;
					currentTrackActor->dynamicFlagsBF.bIsMoving = 1;
					currentTrackActor->speed = 1000;
					setActorAngle(0, 1000, 50, &currentTrackActor->time);
				}
				break;
			}
		case 25: { // CLOSE
				if (currentTrackActor->staticFlagsBF.bIsSpriteActor && currentTrackActor->staticFlagsBF.bIsUsingClipping) {
					currentTrackActor->dynamicFlagsBF.bIsMoving = 1;
					currentTrackActor->doorStatus = 0;
					currentTrackActor->speed = -1000;
					setActorAngle(0, -1000, 50, &currentTrackActor->time);
				}
				currentTrackActor->positionInMoveScript++;
				break;
			}
		case 26: {
				if (currentTrackActor->staticFlagsBF.bIsSpriteActor && currentTrackActor->staticFlagsBF.bIsUsingClipping) {
					if (currentTrackActor->speed) {
						breakTrack = 1;
					} else {
						currentTrackActor->positionInMoveScript++;
					}
				} else {
					currentTrackActor->positionInMoveScript++;
				}
				break;
			}
		case 28: { // SAMPLE_ALWAYS
				// TODO!
				currentTrackActor->positionInMoveScript++;
				break;
			}
		case 29: { // SAMPLE_STOP
				// TODO!
				currentTrackActor->positionInMoveScript++;
				break;
			}
		case 33: { // FACE_TWINKEL
				if (!(currentTrackActor->staticFlagsBF.bIsSpriteActor)) {
					if (currentTrackActor->faceTwinkenVar == -1 && currentTrackActor->time.numOfStep == 0) {
						currentTrackActor->faceTwinkenVar = GetAngle(currentTrackActor->X, currentTrackActor->Z, twinsen->X, twinsen->Z);
						ManualRealAngle(currentTrackActor->angle, currentTrackActor->faceTwinkenVar, currentTrackActor->speed, &currentTrackActor->time);
					}

					if (currentTrackActor->angle != currentTrackActor->faceTwinkenVar) {
						breakTrack = 1;
					} else {
						ClearRealAngle(currentTrackActor);
						currentTrackActor->faceTwinkenVar = -1;
						currentTrackActor->positionInMoveScript++;
					}
				} else {
					currentTrackActor->positionInMoveScript++;
				}
				break;
			}
		default: {
				exit(1);
			}
		}
	}
}

#endif

void ManualRealAngle(int angleFrom, int angleTo, int angleSpeed, timeStruct * angleStruct) {
	// it's supposed to work with shorts, but for some reasons, it doesn't always.... (numOfStep overflow when lFrom is 0x100 and lTo is 0x300)...
	long int numOfStepLong;
	short int numOfStep;
	short int lFrom;
	short int lTo;

	lFrom = angleFrom & 0x3FF;
	lTo = angleTo & 0x3FF;

	angleStruct->from = lFrom;
	angleStruct->to = lTo;

	numOfStep = (lFrom - lTo) << 6;

	if (numOfStep < 0) {
		numOfStepLong = -numOfStep;
	} else {
		numOfStepLong = numOfStep;
	}

	numOfStepLong >>= 6;

	numOfStepLong *= angleSpeed;
	numOfStepLong >>= 8;

	angleStruct->numOfStep = (short int)numOfStepLong;
	angleStruct->timeOfChange = lba_time;
}
