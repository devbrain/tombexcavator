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
#include "extraData.h"

void makeMagicBallBounce(extraListStruct* pExtra, int X, int Y, int Z) {
	if (WorldColBrick(X, pExtra->Z, Z)) {
		pExtra->field_10 = -pExtra->field_10;
	}

	if (WorldColBrick(pExtra->X, Y, Z)) {
		pExtra->field_E = -pExtra->field_E;
	}

	if (WorldColBrick(X, Y, pExtra->Y)) {
		pExtra->field_12 = -pExtra->field_12;
	}

	pExtra->X = X;
	pExtra->field_8 = X;

	pExtra->Z = Y;
	pExtra->field_A = Y;

	pExtra->Y = Z;
	pExtra->field_C = Z;

	pExtra->time = lba_time;
}

void reinitExtraObjectList(void) {
	int counter;

	for (counter = 0; counter < 50; counter++) {
		extraList[counter].field_0 = -1;
		extraList[counter].field_20 = 1;
	}
}

int ThrowExtra(int actorNum, int X, int Y, int Z, int sprite, int var2, int var3, int var4, int var5, int strength) {
	int i;

	for (i = 0;i < 50;i++) {
		if (extraList[i].field_0 == -1) {
			extraList[i].field_0 = sprite;
			extraList[i].field_14 = 0x210C;
			extraList[i].X = X;
			extraList[i].Z = Y;
			extraList[i].Y = Z;

			InitFly(&extraList[i], var2, var3, var4, var5);

			extraList[i].field_1E = strength;
			extraList[i].field_1C = actorNum;
			extraList[i].time = lba_time;
			extraList[i].field_20 = 0;

			return(i);
		}
	}

	return(-1);
}

void InitSpecial(int var0, int var1, int var2, int var3) {
	int i;
	short int flag;

	flag = 0x8000 + var3;

	for (i = 0;i < 50;i++) {
		if (extraList[i].field_0 == -1) {
			extraList[i].field_0 = flag;
			extraList[i].field_20 = 0;

			if (!var3) {
				extraList[i].field_14 = 9;

				extraList[i].X = var0;
				extraList[i].Z = var1;
				extraList[i].Y = var2;

				InitFly(&extraList[i], (rand() % 0x100) + 0x80, rand() % 0x400, 50, 20);

				extraList[i].field_1E = 0;
				extraList[i].time = lba_time;
				extraList[i].field_1C = 100;

				return;
			} else
				if (var3 == 1) {
					extraList[i].field_14 = var3;

					extraList[i].X = var0;
					extraList[i].Z = var1;
					extraList[i].Y = var2;

					extraList[i].field_1E = 0;
					extraList[i].time = lba_time;
					extraList[i].field_1C = 5;

					return;
				}
		}
	}
}

void InitFly(extraListStruct* extraEntry, int var1, int var2, int var3, int var4) {
	extraEntry->field_14 |= 2;

	extraEntry->field_8 = extraEntry->X;
	extraEntry->field_A = extraEntry->Z;
	extraEntry->field_C = extraEntry->Y;

	Rotate(var3, 0, var1);

	extraEntry->field_10 = -destZ;

	Rotate(0, destX, var2);

	extraEntry->field_E = destX;
	extraEntry->field_12 = destZ;

	extraEntry->field_16 = var4;
	extraEntry->time = lba_time;
}

void AffSpecial(int extraNum, int X, int Y) { //line object only
	extraListStruct* extraEntry = &extraList[extraNum];

	int extraType = extraEntry->field_0 & 0x7FFF;

	if (!extraType) { // damage star
		Aff2DShape(extraDataTable1, X, Y, 15, (lba_time << 5) & 0x300, 4);
		return;
	}

	if (extraType == 1) { // little cloud
		int cloudTime = 1 + lba_time - extraEntry->time;

		if (cloudTime > 32)
			cloudTime = 32;

		Aff2DShape(extraDataTable2, X, Y, 15, 0, cloudTime);

		return;
	}
}

int ExtraBonus(int X, int Y, int Z, int param, int angle, int type, int param2) {
	int i;

	for (i = 0;i < 50;i++) {
		if (extraList[i].field_0 == -1) {
			extraList[i].field_0 = type;
			extraList[i].field_14 = 0x4071; // old value: 0x4030

			// This cause an incorrect movement in the Key extra
			/*if(type != 6)
			{
			extraList[i].field_14 = 0x4071;
			}*/

			extraList[i].X = X;
			extraList[i].Z = Y;
			extraList[i].Y = Z;

			InitFly(&extraList[i], param, angle, 40, 15);

			extraList[i].field_1E = 0;

			extraList[i].time = lba_time;

			extraList[i].field_1C = 1000;
			extraList[i].field_20 = param2;

			return(i);
		}
	}

	return(-1);
}

void Aff2DShape(short int* extraData, int X, int Y, int param0, int time, int param1) {
	short int currentExtraData;
	short int var_8;
	short int temp1;
	int computedX;
	int computedY;
	int oldComputedX;
	int oldComputedY;
	int var_18;
	int currentX;
	int currentY;

	currentExtraData = *(extraData++);

	var_8 = ((*(extraData++)) * param1) >> 4;
	temp1 = ((*(extraData++)) * param1) >> 4;

	renderLeft = 0x7D00;
	renderRight = -0x7D00;
	renderTop = 0x7D00;
	renderBottom = -0x7D00;

	Rotate(var_8, temp1, time);

	computedX = destX + X;
	computedY = destZ + Y;

	if (computedX < renderLeft)
		renderLeft = computedX;

	if (computedX > renderRight)
		renderRight = computedX;

	if (computedY < renderTop)
		renderTop = computedY;

	if (computedY > renderBottom)
		renderBottom = computedY;

	var_18 = 1;

	currentX = computedX;
	currentY = computedY;

	while (var_18 < currentExtraData) {
		var_8 = ((*(extraData++)) * param1) >> 4;
		temp1 = ((*(extraData++)) * param1) >> 4;

		oldComputedX = currentX;
		oldComputedY = currentY;

		projectedPositionX = currentX;
		projectedPositionY = currentY;

		Rotate(var_8, temp1, time);

		currentX = destX + X;
		currentY = destZ + Y;

		if (currentX < renderLeft)
			renderLeft = currentX;

		if (currentX > renderRight)
			renderRight = currentX;

		if (currentY < renderTop)
			renderTop = currentY;

		if (currentY > renderBottom)
			renderBottom = currentY;

		projectedPositionX = currentX;
		projectedPositionY = currentY;

		Line(oldComputedX, oldComputedY, currentX, currentY, param0);

		var_18++;

		currentX = projectedPositionX;
		currentY = projectedPositionY;

	}

	projectedPositionX = currentX;
	projectedPositionY = currentY;
	Line(currentX, currentY, computedX, computedY, param0);
}

void GereExtras(void) {
	int i;

	int currentExtraX = 0;
	int currentExtraZ = 0;
	int currentExtraY = 0;
	int currentExtraSpeedX = 0;
	int currentExtraSpeedZ = 0;

	int var_40;
	int var_30;

	for (i = 0;i < 50;i++) {
		if (extraList[i].field_0 != -1) {
			if (extraList[i].field_14 & 1) { //remove after time
				if (extraList[i].field_1C + extraList[i].time <= lba_time) { // time passed
					extraList[i].field_0 = -1;
					continue;
				}
			}

			if (extraList[i].field_14 & 0x800) {
				extraList[i].field_0 = -1;
				continue;
			}

			if (extraList[i].field_14 & 0x1000) {
				extraList[i].field_0 = BoundRegleTrois(97, 100, 30, lba_time - extraList[i].time);
				continue;
			}

			if (extraList[i].field_14 & 0x2) { //is moving
				currentExtraX = extraList[i].X;
				currentExtraZ = extraList[i].Z;
				currentExtraY = extraList[i].Y;

				currentExtraSpeedX = extraList[i].field_E * (lba_time - extraList[i].time);
				extraList[i].X = currentExtraSpeedX + extraList[i].field_8;

				currentExtraSpeedZ = extraList[i].field_10 * (lba_time - extraList[i].time);
				currentExtraSpeedZ += extraList[i].field_A;
				extraList[i].Z = currentExtraSpeedZ - abs(((extraList[i].field_16 * (lba_time - extraList[i].time)) * (lba_time - extraList[i].time)) >> 4);

				extraList[i].Y = extraList[i].field_12 * (lba_time - extraList[i].time) + extraList[i].field_C;

				if (extraList[i].Z < 0 || extraList[i].X < 0 || extraList[i].X > 0x7E00 || extraList[i].Y < 0 || extraList[i].Y > 0x7E00) { // extra is out of cube
					if (i == magicBallIdx) { // if it's the magic ball
						int eax = 44;

						if (extraList[i].field_0 == 42)
							eax = 109;
						if (extraList[i].field_0 == 43)
							eax = 110;

						magicBallIdx = ExtraSearch(-1, extraList[i].X, extraList[i].Z, extraList[i].Y, eax, 0, 10000, 0);
					}

					if (extraList[i].field_14 & 0x20) {
						extraList[i].field_14 &= 0xFFED;
					} else {
						extraList[i].field_0 = -1;
					}

					continue;
				}
			}

			if (extraList[i].field_14 & 0x4000) {
				if (lba_time - extraList[i].time > 40) {
					extraList[i].field_14 &= 0xBFFF;
				}
				continue;
			}

			if (extraList[i].field_14 & 0x80) { // hit targeted actor
				int targetedActor;
				int targetedActor2;
				int var_1C;
				int angleToActor;
				int angleToActor2;

				targetedActor = extraList[i].time;
				var_1C = extraList[i].field_1C;

				targetedActor2 = targetedActor;

				currentExtraX = actors[targetedActor].X;
				currentExtraZ = actors[targetedActor].Y + 1000;
				currentExtraY = actors[targetedActor].Z;

				angleToActor = GetAngle(extraList[i].X, extraList[i].Y, currentExtraX, currentExtraY);

				angleToActor2 = (angleToActor - extraList[i].field_16) & 0x3FF;

				if (angleToActor2 > 400 && angleToActor2 < 600) {
					if (extraList[i].field_1E) {
						HitObj(var_1C, targetedActor2, extraList[i].field_1E, -1);
					}

					if (i == magicBallIdx)
						magicBallIdx = -1;

					extraList[i].field_0 = -1;
					continue;
				} else {
					int verticalAngle;
					int dx;

					verticalAngle = GetAngle(extraList[i].Z, 0, currentExtraZ, DoTrackVar1);

					dx = GetRealAngle(&extraList[i].trackActorRotation);

					if (!dx) {
						dx = 1;
					}

					Rotate(dx, 0, verticalAngle);

					extraList[i].Z -= destZ;

					Rotate(0, destX, angleToActor);

					extraList[i].X += destX;
					extraList[i].Y += destZ;

					setActorAngle(0, extraList[i].field_12, 50, &extraList[i].trackActorRotation);

					if (targetedActor == ExtraCheckObjCol(&extraList[i], var_1C)) {
						if (i == magicBallIdx)
							magicBallIdx = -1;

						extraList[i].field_0 = -1;
						continue;
					}

				}

			}

			if (extraList[i].field_14 & 0x200) { // magic ball aiming for key
			}

			if (extraList[i].field_14 & 0x4) { // extra hit actor
				if (ExtraCheckObjCol(&extraList[i], extraList[i].field_1C) != -1) {
					if (i == magicBallIdx) { // magic ball hit actor
						int magicBallSprite;

						magicBallSprite = 44;

						if (extraList[i].field_0 == 42)
							magicBallSprite = 109;

						if (extraList[i].field_0 == 43)
							magicBallSprite = 110;

						magicBallIdx = ExtraSearch(-1, extraList[i].X, extraList[i].Z, extraList[i].Y, magicBallSprite, 0, 10000, 0);
					}

					extraList[i].field_0 = -1;
					continue;
				}
			}

			if (extraList[i].field_14 & 0x8) { // extra handle collision with ground
				var_40 = 0;
				if (FullWorldColBrick(currentExtraX, currentExtraZ, currentExtraY, extraList[i].X, extraList[i].Z, extraList[i].Y)) {
					if (!(extraList[i].field_14 & 0x2000)) // already touched ground before ?
						var_40 = 1;
				} else {
					if (extraList[i].field_14 & 0x2000) // already touched ground before ?
						extraList[i].field_14 &= 0xDFFF; // then, reinit flag as it's not on the ground anymore
				}

				if (var_40) {
					if (extraList[i].field_14 & 0x100) { // bounce generate a little smoke while touching ground
						InitSpecial(currentExtraX, currentExtraZ, currentExtraY, 1);
					}

					if (i == magicBallIdx) { // if it's the magic ball
						HQ_3D_MixSample(86, (rand() % 300) + 3946, 1, extraList[i].X, extraList[i].Z, extraList[i].Y);

						if (magicBallNumBounce <= 0) { // No magic, the ball can't bounce at all
							int magicBallSprite;

							magicBallSprite = 44;

							if (extraList[i].field_0 == 42)
								magicBallSprite = 109;

							if (extraList[i].field_0 == 43)
								magicBallSprite = 110;

							magicBallIdx = ExtraSearch(-1, extraList[i].X, extraList[i].Z, extraList[i].Y, magicBallSprite, 0, 10000, 0);

							extraList[i].field_0 = -1;
							continue;
						}

						if (magicBallNumBounce == 1) { // there is a bit of magic
							if (!magicBallParam--) { // can the magic ball bounce more ?
								int magicBallSprite;

								magicBallSprite = 44;

								if (extraList[i].field_0 == 42)
									magicBallSprite = 109;

								if (extraList[i].field_0 == 43)
									magicBallSprite = 110;

								magicBallIdx = ExtraSearch(-1, extraList[i].X, extraList[i].Z, extraList[i].Y, magicBallSprite, 0, 10000, 0);

								extraList[i].field_0 = -1;
								continue;
							} else {
								makeMagicBallBounce(&extraList[i], currentExtraX, currentExtraZ, currentExtraY);
							}
						}
					} else {
						extraList[i].field_0 = -1;
						continue;
					}
				}
			}

			if (extraList[i].field_14 & 0x10) { // extra stop moving when collision with brick takes place
				var_30 = 0;
				if (FullWorldColBrick(currentExtraX, currentExtraZ, currentExtraY, extraList[i].X, extraList[i].Z, extraList[i].Y)) {
					if (!(extraList[i].field_14 & 0x2000)) // already touched ground before ?
						var_30 = 1;
				} else {
					if (extraList[i].field_14 & 0x2000) // already touched ground before ?
						extraList[i].field_14 &= 0xDFFF; // then, reinit flag as it's not on the ground anymore
				}

				if (var_30) {
					short int* ptr;

					ptr = (short int*)(spriteActorData + extraList[i].field_0 * 16 + 8);
					extraList[i].Z = (getPosVar2 << 8) + 0x100 - *(ptr);
					extraList[i].field_14 &= 0xFFED;
					continue;
				}
			}

			if ((extraList[i].field_14 & 0x20) && !(extraList[i].field_14 & 0x2)) { // can take extra on the floor
				if (ExtraCheckObjCol(&extraList[i], -1) == 0) { // if twisen touch the extra
					HQ_3D_MixSample(97, 0x1000, 1, extraList[i].X, extraList[i].Z, extraList[i].Y);

					if (extraList[i].field_20 > 1 && !((byte) mainLoopVar5 & 2)) {
						projectPositionOnScreen(extraList[i].X - cameraX, extraList[i].Z - cameraZ, extraList[i].Y - cameraY);
						addOverlayObject(1, extraList[i].field_20, projectedPositionX, projectedPositionY, 158, 0, 2);
					}

					addOverlayObject(0, extraList[i].field_0, 10, 30, 0, 0, 2);

					if (extraList[i].field_0 == 3) {
						numCoin += extraList[i].field_20;
						if (numCoin > 999)
							numCoin = 999;
					}

					if (extraList[i].field_0 == 4) {
						twinsen->life += extraList[i].field_20;
						if (twinsen->life > 50)
							twinsen->life = 50;
					}

					if (extraList[i].field_0 == 5 && magicLevel) {
						magicPoint += extraList[i].field_20 * 2;

						if (magicPoint > magicLevel * 20) {
							magicPoint = magicLevel * 20;
						}
					}

					if (extraList[i].field_0 == 6) {
						numKey += extraList[i].field_20;
					}

					if (extraList[i].field_0 == 7) {
						numClover += extraList[i].field_20;

						if (numClover > numCloverBox)
							numClover = numCloverBox;
					}

					extraList[i].field_0 = -1;
				}
			}
		}
	}
}

int ExtraCheckObjCol(extraListStruct* extra, int param) {
	int X1;
	int X2;
	int Z1;
	int Z2;
	int Y1;
	int Y2;

	int aX1;
	int aX2;
	int aZ1;
	int aZ2;
	int aY1;
	int aY2;

	int i;

	short int* ptr;

	ptr = (short int*)(spriteActorData + extra->field_0 * 16 + 4);

	X1 = *(ptr++) + extra->X;
	X2 = *(ptr++) + extra->X;

	Z1 = *(ptr++) + extra->Z;
	Z2 = *(ptr++) + extra->Z;

	Y1 = *(ptr++) + extra->Y;
	Y2 = *(ptr++) + extra->Y;

	for (i = 0;i < numActorInRoom;i++) {
		if (actors[i].costumeIndex != -1 && i != param) {
			aX1 = actors[i].X + actors[i].boudingBox.X.bottomLeft;
			aX2 = actors[i].X + actors[i].boudingBox.X.topRight;

			aZ1 = actors[i].Y + actors[i].boudingBox.Y.bottomLeft;
			aZ2 = actors[i].Y + actors[i].boudingBox.Y.topRight;

			aY1 = actors[i].Z + actors[i].boudingBox.Z.bottomLeft;
			aY2 = actors[i].Z + actors[i].boudingBox.Z.topRight;

			if (X1 < aX2 && X2 > aX1 && Z1 < aZ2 && Z2 > aZ1 && Y1 < aY2 && Y2 > aY1) {

				if (extra->field_1E != 0) {
					HitObj(param, i, extra->field_1E, -1);
				}

				return(i);
			}
		}
	}

	return(-1);
}

int FullWorldColBrick(int currentX, int currentZ, int currentY, int oldX, int oldZ, int oldY) {
	int averageX;
	int averageZ;
	int averageY;

	if (WorldColBrick(oldX, oldZ, oldY))
		return(1);

	averageX = abs(currentX + oldX) / 2;
	averageZ = abs(currentZ + oldZ) / 2;
	averageY = abs(currentY + oldY) / 2;

	if (WorldColBrick(averageX, averageZ, averageY))
		return(1);

	if (WorldColBrick(abs(oldX + averageX) / 2, abs(oldZ + averageZ) / 2, abs(oldY + averageY) / 2))
		return(1);

	if (WorldColBrick(abs(currentX + averageX) / 2, abs(currentZ + averageZ) / 2, abs(currentY + averageY) / 2))
		return(1);

	return(0);
}

int ExtraSearch(int actorNum, int X, int Y, int Z, int spriteIdx, int targetActor, int maxSpeed, int param4) {
	int i;

	for (i = 0;i < 50;i++) {
		if (extraList[i].field_0 == -1) {
			extraList[i].field_0 = spriteIdx;
			extraList[i].field_14 = 0x80;
			extraList[i].field_20 = 0;
			extraList[i].X = X;
			extraList[i].Z = Y;
			extraList[i].Y = Z;
			extraList[i].field_1C = actorNum;
			extraList[i].time = targetActor;
			extraList[i].field_12 = maxSpeed;
			extraList[i].field_1E = param4;
			setActorAngle(0, maxSpeed, 50, &extraList[i].trackActorRotation);
			extraList[i].field_16 = GetAngle(X, Z, actors[targetActor].X, actors[targetActor].Z);
			return(i);
		}
	}

	return(-1);
}

void ThrowMagicBall(int X, int Z, int Y, int param1, int angle, int param2, int param3) {
	int ballSprite = -1;
	int ballStrength = 0;

	switch (magicLevel) {
	case 0:
	case 1: {
			ballSprite = 1;
			ballStrength = 4;
			break;
		}
	case 2: {
			ballSprite = 42;
			ballStrength = 6;
			break;
		}
	case 3: {
			ballSprite = 43;
			ballStrength = 8;
			break;
		}
	case 4: {
			ballSprite = 13;
			ballStrength = 10;
			break;
		}
	}

	magicBallNumBounce = ((magicPoint - 1) / 20) + 1;

	if (magicPoint == 0)
		magicBallNumBounce = 0;

	/* if(findExtraKey() != -1)
	 {
	     magicBallNumBounce = 5;
	 } */

	switch (magicBallNumBounce) {
	case 0: {
			magicBallIdx = ThrowExtra(0, X, Z, Y, ballSprite, param1, angle, param2, param3, ballStrength);
			break;
		}
	case 1: {
			magicBallParam = 4;
			magicBallIdx = ThrowExtra(0, X, Z, Y, ballSprite, param1, angle, param2, param3, ballStrength);
			break;
		}
	case 2:
	case 3:
	case 4: {
			magicBallNumBounce = 1;
			magicBallParam = 4;
			magicBallIdx = ThrowExtra(0, X, Z, Y, ballSprite, param1, angle, param2, param3, ballStrength);
			break;
		}
	case 5: {
			printf("Unimplemented magic ball aim for key\n");
			exit(1);
			break;
		}
	}

	if (magicPoint > 0)
		magicPoint--;
}
