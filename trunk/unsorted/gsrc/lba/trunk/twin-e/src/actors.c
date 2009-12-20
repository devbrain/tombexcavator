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

void StartInitObj(short int arg_0) {
	actor *lactor;

	lactor = &actors[arg_0];

	if (lactor->staticFlagsBF.bIsSpriteActor) { // if sprite actor
		if (lactor->field_66 != 0) {
			lactor->dynamicFlagsBF.bUnk0002 = 1;
		}

		lactor->costumeIndex = -1;

		InitSprite(lactor->field_8, arg_0);

		setActorAngleSafe(0, 0, 0, &lactor->time);

		if (lactor->staticFlagsBF.bIsUsingClipping) {
			lactor->lastX = lactor->X;
			lactor->lastZ = lactor->Y;
			lactor->lastY = lactor->Z;
		}

	} else {
		lactor->costumeIndex = -1;

		InitBody(lactor->body, arg_0);

		lactor->previousAnimIndex = -1;
		lactor->field_78 = 0;

		if (lactor->costumeIndex != -1) {
			InitAnim(lactor->anim, 0, 255, arg_0);
		}

		setActorAngleSafe(lactor->angle, lactor->angle, 0, &lactor->time);
	}

	lactor->positionInMoveScript = -1;
	lactor->label = -1;
	lactor->positionInActorScript = 0;
}

void resetActor(int actorNumber) {
	actor *localActor;

	localActor = &actors[actorNumber];

	localActor->body = 0;
	localActor->anim = 0;
	localActor->X = 0;
	localActor->Y = -1;
	localActor->Z = 0;

	localActor->boudingBox.X.bottomLeft = 0;
	localActor->boudingBox.X.topRight = 0;
	localActor->boudingBox.Y.bottomLeft = 0;
	localActor->boudingBox.Y.topRight = 0;
	localActor->boudingBox.Z.bottomLeft = 0;
	localActor->boudingBox.Z.topRight = 0;

	localActor->angle = 0;
	localActor->speed = 40;
	localActor->comportement = 0;

	localActor->cropLeft = 0;
	localActor->cropTop = 0;
	localActor->cropRight = 0;
	localActor->cropBottom = 0;

	localActor->field_3 = 0;
	localActor->collision = -1;
	localActor->standOn = -1;
	localActor->zone = -1;
	memset(&localActor->staticFlagsBF, 0, 2);
	memset(&localActor->dynamicFlagsBF, 0, 2);
	localActor->life = 50;
	localActor->field_14 = 1;
	localActor->hitBy = -1;
	localActor->lastRotationSpeed = 0;
	localActor->lastX = 0;
	localActor->lastZ = 0;
	localActor->lastY = 0;
	localActor->costumeIndex = -1;
	localActor->previousAnimIndex = -1;
	localActor->field_78 = 0;
	localActor->animPosition = 0;

	setActorAngleSafe(0, 0, 0, &localActor->time);

	localActor->positionInMoveScript = -1;
	localActor->positionInActorScript = 0;
}

//load spriteActorBoundingBox
void InitSprite(int imageNumber, int actorNumber) {
	actor *lactor = &actors[actorNumber];

	if (lactor->staticFlagsBF.bIsSpriteActor && imageNumber != -1 && lactor->costumeIndex != imageNumber) {
		short int *ptr;

		lactor->costumeIndex = imageNumber;

		ptr = (short int *)(spriteActorData + imageNumber * 16 + 4);

		lactor->boudingBox.X.bottomLeft = READ_LE_S16(ptr++);
		lactor->boudingBox.X.topRight = READ_LE_S16(ptr++);
		lactor->boudingBox.Y.bottomLeft = READ_LE_S16(ptr++);
		lactor->boudingBox.Y.topRight = READ_LE_S16(ptr++);
		lactor->boudingBox.Z.bottomLeft = READ_LE_S16(ptr++);
		lactor->boudingBox.Z.topRight = READ_LE_S16(ptr++);
	}
}

void CheckCarrier(int actorNumber) {
	S32 i;

	if (actors[actorNumber].staticFlagsBF.bIsCarrier) {
		for (i = 0;i < numActorInRoom;i++) {
			if (actors[i].standOn == actorNumber)
				actors[i].standOn = -1;
		}
	}
}
