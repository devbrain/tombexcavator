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

int getAnimIndexForBody(byte anim, short int actorNumber) {
	actor *act;
	char type;

	unsigned char *bodyPtr;
	unsigned char *ptr, *ptr2;
	unsigned char *costumePtr = NULL;
	unsigned short int var1;

	act = &actors[actorNumber];
	bodyPtr = act->entityDataPtr;

	do {
		type = *(bodyPtr++);

		if (type == -1) {
			loadTwinsenCostumesVar1 = NULL;
			return (-1);
		}

		ptr = (bodyPtr + 1);

		if (type == 3) {
			if (anim == *bodyPtr) {
				ptr++;
				var1 = READ_LE_S16(ptr);
				ptr += 2;
				ptr2 = ptr;
				ptr++;
				if (*ptr2 != 0) {
					costumePtr = ptr - 1;
				}
				loadTwinsenCostumesVar1 = costumePtr;
				return (var1);
			}
		}

		bodyPtr = *ptr + ptr;

	} while (1);

	return (0);
}
