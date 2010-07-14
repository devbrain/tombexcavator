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

void GiveExtraBonus(actor * lactor) {
	int i;
	char extraTable[8];
	int numOfExtra = 0;
	char currentBonus;
	int angle;

	for (i = 0;i < 5;i++) {
		if (lactor->field_10&(1 << (i + 4))) {
			extraTable[numOfExtra++] = i;
		}
	}

	if (numOfExtra) {
		currentBonus = extraTable[rand()%numOfExtra];

		currentBonus += 3;

		if (!magicLevel && currentBonus == 2) // if bonus is magic and magicLevel 1 not reached
			currentBonus = 1; //bonus is life

		if (lactor->dynamicFlagsBF.bUnk0020) {
			ExtraBonus(lactor->X, lactor->Y, lactor->Z, 0x100, 0, currentBonus, lactor->field_12);
			HQ_3D_MixSample(11, 0x1000, 1, lactor->X, lactor->Y, lactor->Z);
		} else {
			angle = GetAngle(lactor->X, lactor->Z, twinsen->X, twinsen->Z);
			ExtraBonus(lactor->X, lactor->Y + lactor->boudingBox.Y.topRight, lactor->Z, 200, angle, currentBonus, lactor->field_12);
			HQ_3D_MixSample(11, 0x1000, 1, lactor->X, lactor->Y + lactor->boudingBox.Y.topRight, lactor->Z);
		}
	}
}
