#ifndef READPICT_H
#define READPICT_H
#ifndef ILBM_H
#include <ilbm.h>
#endif

#ifndef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif


#define maxCycles   8
#define RNG_NORATE  36   /* Dpaint uses this rate to mean non-active */

typedef struct {
   WORD  pad1;   /* future exp - store 0 here */
   WORD  rate;   /* 60/sec=16384, 30/sec=8192, 1/sec=16384/60=273 */
   WORD  active; /* lo bit 0=no cycle, 1=yes; next bit 1=rvs */
   UBYTE low;    /* range lower */
   UBYTE high;   /* range upper */
   } CrngChunk;

typedef struct {
   WORD  direction;  /* 0=don't cycle, 1=forward, -1=backwards */
   UBYTE start;      /* range lower */
   UBYTE end;        /* range upper */
   LONG  seconds;    /* seconds between cycling */
   LONG  microseconds; /* msecs between cycling */
   WORD  pad;        /* future exp - store 0 here */
   } CcrtChunk;


#define GetCAMG(context, camg)  \
    IFFReadBytes(context, (BYTE *)camg, sizeof(CamgChunk))

#define ID_CRNG  MakeID('C','R','N','G')
#define GetCRNG(context, crng)  \
    IFFReadBytes(context, (BYTE *)crng, sizeof(CrngChunk))

#define ID_CCRT  MakeID('C','C','R','T')
#define GetCCRT(context, ccrt)  \
    IFFReadBytes(context, (BYTE *)ccrt, sizeof(CcrtChunk))

typedef struct {
ClientFrame clientFrame;
UBYTE foundBMHD;
UBYTE nColorRegs;
BitMapHeader bmHdr;
Color4 colorMap[32 ];
   /* If you want to read any other property chunks, e.g. GRAB or CAMG, add
    * fields to this record to store them. */
   UBYTE foundCAMG;
   CamgChunk camgChunk;
   UBYTE cycleCnt;
   CrngChunk crngChunks[maxCycles]; /* I'll convert CCRT to this */
} ILBMFrame;
typedef UBYTE *UBytePtr;
#ifdef FDwAT
typedef UBytePtr Allocator(LONG);
extern IFFP ReadPicture(LONG, struct BitMap *, ILBMFrame *, Allocator *);
#else
typedef UBytePtr Allocator();
extern IFFP ReadPicture();
#endif
#endif READPICT_H
