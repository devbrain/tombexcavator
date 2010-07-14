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

int lastFireTime = 0;

void makeFireEffect(int top, int param) {
	unsigned char *temp;
	unsigned char *out;
	int i, j;
	unsigned char temp3;
	unsigned char bh, bl;

	/*while(lastFireTime==time) // loop to wait next frame
	{
	      osystem_delay(5);
	};*/

	lastFireTime = lba_time;

	makeFireEffectInit();

	temp = fireEffectVar1 + 1600;

	out = frontVideoBuffer + screenLockupTable[top];

	bl = param;
	bh = bl + 15;

	for (i = 25; i >= 0; i--) {
		for (j = 320; j >= 0; j--) {
			temp3 = *temp;
			temp3 = temp3 >> 1;
			temp3 += bl;
			if (temp3 > bh)
				temp3 = bh;

			out[0] = temp3;
			out[1] = temp3;
			out[640] = temp3;
			out[641] = temp3;

			temp++;
			out += 2;
		}
		out += 640;
	}
}

void makeFireEffectInit(void) {
	unsigned char *temp1 = 0;
	unsigned char *temp2 = 0;
	char *temp2bis = 0;
	short int temp4 = 0;
	int i = 0;
	unsigned int *copy1 = 0;
	unsigned int *copy2 = 0;

	fireEffectVar1 = bufSpeak;
	fireEffectVar2 = bufSpeak + 16000;

	temp1 = fireEffectVar1 + 321; // on commence a la ligne 1
	temp2 = fireEffectVar2 + 321; // on commence a la ligne 1

	for (i = 0; i < 15358; i++) { // for all the lines except the last one
		temp4 = 0;

		temp4 += temp1[-1];
		temp4 += temp1[-320];
		temp4 += temp1[-319];
		temp4 += temp1[-321];
		temp4 += temp1[+1];
		temp4 += temp1[+320];
		temp4 += temp1[+319];
		temp4 += temp1[+321];

		temp4 = (temp4 >> 3) | ((temp4 & 0x0003) << 13);  // pour recoder le ror

		if (!(temp4 & 0x6500)) {
			if (temp2 >= (fireEffectVar2 + 14720)) {
				temp4--;
			} else if (temp4 > 0) {
				temp4--;
			}
		}

		temp1++;
		*temp2 = (unsigned char) temp4;
		temp2++;
	}

	copy1 = (unsigned int *) fireEffectVar1;
	copy2 = (unsigned int *)(fireEffectVar2 + 320);   // on decalle tout d'une ligne vers le haut

	for (i = 0; i < 3840; i++)
		*(copy1++) = *(copy2++);  // on copie la frame actuelle dans le buffer 1 avec une ligne de moins

	temp2bis = (char *) fireEffectVar2 + 12480; // ptr sur les 11 dernieres lignes

	for (i = 1600; i >= 0; i--) { // prepare la prochaine frame
		temp4 = *temp2bis;
		if (temp4 <= 15) {
			*temp2bis = -(temp4 - 11);
		}
		temp2++;
	}
}
