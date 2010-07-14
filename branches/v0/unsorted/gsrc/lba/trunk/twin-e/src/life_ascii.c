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

#ifdef TXT_SCRIPTS

actor* currentScriptActor;

void getLine(char* script, char* buffer, int line) {
	int i;
	char* temp;

	for (i = 0;i < line;i++) {
		script = strchr(script, 0x0A) + 1;
	}

	temp = strchr(script, 0x0A);
	*temp = 0;

	strcpy(buffer, script);

	*temp = 0xA;
}

int resolveComportementLineFromIdx(char* scriptPtr, int comportementIdx) {
	char buffer[256];
	int lineCount = 0;
	int tempComportement;

	do {
		getLine(scriptPtr, buffer, lineCount);

		if (strstr(buffer, "COMPORTEMENT ")) {
			if (sscanf(buffer, "COMPORTEMENT %d", &tempComportement) == 1) {
				if (tempComportement == comportementIdx) {
					return lineCount;
				}
			}
		}
		lineCount++;
	} while (strcmp(buffer, "END"));

	exit(1);
}

void replaceLine(char* script, char* newLine, int line) {
	int i;
	char* temp;

	for (i = 0;i < line;i++) {
		script = strchr(script, 0x0A) + 1;
	}

	temp = strchr(script, 0x0A);
	*temp = 0;

	strcpy(script, newLine);

	*temp = 0xA;
}

char lifeMacroList[][40] = {
	"END", // 0
	"NOP",
	"SNIF",
	"OFFSET",
	"NEVERIF",
	"", //5
	"NO_IF",
	"",
	"",
	"",
	"LABEL", // 10
	"RETURN",
	"IF",
	"SWIF",
	"ONEIF",
	"ELSE", // 15
	"ENDIF",
	"BODY",
	"BODY_OBJ",
	"ANIM",
	"ANIM_OBJ", // 20
	"SET_LIFE",
	"SET_LIFE_OBJ",
	"SET_TRACK",
	"SET_TRACK_OBJ",
	"MESSAGE", // 25
	"FALLABLE",
	"SET_DIR",
	"SET_DIR_OBJ",
	"CAM_FOLLOW",
	"COMPORTEMENT_HERO", // 30
	"SET_FLAG_CUBE",
	"COMPORTEMENT",
	"SET_COMPORTEMENT",
	"SET_COMPORTEMENT_OBJ",
	"END_COMPORTEMENT", // 35
	"SET_FLAG_GAME",
	"KILL_OBJ",
	"SUICIDE",
	"USE_ONE_LITTLE_KEY",
	"GIVE_GOLD_PIECES", // 40
	"END_LIFE",
	"STOP_L_TRACK",
	"RESTORE_L_TRACK",
	"MESSAGE_OBJ",
	"INC_CHAPTER", // 45
	"FOUND_OBJECT",
	"SET_DOOR_LEFT",
	"SET_DOOR_RIGHT",
	"SET_DOOR_UP",
	"SET_DOOR_DOWN", // 50
	"GIVE_BONUS",
	"CHANGE_CUBE",
	"OBJ_COL",
	"BRICK_COL",
	"OR_IF", // 55
	"INVISIBLE",
	"ZOOM",
	"POS_POINT",
	"SET_MAGIC_LEVEL",
	"SUB_MAGIC_POINT", // 60
	"SET_LIFE_POINT_OBJ",
	"SUB_LIFE_POINT_OBJ",
	"HIT_OBJ",
	"PLAY_FLA",
	"PLAY_MIDI", // 65
	"INC_CLOVER_BOX",
	"SET_USED_INVENTORY",
	"ADD_CHOICE",
	"ASK_CHOICE",
	"BIG_MESSAGE", // 70
	"INIT_PINGOUIN",
	"SET_HOLO_POS",
	"CLR_HOLO_POS",
	"ADD_FUEL",
	"SUB_FUEL", // 75
	"SET_GRM",
	"SAY_MESSAGE",
	"SAY_MESSAGE_OBJ",
	"FULL_POINT",
	"BETA", // 80
	"GRM_OFF",
	"FADE_PAL_RED",
	"FADE_ALARM_RED",
	"FADE_ALARM_PAL",
	"FADE_RED_PAL", // 85
	"FADE_RED_ALARM",
	"FADE_PAL_ALARM",
	"EXPLODE_OBJ",
	"BULLE_ON",
	"BULLE_OFF", // 90
	"ASK_CHOICE_OBJ",
	"SET_DARK_PAL",
	"SET_NORMAL_PAL",
	"MESSAGE_SENDELL",
	"ANIM_SET", // 95
	"HOLOMAP_TRAJ",
	"GAME_OVER",
	"THE_END",
	"MIDI_OFF",
	"PLAY_CD_TRACK", // 100
	"PROJ_ISO",
	"PROJ_3D",
	"TEXT",
	"CLEAR_TEXT",
	"BRUTAL_EXIT" // 105
};

char varNames[][40] = {
	"COL",
	"COL_OBJ",
	"DISTANCE",
	"ZONE",
	"ZONE_OBJ",
	"BODY",
	"BODY_OBJ",
	"ANIM",
	"ANIM_OBJ",
	"L_TRACK",
	"L_TRACK_OBJ",
	"FLAG_CUBE",
	"CONE_VIEW",
	"HIT_BY",
	"ACTION",
	"FLAG_GAME",
	"LIFE_POINT",
	"LIFE_POINT_OBJ",
	"NB_LITTLE_KEYS",
	"NB_GOLD_PIECES",
	"COMPORTEMENT_HERO",
	"CHAPTER",
	"DISTANCE_3D",
	"USE_INVENTORY",
	"CHOICE",
	"FUEL",
	"CARRY_BY",
	"CDROM"
};

int resolveLifeMacroIdx(char* lineBuffer) {
	int i;
	char buffer[256];
	char* ptr;

	strcpy(buffer, lineBuffer);

	ptr = strchr(buffer, ' ');

	if (ptr) {
		*ptr = 0;
	}

	for (i = 0;i <= (sizeof(lifeMacroList) / 40);i++) {
		if (strlen(lifeMacroList[i])) {
			if (!strcmp(lifeMacroList[i], buffer))
				return i;
		}
	}

	return -1;
}

int resolveVarMacroIdx(char* lineBuffer) {
	char* temp;
	int i;
	char buffer[256];

	strcpy(buffer, lineBuffer);

	temp = strchr(buffer, ' ');

	if (temp)
		*temp = 0;

	for (i = 0;i <= (sizeof(varNames) / 40);i++) {
		if (strlen(varNames[i])) {
			if (!strcmp(varNames[i], buffer))
				return i;
		}
	}

	return -1;
}

char* resolveVar(char* var, int* outputVar) {
	char testBuffer[256];
	int numerical;
	int varType;
	char* returnPtr;
	int numAdditionalArg = 0;
	int i;

	// test numerical
	numerical = atoi(var);
	sprintf(testBuffer, "%d", numerical);
	if (!strcmp(var, testBuffer)) {
		*outputVar = numerical;

		returnPtr = (var + strlen(testBuffer)) + 1;

		return returnPtr;
	}

	varType = resolveVarMacroIdx(var);

	assert(varType != -1);

	switch (varType) {
	case 0: { // COL
			*outputVar = getCOL(currentScriptActor);
			break;
		}
	case 1: { // COL_OBJ
			int actorIdx;

			verify(sscanf(var, "COL_OBJ %d ", &actorIdx) == 1);

			*outputVar = getCOL(&actors[actorIdx]);

			numAdditionalArg = 1;
			break;
		}
	case 2: { // DISTANCE
			int actorIdx;

			verify(sscanf(var, "DISTANCE %d ", &actorIdx) == 1);

			*outputVar = getDISTANCE(currentScriptActor, &actors[actorIdx]);

			numAdditionalArg = 1;

			break;
		}
	case 3: { // ZONE
			*outputVar = currentScriptActor->zone;
			break;
		}
	case 4: { // ZONE_OBJ
			int actor2;
			actor* lactor2;

			sscanf(var, "ZONE_OBJ %d ", &actor2);

			lactor2 = &actors[actor2];

			*outputVar = lactor2->zone;

			numAdditionalArg = 1;

			break;
		}
	case 5: { // BODY
			*outputVar = currentScriptActor->body;
			break;
		}
	case 6: { // BODY_OBJ
			int actor2;
			actor* lactor2;

			sscanf(var, "BODY_OBJ %d ", &actor2);

			lactor2 = &actors[actor2];

			*outputVar = lactor2->body;

			numAdditionalArg = 1;

			break;
		}
	case 7: { // ANIM
			*outputVar = currentScriptActor->anim;
			break;
		}
	case 8: { // ANIM_OBJ
			int actor2;
			actor* lactor2;

			sscanf(var, "ANIM_OBJ %d ", &actor2);

			lactor2 = &actors[actor2];

			*outputVar = lactor2->anim;

			numAdditionalArg = 1;

			break;
		}
	case 9: { // L_TRACK
			*outputVar = currentScriptActor->label;
			break;
		}
	case 10: { // L_TRACK_OBJ
			int actor2;
			actor* lactor2;

			verify(sscanf(var, "L_TRACK_OBJ %d ", &actor2) == 1);

			lactor2 = &actors[actor2];

			*outputVar = lactor2->label;

			numAdditionalArg = 1;

			break;
		}
	case 11: { // FLAG_CUBE
			int varIdx;

			verify(sscanf(var, "FLAG_CUBE %d", &varIdx) == 1);
			*outputVar = cubeFlags[varIdx];

			numAdditionalArg = 1;

			break;
		}
	case 12: { // CONE_VIEW
			short int angle;
			int newActor;
			actor* lactor2;

			angle = 0;		// todo: not supposed to have that

			sscanf(var, "CONE_VIEW %d", &newActor);

			lactor2 = &actors[newActor];
			manipActorVar1 = 1;

			if (!(lactor2->dynamicFlagsBF.bUnk0020)) {
				if (lactor2->Y - currentScriptActor->Y < 1500) {
					angle = GetAngle(currentScriptActor->X, currentScriptActor->Z, lactor2->X, lactor2->Z);
					if (DoTrackVar1 > 32000)
						DoTrackVar1 = 32000;
				} else {
					DoTrackVar1 = 32000;
				}

				if (!newActor) {
					int newAngle;

					newAngle = (currentScriptActor->angle + 0x480) - (angle + 0x400);
					newAngle &= 0x3FF;

					if (newAngle >= 0x100) {
						*outputVar = 32000;
					} else {
						*outputVar = DoTrackVar1;
					}
				}
			} else {
				*outputVar = 32000;
			}

			numAdditionalArg = 1;

			break;
		}
	case 13: { // HIT_BY
			*outputVar = currentScriptActor->hitBy;
			break;
		}
	case 14: { // ACTION
			*outputVar = action;
			break;
		}
	case 15: { // FLAG_GAME
			int varIdx;

			verify(sscanf(var, "FLAG_GAME %d ", &varIdx) == 1);

			if (!vars[70] || (vars[70] && varIdx >= 28)) {
				*outputVar = vars[varIdx];
			} else {
				if (varIdx == 70)
					*outputVar = vars[varIdx];
				else
					*outputVar = 0;
			}

			numAdditionalArg = 1;

			break;
		}
	case 16: { // LIFE_POINT
			*outputVar = currentScriptActor->life;
			break;
		}
	case 17: { // LIFE_POINT_OBJ
			int actorIdx;

			verify(sscanf(var, "LIFE_POINT_OBJ %d", &actorIdx) == 1);

			*outputVar = actors[actorIdx].life;

			numAdditionalArg = 1;

			break;
		}
	case 18: {
			*outputVar = numKey;
			break;
		}
	case 19: {
			manipActorVar1 = 1;
			*outputVar = numCoin;
			break;
		}
	case 20: { // COMPORTEMENT_HERO
			*outputVar = comportementHero;
			break;
		}
	case 21: { // CHAPTER
			*outputVar = chapter;
			break;
		}
	case 22: { // DISTANCE_3D
			int actor2;
			actor* lactor2;

			verify(sscanf(var, "DISTANCE_3D %d ", &actor2) == 1);

			lactor2 = &actors[actor2];

			manipActorVar1 = 1;
			numAdditionalArg = 1;
			if (!(lactor2->dynamicFlagsBF.bUnk0020)) {
				*outputVar = Distance3D(currentScriptActor->X, currentScriptActor->Y, currentScriptActor->Z, lactor2->X, lactor2->Y, lactor2->Z);

				if (*outputVar > 32000)
					*outputVar = 32000;
			} else {
				*outputVar = 32000;
			}
			break;
		}
	default: {
			exit(1);
		}
	}

	returnPtr = (var + strlen(varNames[varType])) + 1;

	for (i = 0;i < numAdditionalArg;i++) {
		returnPtr = strchr(returnPtr, ' ') + 1;
	}

	return returnPtr;
}

int resolveCondition(char* condition) {
	char buffer[256];
	char* var1;
	char* var2;
	char* testType;

	int var1Value;
	int var2Value;

	char* temp;

	strcpy(buffer, condition);

	testType = resolveVar(buffer, &var1Value);

	temp = strchr(testType, ' ');
	*temp = 0;

	resolveVar(temp + 1, &var2Value);

	if (!strcmp(testType, "==")) {
		if (var1Value == var2Value)
			return 1;
		return 0;
	}

	if (!strcmp(testType, ">")) {
		if (var1Value > var2Value)
			return 1;
		return 0;
	}

	if (!strcmp(testType, ">=")) {
		if (var1Value >= var2Value)
			return 1;
		return 0;
	}

	if (!strcmp(testType, "<")) {
		if (var1Value < var2Value)
			return 1;
		return 0;
	}

	if (!strcmp(testType, "<=")) {
		if (var1Value <= var2Value)
			return 1;
		return 0;
	}

	if (!strcmp(testType, "!=")) {
		if (var1Value != var2Value)
			return 1;
		return 0;
	}

	exit(1);
}

int getIndentationLevel(char* line) {
	int indentationLevel = 0;

	while (*line == ' ') {
		line += 2;
		indentationLevel++;
	}

	return indentationLevel;
}

int seekSameLevelString(char* seekString, char* script, int startLine) { // script MUST be properly indented for this to work !
	char buffer[256];
	int indentation;
	int currentLineSeek = startLine;
	int indentation2;

	getLine(script, buffer, startLine);

	indentation = getIndentationLevel(buffer);

	do {
		currentLineSeek++;
		getLine(script, buffer, currentLineSeek);
		indentation2 = getIndentationLevel(buffer);

		if (indentation2 == indentation) {
			if (strstr(buffer, seekString)) {
				return currentLineSeek;
			}

			if (strstr(buffer, "ENDIF")) {
				return -1;
			}
		}
	} while (indentation <= indentation2);

	return -1;
}

int resolveIfNextInstruction(char* script, int currentLine, int condition) {
	int elseLine;
	int endifLine;
	int conditionEntry;
	int currentLineSeek = currentLine;

	char buffer[256];
	int indentation;
	int indentation2;

	getLine(script, buffer, currentLine);

	indentation = getIndentationLevel(buffer);

	do {
		currentLineSeek++;
		getLine(script, buffer, currentLineSeek);
		indentation2 = getIndentationLevel(buffer);
	} while (indentation == indentation2);

	conditionEntry = currentLineSeek;

	elseLine = seekSameLevelString("ELSE", script, currentLine);
	endifLine = seekSameLevelString("ENDIF", script, currentLine);

	if (condition)
		return conditionEntry;

	if (elseLine != -1)
		return elseLine + 1;

	return endifLine + 1;
}

void runActorScript(short int actorNumber) {
	int breakLife = 0;
	char lineBuffer[256];
	int macroIdx;
	int currentLine = actors[actorNumber].positionInActorScript;

	currentScriptActor = &actors[actorNumber];

	while (!breakLife) {
		char* tempPtr;

		getLine(actors[actorNumber].actorScript, lineBuffer, currentLine);

		tempPtr = lineBuffer;

		while (*tempPtr == ' ') { // remove indentation
			tempPtr++;
		}

		macroIdx = resolveLifeMacroIdx(tempPtr);

		assert(macroIdx != -1);

		switch (macroIdx) {
		case 0: { // END
				breakLife = 1;
				currentScriptActor->positionInActorScript = -1;
				break;
			}
		case 6: { // NO_IF
				currentLine = resolveIfNextInstruction(actors[actorNumber].actorScript, currentLine, 0) - 1;
				break;
			}
		case 10: { // LABEL
				break;
			}
		case 11: { // RETURN
				breakLife = 1;
				break;
			}
		case 12: { // IF
				char* temp;
				int result = resolveCondition(tempPtr + 3);

				currentLine = resolveIfNextInstruction(actors[actorNumber].actorScript, currentLine, result) - 1;

				break;
			}
		case 13: { // SWIF
				char* temp;
				int result = resolveCondition(tempPtr + 5);

				currentLine = resolveIfNextInstruction(actors[actorNumber].actorScript, currentLine, result) - 1;

				if (result) {
					tempPtr[0] = 'W';
					tempPtr[1] = 'S';
				}

				break;
			}
		case 14: { // ONEIF
				int oldLine = currentLine;
				char* temp;
				int result = resolveCondition(tempPtr + 6);

				currentLine = resolveIfNextInstruction(actors[actorNumber].actorScript, currentLine, result) - 1;

				if (result) {
					temp = strstr(lineBuffer, "ONEIF");

					temp[0] = 'N';
					temp[1] = 'O';
					temp[2] = '_';

					replaceLine(actors[actorNumber].actorScript, lineBuffer, oldLine);
				}
				break;
			}
		case 15: { // ELSE
				currentLine = resolveIfNextInstruction(actors[actorNumber].actorScript, currentLine, 0) - 1;
				break;
			}
		case 16: { // ENDIF
				break;
			}
		case 17: { // BODY
				int bodyIdx;

				verify(sscanf(tempPtr, "BODY %d", &bodyIdx) == 1);

				InitBody(bodyIdx, actorNumber);
				break;
			}
		case 18: { // BODY_OBJ
				int objectIdx;
				int bodyIdx;

				verify(sscanf(tempPtr, "BODY_OBJ %d %d", &objectIdx, &bodyIdx) == 2);

				InitBody(bodyIdx, objectIdx);
				break;
			}
		case 19: { // ANIM
				int animIdx;

				verify(sscanf(tempPtr, "ANIM %d", &animIdx) == 1);

				InitAnim(animIdx, 0, 0, actorNumber);
				break;
			}
		case 20: { // ANIM_OBJ
				int animIdx;
				int actorIdx;

				verify(sscanf(tempPtr, "ANIM_OBJ %d %d", &actorIdx, &animIdx) == 2);

				InitAnim(animIdx, 0, 0, actorIdx);
				break;
			}
			//case 21: // SET_LIFE
			//case 22: // SET_LIFE_OBJ
		case 23: { // SET_TRACK
				int trackPosition;

				verify(sscanf(tempPtr, "SET_TRACK %d", &trackPosition) == 1);
				currentScriptActor->positionInMoveScript = resolveTrackLineFromLabel(currentScriptActor, trackPosition);
				break;
			}
		case 24: { // SET_TRACK_OBJ
				int objectIdx;
				int trackPosition;

				verify(sscanf(tempPtr, "SET_TRACK_OBJ %d %d", &objectIdx, &trackPosition) == 2);
				actors[objectIdx].positionInMoveScript = resolveTrackLineFromLabel(&actors[objectIdx], trackPosition);
				break;
			}
		case 25: { // MESSAGE
				int messageIdx;

				verify(sscanf(tempPtr, "MESSAGE %d", &messageIdx) == 1);

				freezeTime();
				TestRestoreModeSVGA(1);
				//                             if(showTalkIcon)
				//                             DrawBulle(actorNumber);
				TestCoulDial(currentScriptActor->talkColor);	// CoulFont
				talkingActor = actorNumber;
				printTextFullScreen(messageIdx);
				unfreezeTime();
				fullRedraw(1);
				//                             waitForKey();
				break;
			}
		case 26: { // FALLABLE
				int fallableParam;

				verify(sscanf(tempPtr, "FALLABLE %d", &fallableParam) == 1);

				currentScriptActor->staticFlagsBF.bIsFallable = fallableParam;
				break;
			}
		case 27: { // SET_DIR
				int newDir;

				verify(sscanf(tempPtr, "SET_DIR %d", &newDir) == 1);

				currentScriptActor->comportement = newDir;
				if (newDir == 2) {
					int followActor;

					verify(sscanf(tempPtr, "SET_DIR %d follow %d", &newDir, &followActor) == 2);

					currentScriptActor->followedActor = followActor;
				}
				break;
			}
		case 28: {	// SET_DIR_OBJ
				int objIdx;
				int newDir;

				verify(sscanf(tempPtr, "SET_DIR_OBJ %d %d", &objIdx, &newDir) == 2);

				actors[objIdx].comportement = newDir;
				if (newDir == 2) {
					int followActor;

					verify(sscanf(tempPtr, "SET_DIR_OBJ %d %d follow %d", &objIdx, &newDir, &followActor) == 3);

					actors[objIdx].followedActor = followActor;
				}
				break;
			}
		case 29: {	// CAM_FOLLOW
				int newActorToFollow;

				verify(sscanf(tempPtr, "CAM_FOLLOW %d", &newActorToFollow) == 1);

				if (currentlyFollowedActor != newActorToFollow) {
					newCameraX = (actors[newActorToFollow].X >> 9);
					newCameraZ = (actors[newActorToFollow].Y >> 8);
					newCameraY = (actors[newActorToFollow].Z >> 9);

					currentlyFollowedActor = newActorToFollow;
					requestBackgroundRedraw = 1;
				}
				break;
			}
		case 30: { // COMPORTEMENT_HERO
				int comportementIdx;

				verify(sscanf(tempPtr, "COMPORTEMENT_HERO %d", &comportementIdx) == 1);

				InitAnim(ANIM_static, 0, 255, 0);
				SetComportement(comportementIdx);

				break;
			}
		case 31: { // SET_FLAG_CUBE
				int var1;
				int var2;

				verify(sscanf(tempPtr, "SET_FLAG_CUBE %d %d", &var1, &var2) == 2);

				cubeFlags[var1] = var2;
				break;
			}
		case 32: { // COMPORTEMENT
				break;
			}
		case 33: { // SET_COMPORTEMENT
				int comportementIdx;

				verify(sscanf(tempPtr, "SET_COMPORTEMENT %d", &comportementIdx) == 1);
				currentScriptActor->positionInActorScript = resolveComportementLineFromIdx(actors[actorNumber].actorScript, comportementIdx);
				break;
			}
		case 34: { // SET_COMPORTEMENT_OBJ
				int comportementIdx;
				int objIdx;

				verify(sscanf(tempPtr, "SET_COMPORTEMENT_OBJ %d %d", &objIdx, &comportementIdx) == 2);
				actors[objIdx].positionInActorScript = resolveComportementLineFromIdx(actors[objIdx].actorScript, comportementIdx);
				break;
			}
		case 35: { // END_COMPORTEMENT
				breakLife = 1;
				break;
			}
		case 36: { // SET_FLAG_GAME
				int var1;
				int var2;

				verify(sscanf(tempPtr, "SET_FLAG_GAME %d %d", &var1, &var2) == 2);

				vars[var1] = var2;
				break;
			}
		case 37: { // KILL_OBJ
				int objIdx;

				verify(sscanf(tempPtr, "KILL_OBJ", &objIdx) == 1);

				CheckCarrier(objIdx);

				actors[objIdx].dynamicFlagsBF.bUnk0020 = 1;
				actors[objIdx].costumeIndex = -1;
				actors[objIdx].zone = -1;
				actors[objIdx].life = 0;

				break;
			}
		case 38: { // SUICIDE
				CheckCarrier(actorNumber);
				currentScriptActor->dynamicFlagsBF.bUnk0020 = 1;
				currentScriptActor->costumeIndex = -1;
				currentScriptActor->zone = -1;
				currentScriptActor->life = 0;
				break;
			}
		case 39: { // USE_ONE_LITTLE_KEY
				numKey--;

				if (numKey < 0)
					numKey = 0;

				addOverlayObject(0, 6, 0, 0, 0, 1, 1);
				break;
			}
		case 40: { // GIVE_GOLD_PIECES
				int numGoldPieces;
				int eax;
				int ecx;
				overlayObjectListStruct *edi;
				int oldNumCoin;

				verify(sscanf(tempPtr, "GIVE_GOLD_PIECES %d", &numGoldPieces) == 1);

				oldNumCoin = numCoin;
				numCoin -= numGoldPieces;
				if (numCoin < 0)
					numCoin = 0;

				addOverlayObject(0, 3, 10, 30, 0, 0, 3);

				eax = 0;
				ecx = 0;
				edi = overlayObjectList;

				while (eax < 10) {
					if (edi->field_0 != -1 && edi->type == 2) {
						edi->field_0 = BoundRegleTrois(edi->followedActor, edi->field_0, 100, edi->timeToDie - lba_time - 50);
						edi->followedActor = numGoldPieces;
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
		case 41: { // END_LIFE
				breakLife = -1;
				currentScriptActor->positionInActorScript = -1;
				break;
			}
		case 42: { // STOP_L_TRACK
				currentScriptActor->pausedTrackPtr = currentScriptActor->currentLabelPtr;
				currentScriptActor->positionInMoveScript = -1;

				break;
			}
		case 43: { // RESTORE_L_TRACK
				currentScriptActor->positionInMoveScript = currentScriptActor->pausedTrackPtr;
				break;
			}
		case 45: { // INC_CHAPTER
				chapter++;
				break;
			}
		case 47: { // SET_DOOR_LEFT
				int moveFactor;

				verify(sscanf(tempPtr, "SET_DOOR_LEFT %d", &moveFactor) == 1);

				currentScriptActor->angle = 0x300;
				currentScriptActor->X = currentScriptActor->lastX - moveFactor;
				currentScriptActor->dynamicFlagsBF.bIsMoving = 0;
				currentScriptActor->speed = 0;
				break;
			}
		case 48: { // SET_DOOR_RIGHT
				int moveFactor;

				verify(sscanf(tempPtr, "SET_DOOR_RIGHT %d", &moveFactor) == 1);

				currentScriptActor->angle = 0x100;
				currentScriptActor->X = currentScriptActor->lastX + moveFactor;
				currentScriptActor->dynamicFlagsBF.bIsMoving = 0;
				currentScriptActor->speed = 0;
				break;
			}
		case 49: { // SET_DOOR_UP
				int moveFactor;

				verify(sscanf(tempPtr, "SET_DOOR_UP %d", &moveFactor) == 1);

				currentScriptActor->angle = 0x200;
				currentScriptActor->Z = currentScriptActor->lastY - moveFactor;
				currentScriptActor->dynamicFlagsBF.bIsMoving = 0;
				currentScriptActor->speed = 0;
				break;
			}
		case 50: { // SET_DOOR_DOWN
				int moveFactor;

				verify(sscanf(tempPtr, "SET_DOOR_DOWN %d", &moveFactor) == 1);

				currentScriptActor->angle = 0;
				currentScriptActor->Z = currentScriptActor->lastY + moveFactor;
				currentScriptActor->dynamicFlagsBF.bIsMoving = 0;
				currentScriptActor->speed = 0;
				break;
			}
		case 53: { // OBJ_COL
				int colSwitch;

				verify(sscanf(tempPtr, "OBJ_COL %d", &colSwitch) == 1);

				if (colSwitch != 0) {
					currentScriptActor->staticFlagsBF.bComputeCollisionWithObj = 1;
				} else {
					currentScriptActor->staticFlagsBF.bComputeCollisionWithObj = 0;
				}
				break;
			}
		case 55: { // OR_IF
				char* temp;
				int result = resolveCondition(tempPtr + 6);

				if (result)
					currentLine = resolveIfNextInstruction(actors[actorNumber].actorScript, currentLine, result) - 1;

				break;
			}
		case 56: {	// INVISIBLE
				int invisibleSwitch;

				verify(sscanf(tempPtr, "INVISIBLE %d", &invisibleSwitch) == 1);

				currentScriptActor->staticFlagsBF.bNoDisplay = invisibleSwitch;
				break;
			}
		case 57: { // ZOOM
				//TODO!
				break;
			}
		case 58: { // POS_POINT
				int flagIdx;

				verify(sscanf(tempPtr, "POS_POINT %d", &flagIdx) == 1);

				currentScriptActor->X = flagData[flagIdx].x;
				currentScriptActor->Y = flagData[flagIdx].y;
				currentScriptActor->Z = flagData[flagIdx].z;

				break;
			}
		case 61: { // SET_LIFE_POINT_OBJ
				int actorIdx;
				int newLifePoint;

				verify(sscanf(tempPtr, "SET_LIFE_POINT_OBJ %d %d", &actorIdx, &newLifePoint) == 2);

				actors[actorIdx].life = newLifePoint;

				break;
			}
		case 71: { // INIT_PINGOUIN
				int actorIdx;

				verify(sscanf(tempPtr, "INIT_PINGOUIN %d", &actorIdx) == 1);

				actors[actorIdx].dynamicFlagsBF.bUnk0020 = 1;
				currentPingouin = actorIdx;
				actors[actorIdx].costumeIndex = -1;
				actors[actorIdx].zone = -1;

				break;
			}
		case 77: { //LM_SAY_MESSAGE
				int messageNumber;

				verify(sscanf(tempPtr, "SAY_MESSAGE %d", &messageNumber) == 1);

				addOverlayObject(4, messageNumber, 0, 0, actorNumber, 1, 2);

				freezeTime();
				setVoxFileAtDigit(messageNumber);
				unfreezeTime();

				break;
			}
		default: {
				exit(1);
			}
		}

		currentLine ++;
	}
}
#endif
