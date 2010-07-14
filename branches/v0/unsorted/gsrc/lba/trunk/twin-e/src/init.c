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

void InitProgram(void) {
	initAll("LBA.CFG", 15);
// processVOXfile
}

void initAll(char *fileName, int a) {
// int var0C=0;
// int var10=unkPtr;
	char *environment;
	FILE *cfg_file;
	char **usedCFG;

	printf("Copyright (c) Adeline Software International 1994, All Rights Reserved.\n");
	printf("Compiled the %s at %s\n", __DATE__, __TIME__);
	cfg_file = 0;

	if (a == 0)
		return;

	if (!(a & 16))
		a |= 1;

#ifdef PCLIKE
	environment = getenv("Adeline");
	usedCFG = &environment;
#endif //PCLIKE

	/*
	if (a != -50)
	{
	  if (a & 1)
	  {
	    if (!fileName)
	    {
	      printf("Error: Empty configuration filename. Go kill the programmer! (Tell him to talk to Serge as well...)\n");
	     exit(1);
	    }

	    cfg_file = checkIfFileExist(environment);

	    if (!cfg_file)
	    {
	      if (environment)
	      {
	        printf("Warning: Cannot find configuration file %s as defined in the environment.\n", environment);
	        printf("         Using local configuration file %s.\n", fileName);
	      }
	      cfg_file = checkIfFileExist(fileName);
	      if (!cfg_file)
	      {
	        printf("Error: Cannot find configuration file %s.\n", fileName);
	        exit(1);
	      }
	      usedCFG = &fileName;
	    }

	    printf("Please wait, loading drivers using %s...\n", *usedCFG);
	  }
	  else
	  {
	    cfg_file = checkIfFileExist(environment);
	    if (environment)
	      usedCFG = &environment;
	  }
	}*/

	if (a & 4) {
		printf("Initialising Midi device. Please wait...\n");
	}

	initVideoStuff();

	if (a & 1) {
		printf("Initialising SVGA device. Please wait...\n");
	}

	/*
	 * init stuff for sdl version...
	 */

	useSound = 0;
	useSB = 0;
}

void initVideoStuff(void) {
	int i, j, k;

	frontVideoBuffer = frontVideoBufferbis = Malloc(sizeof(byte) * 307200);

	osystem_initBuffer((char *) frontVideoBuffer, 640, 480);
	osystem_initVideoBuffer(flaBuffer, 320, 200);

	j = 0;
	k = 0;
	for (i = hauteurEcran; i > 0; i--) {
		screenLockupTable[j] = k;
		j++;
		k += largeurEcran;
	}

	initVideoVar1 = -1;
}
