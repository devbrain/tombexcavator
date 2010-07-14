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
/*
-----------------
memory usage list:
-----------------

data:

- fic         11k    82 files
- inventory       60k    28 files
- text         170k    56 files  (french only)
- scenes       250k   120 files
- sprites      290k   118 files
- animes       440k   516 files
- body         620k   132 files
- samples     3600k   230 files

-----------------------------------------
Special:

- bricks      3900k   unpacked, used to speed up cube loading
-----------------------------------------

total:        9341k



*/


void InitBufferCube(void) {
	// todo: mettre les codes d'erreur

	bufCube = Malloc(204800);  // 204800 = 64*64*25*2
	bufferBrick = Malloc(361472);
}
