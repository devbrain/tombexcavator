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

void GetShadow(int X, int Z, int Y) {
	int tempX;
	int tempZ;
	int tempY;
	unsigned char* ptr;

	tempX = (X + 0x100) >> 9;
	tempZ = Z >> 8;
	tempY = (Y + 0x100) >> 9;

	ptr = bufCube + tempZ * 2 + tempX * 25 * 2 + (tempY << 6) * 25 * 2;

	while (tempZ) { // search down until either ground is found or lower border of the cube is reached
		if (*(short int*)ptr) // found the ground
			break;

		tempZ--;
		ptr -= 2;
	}

	shadowVar = 0;

	getPosVar1 = tempX;
	getPosVar2 = tempZ;
	getPosVar3 = tempY;

	processActorX = X;
	processActorZ = (tempZ + 1) << 8;
	processActorY = Y;

	if (*ptr) {
		unsigned char* tempPtr;

		tempPtr = zbufferSub1(*(ptr++) - 1) + 3;

		shadowVar = *(tempPtr + *(ptr) * 4);

		ReajustPos(shadowVar);
	}

	shadowX = processActorX;
	shadowZ = processActorZ;
	shadowY = processActorY;
}
