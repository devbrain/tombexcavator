/* LINCONIO.C - Wrapper to NCurses on Linux, used to keep DOS conio.h compatibility
**              (used in ModKeen 2.0.1 Linux port)
**
** Code Copyright (C) 2007 by Ignacio R. Morelle "Shadow Master". (shadowm2006@gmail.com)
** March 11th 2007
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
** Permission is granted to anyone to use this software for any purpose, including
** commercial applications, and to alter it and redistribute it freely, subject
** to the following restrictions:
**    1. The origin of this software must not be misrepresented; you must not
**       claim that you wrote the original software. If you use this software in
**       a product, an acknowledgment in the product documentation would be
**       appreciated but is not required.
**    2. Altered source versions must be plainly marked as such, and must not be
**       misrepresented as being the original software.
**    3. This notice may not be removed or altered from any source distribution.
*/

// Parts come from DOSBox 0.65 source code (debug/debug_gui.cpp)

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "linconio.h"

int NoWait = 0;

WINDOW	*screen;
int		oldcurstat;
void linconio_exit();

void do_output(char* format, ...)
{
	char buf[512];
	va_list msg;
	va_start(msg, format);
	vsprintf(buf, format, msg);
	va_end(msg);
	
	wprintw(screen, "%s", buf);
	wrefresh(screen);
}

int linconio_init()
{
	screen = initscr();
	if (!screen) return -1;

	cbreak();
	noecho();
	nonl();
	touchwin(screen);
	curs_set(0);
	atexit(&linconio_exit);
	return 0;
}

void linconio_exit()
{
	if (!NoWait) do_output("Press a key to exit...");
	getch();
	curs_set(oldcurstat);
	endwin();
}

void gotoxy(int _x, int _y)
{
	wmove(screen, _y, _x);
}

int wherex()
{
	int _y, _x;
	getyx(screen, _y, _x);
	return _x;
}

int wherey()
{
	int _y, _x;
	getyx(screen, _y, _x);
	return _y;
}
