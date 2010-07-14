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

struct MDL_BoundingBoxPoint {
	short int bottomLeft;
	short int topRight;
};

typedef struct MDL_BoundingBoxPoint MDL_BoundingBoxPoint;

struct MDL_BoundingBox {
	MDL_BoundingBoxPoint X;
	MDL_BoundingBoxPoint Y;
	MDL_BoundingBoxPoint Z;
};

typedef struct MDL_BoundingBox MDL_BoundingBox;

struct animTimerDataStruct {
	char* ptr;
	int time;
};

typedef struct animTimerDataStruct animTimerDataStruct;

struct actor {
#ifdef TXT_SCRIPTS
	char name[256];
	int modelNumber;
	int timeDelay;
	int faceTwinkenVar;
#endif
	int body;
	int anim;
	int field_2;
	int field_3;
	unsigned char *animExtraData;
	int field_8;
	int currentLabelPtr;
	byte *entityDataPtr;
	int field_10;
	int field_12;
	int field_14;
	int talkColor;
	int costumeIndex;
	int X;		/* \ */
	int Y;		/* |----coordiantes */
	int Z;		/* / */
	int field_20;
	int field_22;
	int field_24;

	MDL_BoundingBox boudingBox;

	int angle;	// rotation ?
	int speed;
	timeStruct time;
	int comportement;
	int positionInMoveScript;
	int positionInActorScript;

	int cropLeft;
	int cropTop;
	int cropRight;
	int cropBottom;

	int followedActor;

	int collision;
	int standOn;
	int zone;
	int label;
	int pausedTrackPtr;

	struct {
		U16 bComputeCollisionWithObj : 1; // 0x0001
		U16 bComputeCollisionWithBricks : 1; // 0x0002
		U16 bIsZonable : 1; // 0x0004
		U16 bIsUsingClipping : 1; // 0x0008
		U16 bIsPushable : 1; // 0x0010
		U16 bIsDead : 1; // 0x0020
		U16 bCanDrown : 1; // 0x0040
		U16 bUnk80 : 1; // 0x0080
		U16 bUnk0100 : 1; // 0x0100
		U16 bNoDisplay : 1; // 0x0200
		U16 bIsSpriteActor : 1; // 0x0400
		U16 bIsFallable : 1; // 0x0800
		U16 bDoesntCastShadow : 1; // 0x1000
		U16 bIsBackgrounded : 1; // 0x2000
		U16 bIsCarrier : 1; // 0x4000
		U16 bIsUsingMiniZv : 1; // 0x8000
	} staticFlagsBF;

	struct {
		U16 bUnk0001 : 1; // 0x0001
		U16 bUnk0002 : 1; // 0x0002
		U16 bUnk0004 : 1; // 0x0004
		U16 bUnk0008 : 1; // 0x0008
		U16 wasDrawn : 1; // 0x0010
		U16 bUnk0020 : 1; // 0x0020
		U16 bIsMoving : 1; // 0x0040
		U16 bUnk0080 : 1; // 0x0080
		U16 bUnk0100 : 1; // 0x0100
		U16 bUnk0200 : 1; // 0x0200
		U16 bUnk0400 : 1; // 0x0400
		U16 bUnk0800 : 1; // 0x0800
		U16 bUnk1000 : 1; // 0x1000
		U16 bUnk2000 : 1; // 0x2000
		U16 bUnk4000 : 1; // 0x4000
		U16 bUnk8000 : 1; // 0x8000
	} dynamicFlagsBF;

	int hitBy;
	int field_66;
	int life;
	int lastRotationSpeed;
	int lastX;
	int lastZ;
	int lastY;
	int previousAnimIndex;
	int doorStatus;
	int animPosition;	// animPosition
	int field_78;

	unsigned char *moveScript;
	unsigned char *actorScript;

	animTimerDataStruct animTimerData; // new
};

typedef struct actor actor;

