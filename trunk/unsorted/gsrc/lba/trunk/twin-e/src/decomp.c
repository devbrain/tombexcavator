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

#include "decomp.h"

void Expand(int decompressedSize, unsigned char *destination, unsigned char *source) {
	byte loop;
	byte indic;
	unsigned char *jumpBack;
	int size;
	unsigned short int temp;
	int i;

	do {
		loop = 8;
		indic = *(source++);
		do {
			if (!(indic & 1)) {
				temp = *(unsigned short int *)(source);
				source += 2;
				size = temp & 0x0F;
				size += 2;
				decompressedSize -= size;
				jumpBack = destination - (temp >> 4) - 1;
				for (i = 0; i < size; i++) {
					*(destination++) = *(jumpBack++);
				}
				if (decompressedSize <= 0)
					return;
				loop--;
			} else {
				*(destination++) = *(source++);
				loop--;
				decompressedSize--;
				if (decompressedSize <= 0)
					return;
			}
			indic >>= 1;
		} while (loop);
	} while (decompressedSize);
	return;
}
