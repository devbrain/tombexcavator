/* LINCONIO.H - Wrapper to NCurses on Linux, used to keep DOS conio.h compatibility
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

#ifndef LINCONIO_H__
#define LINCONIO_H__

#include <ncurses.h>
// We'll just wrap the functions wherex(), wherey(), and gotoxy() here.

#ifdef __cplusplus
extern "C" {
#endif

#define IS_X 0
#define	IS_Y 1

// I don't really know what these do, but I'll try to work based on their names.
void    gotoxy(int _x, int _y);
int     wherex();
int     wherey();

// Saves some values required by linconio calls at start
int     linconio_init(void);

void    do_output(char* format, ...);

extern int NoWait;

#undef printf
#define printf do_output

#ifdef __cplusplus
}
#endif
 
#endif /*!LINCONIO_H__*/
