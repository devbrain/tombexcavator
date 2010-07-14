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
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
/*
extern volatile int lba_time;

int threadTimer(void *test) // LBA time is 50 frames per seconds ie,  a frame every 20 miliseconds
{
    int temp = SDL_GetTicks();
    while (1)
  {
      SDL_Delay(2); // granularity restriction
        if(SDL_GetTicks() - temp >= 15)
        {
            musicPosition++;
          lba_time++;
            temp = SDL_GetTicks();
        }
  }
    return (0);
}*/

void startThreadTimer() {
	// SDL_CreateThread(threadTimer, NULL);
}
