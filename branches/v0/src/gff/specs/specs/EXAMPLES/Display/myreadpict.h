/* myreadpict.h
 * Modified 12/88 - removed Camg, Ccrt, Crng defs (now in ilbm.h)
 */

#ifndef MYREADPICT_H
#define MYREADPICT_H


#ifndef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif

#ifndef ILBM_H
#include <iff/ilbm.h>
#endif


#define EXDepth 6        /* Maximum depth (6=HAM) */
#define maxColorReg 32
#define maxCycles   8
#define RNG_NORATE  36   /* Dpaint uses this rate to mean non-active */

typedef struct {
   ClientFrame clientFrame;
   UBYTE foundBMHD;
   UBYTE nColorRegs;
   BitMapHeader bmHdr;
   Color4 colorMap[maxColorReg];
   /* If you want to read any other property chunks, e.g. GRAB or CAMG, add
    * fields to this record to store them. */
   UBYTE foundCAMG;
   CamgChunk camgChunk;
   UBYTE cycleCnt;
   CRange crngChunks[maxCycles]; /* I'll convert CCRT to this */
   } ILBMFrame;

typedef UBYTE *UBytePtr;

#ifdef FDwAT
extern IFFP myReadPicture(LONG, ILBMFrame *);
extern struct BitMap *getBitMap(ILBMFrame *);
#else 
extern IFFP myReadPicture();
extern struct BitMap *getBitMap();
#endif

#endif MYREADPICT_H


