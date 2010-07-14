/*
Copyright (C) 2002-2004 The TwinE team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at youoption) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _LBA_
#define _LBA_

#ifndef _EE
# ifndef WIN32
#  include "config.h"
# endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"

#ifdef WIN32
# define PCLIKE
#endif

#ifdef UNIX
# define PCLIKE
#endif

#ifdef PCLIKE
# ifdef MACOSX
#  include <malloc/malloc.h>
# else
#  include <malloc.h>
# endif
# ifdef USE_GL
#  include <windows.h>    // Header File For Windows
#  include <gl/gl.h>      // Header File For The OpenGL32 Library
#  include <gl/glu.h>     // Header File For The GLu32 Library
#  include <gl/glaux.h>   // Header File For The Glaux Library
# endif
#endif

#ifndef PCLIKE
# ifdef DREAMCAST
#  include <shinobi.h>    /* Shinobi system routines. */
#  include <kamui2.h>     /* Kamui2 low-level graphics HAL. */
#  include <sn_fcntl.h>   /* LibCross file types. */
#  include <usrsnasm.h>   /* LibCross I/O routines. */
#  include <sg_syCbl.h>   /* NTSC/RGB/VGA Cable check interface. */
#  define PRELOAD_ALL
#  define USE_GL
# endif
#endif

#include <assert.h>

#ifdef NDEBUG
# define verify(exp)     (exp)
#else
# define verify(exp)     (assert(exp))
#endif

#include "HQRlib.h"
#include "HQMLib.h"

#include "streamReader.h"

#include "samples.h"
#include "music.h"

#ifndef MEM_DEBUG
# define Malloc(size) malloc(size)
# define MemFree(ptr) free(ptr)
#endif


#ifdef GAME_DEBUG
# define todo(string) printf("TODO %s:%d %s\n", __FILE__, __LINE__, string)
#else // _DEBUG
# define todo(string)
#endif // _DEBUG

#ifdef GAME_DEBUG
# define assert_ptr(ptr) assert(((int)ptr!=0xCDCDCDCD) && (ptr!=NULL))
#else // _DEBUG
# define assert_ptr(ptr)
#endif // _DEBUG

#ifndef PCLIKE
# define exit(string) printf("Exit required\n")
#endif

#ifdef MACOSX
# define convertDWFromLE(x) ((x >> 24) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000) | (x << 24))
# define convertWFromLE(x) (((x>>8)&0xFF)|((x<<8)&0xFF00))
#else
# define convertDWFromLE(x) (x)
# define convertWFromLE(x) (x)
#endif

struct mouseStatusStruct {
	int left;
	int right;
	int X;
	int Y;
};

typedef struct mouseStatusStruct mouseStatusStruct;

struct actorBoxStruct {
	int actorNum;
	int left;
	int right;
	int top;
	int bottom;
};

typedef struct actorBoxStruct actorBoxStruct;

// memory sizes
#define BUF_SPEAK_SIZE 262176
#define BUF_MEMOSEEK_SIZE 2048
#define BUF_TEXT_SIZE 25000
#define BUF_ORDER_SIZE 1024
#define BUF_ANIM_SIZE 5000
#define BUF_INVENTORY_SIZE 20000

#define BUF_HQM_SIZE 400000

// defines for generic animations
#define ANIM_static 0
#define ANIM_walk 1
#define ANIM_walkBackward 2
#define ANIM_turnLeft 3
#define ANIM_turnRight 4
#define ANIM_hit 5
#define ANIM_hitBig 6
#define ANIM_fall 7
#define ANIM_land 8
#define ANIM_landHit 9
#define ANIM_landDeath 10
#define ANIM_activate 11
#define ANIM_climbUpLadder 12
#define ANIM_reachTopOfLadder 13
#define ANIM_jump 14
#define ANIM_throwBall 15
#define ANIM_hide 16
#define ANIM_kick 17
#define ANIM_rightPunch 18
#define ANIM_leftPunch 19
#define ANIM_findObject 20
#define ANIM_drown 21
#define ANIM_hit2 22
#define ANIM_swordAttack 23
#define ANIM_drawSword 24
#define ANIM_fightMode 25
#define ANIM_fightAttack 26
#define ANIM_run 27 // lba2= push
#define ANIM_joinHands 28 // lba2 = talk
#define ANIM_pushMode 29
#define ANIM_pushForward 30 // lba2= climb down ladder

#define ANIM_useSwitch 31
#define ANIM_useUnk 32

#define ANIM_crawl 33
#define ANIM_climbInHole 34
#define ANIM_useUnk2 35
#define ANIM_useSarbacan 36
#define ANIM_dead 37
#define ANIM_sportifTurnLeft 38
#define ANIM_backwardJump 39
#define ANIM_crawl2 40
#define ANIM_usePotion 41

// lba2 48 = use switch
// lba2 49 = crawl

//#include "opcodes.h"
#include "osystem.h"
#include "angle.h"
#include "costume.h"
#include "debug.h"
#include "zones.h"
#include "vars.h"
#include "angleTable.h"
#include "overlay.h"
#include "file.h"

#define MAX_PATH 256
#define DATA_DIR "data/"

#endif
