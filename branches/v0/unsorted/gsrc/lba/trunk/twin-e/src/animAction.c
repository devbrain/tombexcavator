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

char magicLevelForceTable[] = {
	2,
	3,
	4,
	6,
	8,
	0,
};

void GereAnimAction(actor * lactor, int actorNum) {
	char *ebx;
	int var_4;
	int var_18;
	char al;
	int var_54;
	char *edi;
	char *var_48;
	char *var_50;
	char *var_44;
	char *var_40;
	char *var_3C;
	char *var_4C;
	char *var_38;
	char *var_34;
	char *var_30;
	char *edx;
	char *eax;
	short int temp;

	ebx = (char *) lactor->animExtraData;

	var_4 = 0;
	var_18 = *(ebx++);

	while (var_4 < var_18) {
		al = *(ebx++);

		al -= 5;
		if (al > 16)
			return;

		var_54 = al;

		edi = ebx + 1;

		var_48 = edi + 2;
		var_50 = var_48 + 2;
		var_44 = var_50 + 2;
		var_40 = var_44 + 2;
		var_3C = var_40 + 2;
		var_4C = var_3C + 2;
		var_38 = var_4C + 2;

		var_34 = var_38 + 1;
		var_30 = var_34 + 1;

		edx = edi + 11;
		eax = edi + 15;

//        printf("Opcode: %d\n",var_54);

		switch (var_54) {
		case 0: { // attacking !
				temp = *(ebx++);
				temp--;
				if (temp == lactor->animPosition) {
					lactor->field_66 = *(ebx++);
					lactor->dynamicFlagsBF.bUnk0002 = 1;
				} else {
					ebx++;
				}
				break;
			}
		case 1: {
				temp = *(ebx++);

				if (temp == lactor->animPosition) {
					HQ_3D_MixSample(READ_LE_S16(ebx), 0x1000, 1, lactor->X, lactor->Y, lactor->Z);
				}

				ebx += 2;
				break;
			}
		case 2: {
				temp = *(ebx++);
				if (temp == lactor->animPosition) {
					int temp_28;
					int temp_1C;

					temp_28 = READ_LE_S16(ebx);
					ebx += 2;

					temp_1C = READ_LE_S16(ebx);
					ebx += 2;

					HQ_3D_MixSample(temp_28, (rand() % temp_1C) + 0x1000 - (abs(temp_1C) >> 1), 1, lactor->X, lactor->Y, lactor->Z);
				} else {
					ebx += 4;
				}
				break;
			}
		case 3: {
				temp = *(ebx++);
				if (temp == lactor->animPosition) {
					int temp_8;
					int temp_C;
					int temp_24;
					int temp_14;
					int cx;
					int dx;

					temp_8 = READ_LE_S16(ebx);
					ebx += 2;

					temp_C = *(ebx++);

					cx = READ_LE_S16(ebx);
					ebx += 2;

					dx = lactor->angle + READ_LE_S16(ebx);
					ebx += 2;

					temp_24 = READ_LE_S16(ebx);
					ebx += 2;

					temp_14 = *(ebx++);

					ThrowExtra(actorNum, lactor->X, lactor->Y + temp_8, lactor->Z, temp_C, cx, dx, temp_24, temp_14, *(ebx++));
				} else {
					ebx += 11;
				}
				break;
			}
		case 4: {
				if (magicBallIdx == -1) {
					temp = *(ebx++);
					if (temp == lactor->animPosition) {
						int var_8;
						int dx;
						int var_24;
						int var_14;

						var_8 = READ_LE_S16(ebx);
						ebx += 2;
						dx = READ_LE_S16(ebx);
						ebx += 2;
						var_24 = READ_LE_S16(ebx);
						ebx += 2;
						var_14 = *(ebx++);

						ThrowMagicBall(lactor->X, lactor->Y + var_8, lactor->Z, dx, lactor->angle, var_24, var_14);
					} else {
						ebx += 7;
					}
				} else {
					ebx += 8;
				}
				break;
			}
		case 5: {
				temp = *(ebx++);
				if (temp == lactor->animPosition) {
					int dx;
					int cx;

					dx = READ_LE_S16(ebx);
					ebx += 2;
					cx = READ_LE_S16(ebx);
					ebx += 2;

					HQ_3D_MixSample(dx, 0x1000, cx, lactor->X, lactor->Y, lactor->Z);
				} else {
					ebx += 4;
				}
				break;
			}
		case 6: { // shoot auto aiming missile
				temp = *(ebx++);
				if (temp == lactor->animPosition) {
					int var_8;
					int var_C;
					int dx;
					int var_24;
					int temp;

					var_8 = READ_LE_S16(ebx);
					ebx += 2;
					var_C = *(ebx++);
					dx = *(ebx++);
					var_24 = READ_LE_S16(ebx);
					ebx += 2;
					temp = *(ebx++);

					ExtraSearch(actorNum, lactor->X, lactor->Y + var_8, lactor->Z, var_C, dx, var_24, temp);
				} else {
					ebx += 6;
				}
				break;
			}
		case 7: { // shoot with angle offset
				temp = *(ebx);
				ebx = edi;
				if (temp == lactor->animPosition) {
					int distance;
					int angle;
					int var_8;
					int var_C;
					int dx;
					int cx;
					int var_24;
					int var_14;
					int temp;

					distance = Distance2D(lactor->X, lactor->Z, twinsen->X, twinsen->Z);
					angle = GetAngle(lactor->Y, 0, twinsen->Y, distance);

					var_8 = READ_LE_S16(edi);
					ebx = edi + 2;
					var_C = *(ebx++);
					dx = READ_LE_S16(ebx);
					ebx += 2;
					cx = lactor->angle + READ_LE_S16(ebx);
					ebx += 2;
					var_24 = READ_LE_S16(ebx);
					ebx += 2;
					var_14 = *(ebx++);
					temp = *(ebx++);

					ThrowExtra(actorNum, lactor->X, lactor->Y + var_8, lactor->Z, var_C, dx, cx, var_24, var_14, temp);
				}
				ebx = edx;
				break;
			}
		case 8: {
				temp = *(ebx);
				if (temp == lactor->animPosition) {
					// TODO: implement
					//playSound7(READ_LE_S16(edi));
					ebx = var_48;
				} else {
					ebx = var_48;
				}
				break;
			}
		case 10: {
				temp = *(ebx++);

				ebx = edi;
				break;
			}
		case 11: {
				temp = *(ebx++);

				ebx = edi;
				break;
			}
		case 12: { // twinsen attacking
				temp = *(ebx++);
				temp--;
				ebx = edi;
				if (temp == lactor->animPosition) {
					lactor->field_66 = magicLevelForceTable[magicLevel];
					lactor->dynamicFlagsBF.bUnk0002 = 1;
				}
				break;
			}
		case 13: {
				if (*ebx == lactor->animPosition) {
					int throwX;
					int throwY;
					int throwZ;

					int distanceX;
					int distanceY;
					int distanceZ;

					int spriteIdx;

					int param1;
					int param2;
					int param3;
					int param4;

					int strength;

					distanceX = READ_LE_S16(edi);
					distanceY = READ_LE_S16(edi + 2);
					distanceZ = READ_LE_S16(edi + 4);

					Rotate(distanceX, distanceZ, lactor->angle);

					throwX = destX + lactor->X;
					throwY = distanceY + lactor->Y;
					throwZ = destZ + lactor->Z;

					spriteIdx = *(edi + 6);

					param1 = READ_LE_S16(edi + 7);
					param2 = READ_LE_S16(edi + 9) + lactor->angle;
					param3 = READ_LE_S16(edi + 11);
					param4 = *(edi + 13);

					strength = *(edi + 14);

					ThrowExtra(actorNum, throwX, throwY, throwZ, spriteIdx, param1, param2, param3, param4, strength);
				}
				ebx = eax;
				break;
			}
		case 14: {
				if (*ebx == lactor->animPosition) {
					int newAngle;

					int throwX;
					int throwY;
					int throwZ;

					int distanceX;
					int distanceY;
					int distanceZ;

					int spriteIdx;

					int param1;
					int param2;
					int param3;
					int param4;

					int strength;

					newAngle = GetAngle(lactor->Y, 0, twinsen->Y, Distance2D(lactor->X, lactor->Z, twinsen->X, twinsen->Z));

					distanceX = READ_LE_S16(edi);
					distanceY = READ_LE_S16(edi + 2);
					distanceZ = READ_LE_S16(edi + 4);

					Rotate(distanceX, distanceZ, lactor->angle);

					throwX = destX + lactor->X;
					throwY = distanceY + lactor->Y;
					throwZ = destZ + lactor->Z;

					spriteIdx = *(edi + 6);

					param1 = READ_LE_S16(edi + 7) + newAngle;
					param2 = READ_LE_S16(edi + 9) + lactor->angle;
					param3 = READ_LE_S16(edi + 11);
					param4 = *(edi + 13);

					strength = *(edi + 14);

					ThrowExtra(actorNum, throwX, throwY, throwZ, spriteIdx, param1, param2, param3, param4, strength);
				}
				ebx = eax;
				break;
			}
		case 15: { // shoot slightly aiming
				if (*ebx == lactor->animPosition) {
					int distanceX;
					int distanceY;
					int distanceZ;

					int spriteIdx;

					int targetActor;
					int param3;
					int param4;

					distanceX = READ_LE_S16(edi);
					distanceY = READ_LE_S16(edi + 2);
					distanceZ = READ_LE_S16(edi + 4);

					Rotate(distanceX, distanceZ, lactor->angle);

					spriteIdx = *(edi + 6);
					targetActor = *(edi + 7);
					param3 = READ_LE_S16(edi + 8);
					param4 = *(edi + 10);

					ExtraSearch(actorNum, lactor->X + destX, lactor->Y + distanceY, lactor->Z + distanceZ, spriteIdx, targetActor, param3, param4);
				}
				ebx = edx;
				break;
			}
		default: {
#ifdef GAME_DEBUG
				printf("Unsupported opcode %d in GereAnimAction\n", var_54);
#endif
				exit(1);
			}
		}
		var_4++;
	}

}
