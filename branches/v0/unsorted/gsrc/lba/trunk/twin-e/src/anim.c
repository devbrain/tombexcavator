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

struct bodyHeaderStruct {
	short int bodyFlag;     // 2
	short int unk0;         // 2
	short int unk1;         // 2
	short int unk2;         // 2
	short int unk3;         // 2
	short int unk4;         // 2
	short int unk5;         // 2
	short int offsetToData; // 2
	char *ptrToKeyFrame;
	int keyFrameTime;
};

typedef struct bodyHeaderStruct bodyHeaderStruct;

int setAnimAtKeyFrame(int keyframeIdx, unsigned char *anim, unsigned char *body, animTimerDataStruct* animTimerDataPtr) {
	short int numOfKeyframeInAnim;
	short int numOfBonesInAnim;
	char *ptrToData;
	char *ptrToDataBackup;
	char *ptrToBodyData;
	short int bodyHeader;
	short int numOfElementInBody;
	short int numOfPointInBody;
	int i;

	numOfKeyframeInAnim = READ_LE_S16(anim);

	if (keyframeIdx >= numOfKeyframeInAnim)
		return (numOfKeyframeInAnim);

	numOfBonesInAnim = READ_LE_S16(anim + 2);

	ptrToData = (char *)((numOfBonesInAnim * 8 + 8) * keyframeIdx + anim + 8);

	bodyHeader = READ_LE_S16(body);

	if (!(bodyHeader & 2))
		return (0);

	ptrToBodyData = (char *)(body + 14);

	animTimerDataPtr->ptr = ptrToData;
	animTimerDataPtr->time = lba_time;

	ptrToBodyData = ptrToBodyData + READ_LE_S16(ptrToBodyData) + 2; // Skip ?

	numOfElementInBody = READ_LE_S16(ptrToBodyData);

	ptrToBodyData = ptrToBodyData + numOfElementInBody * 6 + 12; // Skip

	numOfPointInBody = READ_LE_S16(ptrToBodyData - 10); // num bones

	if (numOfBonesInAnim > numOfPointInBody) {
		numOfBonesInAnim = numOfPointInBody;
	}

	ptrToDataBackup = ptrToData;

	ptrToData += 8;

	do {
		for (i = 0; i < 8; i++) {
			*(ptrToBodyData++) = *(ptrToData++);
		}

		ptrToBodyData += 30;

	} while (--numOfBonesInAnim);

	ptrToData = ptrToDataBackup + 2;

	currentX = READ_LE_S16(ptrToData);
	currentZ = READ_LE_S16(ptrToData + 2);
	currentY = READ_LE_S16(ptrToData + 4);

	processActorVar5 = READ_LE_S16(ptrToData + 6);
	processActorSub2Var0 = READ_LE_S16(ptrToData + 8);
	processActorVar6 = READ_LE_S16(ptrToData + 10);
	processActorSub2Var1 = READ_LE_S16(ptrToData + 12);

	return (1);
}

int GetNbFramesAnim(char *ptr) {
	return (READ_LE_S16(ptr));
}

int GetBouclageAnim(char *ptr) {
	return (READ_LE_S16(ptr + 4));
}

int SetInterAnimObjet2(int animState, char *animData, char *body, animTimerDataStruct* animTimerDataPtr) {
	short int animOpcode;

	short int var0;

	char *edi;
	char *ebx;
	int ebp;
	int eax;
	int keyFrameLength;
	int numOfPointInBody;
	int numOfPointInAnim;
	char *keyFramePtrOld;

	numOfPointInAnim = READ_LE_U16(animData + 2);

	keyFramePtr = ((numOfPointInAnim * 8 + 8) * animState) + animData + 8;

	keyFrameLength = READ_LE_U16(keyFramePtr);

	var0 = READ_LE_U16(body);

	if (!(var0 & 2)) {
		return (0);
	}

	edi = body + 16;

	animVar1 = edi;

	ebx = animTimerDataPtr->ptr;
	ebp = animTimerDataPtr->time;

	if (!ebx) {
		ebx = keyFramePtr;
		ebp = keyFrameLength;
	} else {
		assert_ptr(ebx);
	}

	lastKeyFramePtr = ebx;

	eax = READ_LE_S16(edi - 2);
	edi += eax;

	eax = READ_LE_S16(edi);
	eax = eax + eax * 2;
	edi = edi + eax * 2 + 12;

	numOfPointInBody = READ_LE_S16(edi - 10);

	if (numOfPointInAnim > numOfPointInBody) {
		numOfPointInAnim = numOfPointInBody;
	}

	eax = lba_time - ebp;

	if (eax >= keyFrameLength) {
		int *destPtr; // keyFrame
		int *sourcePtr;

		sourcePtr = (int *)(keyFramePtr + 8);
		destPtr = (int *) edi;

		do {
			WRITE_LE_U32(destPtr++, READ_LE_U32(sourcePtr++));
			WRITE_LE_U32(destPtr++, READ_LE_U32(sourcePtr++));

			destPtr = (int *)(((char *) destPtr) + 30);
		} while (--numOfPointInAnim);

		animTimerDataPtr->ptr = keyFramePtr;
		animTimerDataPtr->time = lba_time;

		currentX = READ_LE_S16(keyFramePtr + 2);
		currentZ = READ_LE_S16(keyFramePtr + 4);
		currentY = READ_LE_S16(keyFramePtr + 6);

		processActorVar5 = READ_LE_S16(keyFramePtr + 8);
		processActorSub2Var0 = READ_LE_S16(keyFramePtr + 10);
		processActorVar6 = READ_LE_S16(keyFramePtr + 12);
		processActorSub2Var1 = READ_LE_S16(keyFramePtr + 14);

		return (1);
	} else {
		keyFramePtrOld = keyFramePtr;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		processActorVar5 = READ_LE_S16(keyFramePtr);
		processActorSub2Var0 = (READ_LE_S16(keyFramePtr + 2) * eax) / keyFrameLength;
		processActorVar6 = (READ_LE_S16(keyFramePtr + 4) * eax) / keyFrameLength;
		processActorSub2Var1 = (READ_LE_S16(keyFramePtr + 6) * eax) / keyFrameLength;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		edi += 38;

		if (--numOfPointInAnim) {
			animVar4 = numOfPointInAnim;

			do {
				animOpcode = PatchType(&edi);

				switch (animOpcode) {
				case 0: {	// allow global rotate
						PatchInterAngle(&edi, eax, keyFrameLength);
						PatchInterAngle(&edi, eax, keyFrameLength);
						PatchInterAngle(&edi, eax, keyFrameLength);
						break;
					}
				case 1: {	// dissallow global rotate
						PatchInterStep(&edi, eax, keyFrameLength);
						PatchInterStep(&edi, eax, keyFrameLength);
						PatchInterStep(&edi, eax, keyFrameLength);
						break;
					}
				case 2: {	// dissallow global rotate + hide
						PatchInterStep(&edi, eax, keyFrameLength);
						PatchInterStep(&edi, eax, keyFrameLength);
						PatchInterStep(&edi, eax, keyFrameLength);
						break;
					}
				default: {
						printf("Unsupported rotaton mode %d in SetInterAnimObjet!\n", animOpcode);
						exit(1);
					}
				}

				edi += 30;
			} while (--animVar4);
		}

		currentX = (READ_LE_S16(keyFramePtrOld + 2) * eax) / keyFrameLength;
		currentZ = (READ_LE_S16(keyFramePtrOld + 4) * eax) / keyFrameLength;
		currentY = (READ_LE_S16(keyFramePtrOld + 6) * eax) / keyFrameLength;
	}

	return (0);
}

void loadGfxSub(unsigned char *bodyPtr) {
	bodyHeaderStruct *bodyHeader;
	short int offsetToData;
	unsigned char *bodyDataPtr;
	short int numOfElement1;
	short int numOfPoint;
	unsigned char *ptrToKeyData;
	int i;
	int bp = 36;
	int bx = 38;
	unsigned char *ptr2;

	bodyHeader = (bodyHeaderStruct *) bodyPtr;

	if (!(bodyHeader->bodyFlag & 2)) {	// no animation applicable
		return;
	}

	offsetToData = bodyHeader->offsetToData;

	bodyDataPtr = bodyPtr + offsetToData + 16;

	numOfElement1 = READ_LE_S16(bodyDataPtr);
	ptr2 = bodyDataPtr + 2 + numOfElement1 * 6;

	numOfPoint = READ_LE_S16(ptr2);

	ptrToKeyData = ptr2 + 2;

	for (i = 0; i < numOfPoint; i++) {
		ptrToKeyData += 38;
		WRITE_LE_S16(ptrToKeyData + 6, (READ_LE_S16(ptrToKeyData + 6) * bp) / bx);
	}
}

int SetInterAnimObjet(int animState, char *animData, char *body, animTimerDataStruct* animTimerDataPtr) {
	short int animOpcode;

	short int var0;

	char *edi;
	char *ebx;
	int ebp;
	int eax;
	int keyFrameLength;
	int numOfPointInBody;
	int numOfPointInAnim;
	char *keyFramePtrOld;

	numOfPointInAnim = READ_LE_S16(animData + 2);

	keyFramePtr = ((numOfPointInAnim * 8 + 8) * animState) + animData + 8;

	keyFrameLength = READ_LE_S16(keyFramePtr);

	var0 = READ_LE_S16(body);

	if (!(var0 & 2)) {
		return (0);
	}

	edi = body + 16;

	animVar1 = edi;

	ebx = animTimerDataPtr->ptr;
	ebp = animTimerDataPtr->time;

	if (!ebx) {
		ebx = keyFramePtr;
		ebp = keyFrameLength;
	}

	lastKeyFramePtr = ebx;

	eax = READ_LE_S16(edi - 2);
	edi += eax;

	eax = READ_LE_S16(edi);
	eax = eax + eax * 2;
	edi = edi + eax * 2 + 12;

	numOfPointInBody = READ_LE_S16(edi - 10);

	if (numOfPointInAnim > numOfPointInBody) {
		numOfPointInAnim = numOfPointInBody;
	}

	eax = lba_time - ebp;

	if (eax >= keyFrameLength) {
		int *destPtr;	// keyFrame
		int *sourcePtr;

		sourcePtr = (int *)(keyFramePtr + 8);
		destPtr = (int *) edi;

		do {
			WRITE_LE_U32(destPtr++, READ_LE_U32(sourcePtr++));
			WRITE_LE_U32(destPtr++, READ_LE_U32(sourcePtr++));

			destPtr = (int *)(((char *) destPtr) + 30);
		} while (--numOfPointInAnim);

		animTimerDataPtr->ptr = keyFramePtr;
		animTimerDataPtr->time = lba_time;

		currentX = READ_LE_S16(keyFramePtr + 2);
		currentZ = READ_LE_S16(keyFramePtr + 4);
		currentY = READ_LE_S16(keyFramePtr + 6);

		processActorVar5 = READ_LE_S16(keyFramePtr + 8);
		processActorSub2Var0 = READ_LE_S16(keyFramePtr + 10);
		processActorVar6 = READ_LE_S16(keyFramePtr + 12);
		processActorSub2Var1 = READ_LE_S16(keyFramePtr + 14);

		return (1);
	} else {
		keyFramePtrOld = keyFramePtr;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		processActorVar5 = READ_LE_S16(keyFramePtr);
		processActorSub2Var0 = (READ_LE_S16(keyFramePtr + 2) * eax) / keyFrameLength;
		processActorVar6 = (READ_LE_S16(keyFramePtr + 4) * eax) / keyFrameLength;
		processActorSub2Var1 = (READ_LE_S16(keyFramePtr + 6) * eax) / keyFrameLength;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		edi += 38;

		if (--numOfPointInAnim) {
			animVar4 = numOfPointInAnim;

			do {
				animOpcode = PatchType(&edi);

				assert(animOpcode >= 0 && animOpcode <= 2);

				switch (animOpcode) {
				case 0: {	// anim angle
						PatchInterAngle(&edi, eax, keyFrameLength);
						PatchInterAngle(&edi, eax, keyFrameLength);
						PatchInterAngle(&edi, eax, keyFrameLength);
						break;
					}
				case 1: {	// anim step
						PatchInterStep(&edi, eax, keyFrameLength);
						PatchInterStep(&edi, eax, keyFrameLength);
						PatchInterStep(&edi, eax, keyFrameLength);
						break;
					}
				case 2: {	// anim step
						PatchInterStep(&edi, eax, keyFrameLength);
						PatchInterStep(&edi, eax, keyFrameLength);
						PatchInterStep(&edi, eax, keyFrameLength);
						break;
					}
				}
				edi += 30;
			} while (--animVar4);
		}

		currentX = (READ_LE_S16(keyFramePtrOld + 2) * eax) / keyFrameLength;
		currentZ = (READ_LE_S16(keyFramePtrOld + 4) * eax) / keyFrameLength;
		currentY = (READ_LE_S16(keyFramePtrOld + 6) * eax) / keyFrameLength;
	}

	return (0);
}

int InitAnim(char newAnim, short int arg_4, unsigned char arg_8, short int actorNum) {
	actor *lactor;
	int animIndex;

	lactor = &actors[actorNum];

	if (lactor->costumeIndex == -1)
		return (0);

	if (lactor->staticFlagsBF.bIsSpriteActor) // si c'est un sprite
		return (0);

	if (newAnim == lactor->anim && lactor->previousAnimIndex != -1) // le costume est deja loade
		return (1);

	if (arg_8 == 255 && lactor->field_78 != 2)
		arg_8 = lactor->anim;

	animIndex = getAnimIndexForBody(newAnim, actorNum);

	if (animIndex == -1)
		animIndex = getAnimIndexForBody(0, actorNum);

	if (arg_4 != 4 && lactor->field_78 == 2) {
		lactor->field_2 = newAnim;
		return (0);
	}

	if (arg_4 == 3) {
		arg_4 = 2;

		arg_8 = lactor->anim;

		if (arg_8 == 15 || arg_8 == 7 || arg_8 == 8 || arg_8 == 9) {
			arg_8 = 0;
		}
	}

	if (arg_4 == 4)
		arg_4 = 2;

	if (lactor->previousAnimIndex == -1) {	// if no previous animation
		setAnimAtKeyFrame(0, HQR_Get(HQR_Anims, animIndex), bodyPtrTab[lactor->costumeIndex], &lactor->animTimerData);	// set animation directly to first keyFrame
	} else { // interpolation between animations
		bufAni2 += StockInterAnim((char *) bufAni2, (char *) bodyPtrTab[lactor->costumeIndex], &lactor->animTimerData);
		if (bufAni1 + 4488 > bufAni2)
			bufAni2 = bufAni1;
	}

	lactor->previousAnimIndex = animIndex;
	lactor->anim = newAnim;
	lactor->field_2 = arg_8;
	lactor->animExtraData = loadTwinsenCostumesVar1;
	lactor->field_78 = arg_4;
	lactor->animPosition = 0;
	lactor->dynamicFlagsBF.bUnk0002 = 0;
	lactor->dynamicFlagsBF.bUnk0004 = 0;
	lactor->dynamicFlagsBF.bUnk0008 = 1;

	if (lactor->animExtraData) {
		GereAnimAction(lactor, actorNum);
	}

	lactor->lastRotationSpeed = 0;
	lactor->lastX = 0;
	lactor->lastY = 0;
	lactor->lastZ = 0;

	return (1);
}

int StockInterAnim(char *lBufAnim, char *lBody, animTimerDataStruct* animTimerDataPtr) {	// copy the next keyFrame from a different buffer
	int temp;
	char *ptr;
	int *edi;
	int *esi;
	int var0;
	int var1;
	int var2;
	int counter;

	assert_ptr(lBufAnim);
	assert_ptr(lBody);

	temp = READ_LE_S16(lBody);

	if (temp & 2) {
		ptr = (lBody + 0x10);

		animTimerDataPtr->time = lba_time;
		animTimerDataPtr->ptr = lBufAnim;

		var0 = READ_LE_S16(ptr - 2);
		ptr = ptr + var0;

		var1 = READ_LE_S16(ptr);
		var1 = var1 + var1 * 2;

		ptr = ptr + var1 * 2 + 2;

		var2 = READ_LE_S16(ptr);
		counter = var2;
		var2 = (var2 * 8) + 8;

		edi = (int *)(lBufAnim + 8);
		esi = (int *)(ptr + 10);

		do {
			WRITE_LE_U32(edi++, READ_LE_U32(esi++));
			WRITE_LE_U32(edi++, READ_LE_U32(esi++));

			esi = (int *)(((char *) esi) + 30);
		} while (--counter);

		return (var2);
	}
	return (0);
}

int PatchType(char **ptr) {
	short int *lptr;
	short int opcode;

	lptr = (short int *) * ptr;

	opcode = READ_LE_S16(keyFramePtr);
	WRITE_LE_S16(lptr, opcode);

	keyFramePtr += 2;
	*(ptr) = *(ptr) + 2;
	lastKeyFramePtr += 2;

	return (opcode);
}

void PatchInterAngle(char **ptr, int bp, int bx) {
	short int *dest;
	short int lastAngle;
	short int newAngle;
	short int angleDif;
	short int computedAngle;

	lastAngle = READ_LE_S16(lastKeyFramePtr);
	lastKeyFramePtr += 2;

	newAngle = READ_LE_S16(keyFramePtr);
	keyFramePtr += 2;

	lastAngle &= 0x3FF;
	newAngle &= 0x3FF;

	angleDif = newAngle - lastAngle;

	if (angleDif) {
		if (angleDif < -0x200) {
			angleDif += 0x400;
		} else if (angleDif > 0x200) {
			angleDif -= 0x400;
		}

		computedAngle = lastAngle + (angleDif * bp) / bx;
	} else {
		computedAngle = lastAngle;
	}

	dest = (short int *) * (ptr);

	WRITE_LE_S16(dest, computedAngle & 0x3FF);

	*(ptr) = *(ptr) + 2;
}

void PatchInterStep(char **ptr, int bp, int bx) {
	short int *dest;
	short int lastAngle;
	short int newAngle;
	short int angleDif;
	short int computedAngle;

	lastAngle = *(short int *) lastKeyFramePtr;
	lastKeyFramePtr += 2;

	newAngle = *(short int *) keyFramePtr;
	keyFramePtr += 2;

	angleDif = newAngle - lastAngle;

	if (angleDif) {
		computedAngle = lastAngle + (angleDif * bp) / bx;
	} else {
		computedAngle = lastAngle;
	}

	dest = (short int *) * (ptr);

	*dest = computedAngle;

	*(ptr) = *(ptr) + 2;
}

int SetInterDepObjet(int position, char *anim, char *body, animTimerDataStruct* animTimerDataPtr) {
	short int bodyFlags;
	char *edi;
	char *ebx;
	int ebp;
	int eax;
	int keyFrameLength;

// int numOfPointInBody;
	int numOfPointInAnim;
	char *keyFramePtrOld;

	numOfPointInAnim = READ_LE_S16(anim + 2);

	keyFramePtr = (numOfPointInAnim * 8 + 8) * position + anim + 8;

	keyFrameLength = READ_LE_S16(keyFramePtr);

	bodyFlags = READ_LE_S16(body);

	if (bodyFlags & 2) {
		edi = body + 16;

		animVar1 = edi;

		ebx = animTimerDataPtr->ptr;
		ebp = animTimerDataPtr->time;

		if (!ebx) {
			ebx = keyFramePtr;
			ebp = keyFrameLength;
		}

		lastKeyFramePtr = ebx;

		eax = lba_time - ebp;

		//printf("delta=%d / %d  -> time=%d -> ebp=%d\n", eax, keyFrameLength, time, ebp);

		if (eax >= keyFrameLength) {
			animTimerDataPtr->ptr = keyFramePtr;
			animTimerDataPtr->time = lba_time;

			currentX = READ_LE_S16(keyFramePtr + 2);
			currentZ = READ_LE_S16(keyFramePtr + 4);
			currentY = READ_LE_S16(keyFramePtr + 6);

			processActorVar5 = READ_LE_S16(keyFramePtr + 8);
			processActorSub2Var0 = READ_LE_S16(keyFramePtr + 10);
			processActorVar6 = READ_LE_S16(keyFramePtr + 12);
			processActorSub2Var1 = READ_LE_S16(keyFramePtr + 14);

			return (1);
		} else {
			keyFramePtrOld = keyFramePtr;

			lastKeyFramePtr += 8;
			keyFramePtr += 8;

			processActorVar5 = READ_LE_S16(keyFramePtr);
			processActorSub2Var0 = (READ_LE_S16(keyFramePtr + 2) * eax) / keyFrameLength;
			processActorVar6 = (READ_LE_S16(keyFramePtr + 4) * eax) / keyFrameLength;
			processActorSub2Var1 = (READ_LE_S16(keyFramePtr + 6) * eax) / keyFrameLength;

			lastKeyFramePtr += 8;
			keyFramePtr += 8;

			currentX = (READ_LE_S16(keyFramePtrOld + 2) * eax) / keyFrameLength;
			currentZ = (READ_LE_S16(keyFramePtrOld + 4) * eax) / keyFrameLength;
			currentY = (READ_LE_S16(keyFramePtrOld + 6) * eax) / keyFrameLength;

			return (0);
		}
	}

	return (0);
}
