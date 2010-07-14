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

void SaveGame(void) {
	FILE* fHandle;
	char temp;

	fHandle = fopen("SAVE.LBA", "wb+");

	if (!fHandle) {
		printf("Error writting savegame !\n");
		exit(1);
	}

	temp = 3;
	fwrite(&temp, 1, 1, fHandle);

	temp = 0x0; // no name at the momment
	fwrite(&temp, 1, 1, fHandle);

	temp = 0xFF; // size of vars
	fwrite(&temp, 1, 1, fHandle);
	fwrite(vars, 255, 1, fHandle);
	fwrite(&currentRoom, 1, 1, fHandle);
	fwrite(&chapter, 1, 1, fHandle);
	fwrite(&comportementHero, 1, 1, fHandle);
	fwrite(&(twinsen->life), 1, 1, fHandle);
	fwrite(&numCoin, 2, 1, fHandle);
	fwrite(&magicLevel, 1, 1, fHandle);
	fwrite(&magicPoint, 1, 1, fHandle);
	fwrite(&numCloverBox, 1, 1, fHandle);
	fwrite(&newTwinsenX, 2, 1, fHandle);
	fwrite(&newTwinsenZ, 2, 1, fHandle);
	fwrite(&newTwinsenY, 2, 1, fHandle);
	fwrite(&(twinsen->angle), 2, 1, fHandle);
	fwrite(&(twinsen->body), 1, 1, fHandle);
	temp = 150; // size of holomapData
	fwrite(&temp, 1, 1, fHandle);
	fwrite(GV14, 150, 1, fHandle);
	fwrite(&fuel, 1, 1, fHandle);
	temp = 28;
	fwrite(&temp, 1, 1, fHandle);
	fwrite(&itemUsed, 28, 1, fHandle);
	fwrite(&numClover, 1, 1, fHandle);
	fwrite(&usingSword, 1, 1, fHandle);

	fclose(fHandle);
}
