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

#ifdef PCLIKE
#include "SDL/SDL.h"
#endif

int InitCDR(char *string) {
#ifdef PCLIKE
	// int cur_track;
	// int min, sec, frame;
	int numOfCDROM;
	int cdNum;

	numOfCDROM = SDL_CDNumDrives();

	printf("Found %d CDROM devices\n", numOfCDROM);

	if (!numOfCDROM) {
		/*
		* None found
		*/
		fprintf(stderr, "No CDROM devices available\n");
		exit(-1);
	}

	for (cdNum = 0; cdNum < numOfCDROM; cdNum++) {

		printf("Testing drive %s\n", SDL_CDName(cdNum));

		/*
		* Open the default drive
		*/
		cdrom = SDL_CDOpen(cdNum);

		/*
		* Did if open? Check if cdrom is NULL
		*/

		if (!cdrom) {
			fprintf(stderr, "Couldn't open drive: %s\n", SDL_GetError());
		} else

		{
			/*
			 * Print Volume info
			 */

			SDL_CDStatus(cdrom);
			if (cdrom->numtracks == 10) {
				printf("Assuming that it is LBA cd...\n");
				return(1);
			}

		}
		SDL_CDClose(cdrom);
	}

	cdrom = NULL;

	printf("Can't find LBA1 CD!\n");

#endif

	return (0);
}

void stopCD(void) {
#ifdef PCLIKE
	if (cdrom != NULL)
		SDL_CDStop(cdrom);
#endif
}
