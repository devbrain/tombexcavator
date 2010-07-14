#ifndef ILBM_H
#define ILBM_H
#ifndef COMPILER_H
#include "compiler.h"
#endif
#ifndef GRAPHICS_GFX_H
#include "graphics/gfx.h"
#endif
#include "iff.h"
#define ID_ANFR MakeID('A','N','F','R')
#define ID_MAHD MakeID('M','A','H','D')
#define ID_MFHD MakeID('M','F','H','D')
#define ID_CM16 MakeID('C','M','1','6')

#define ID_ILBM MakeID('I','L','B','M')
#define ID_ANIM MakeID('A','N','I','M')
#define ID_BMHD MakeID('B','M','H','D')
#define ID_ANHD MakeID('A','N','H','D')
#define ID_CMAP MakeID('C','M','A','P')
#define ID_GRAB MakeID('G','R','A','B')
#define ID_DEST MakeID('D','E','S','T')
#define ID_SPRT MakeID('S','P','R','T')
#define ID_CAMG MakeID('C','A','M','G')
#define ID_BODY MakeID('B','O','D','Y')
#define ID_ATXT MakeID('A','T','X','T')
#define ID_PTXT MakeID('P','T','X','T')
#define ID_DLTA MakeID('D','L','T','A')
typedef UBYTE Masking;
#define mskNone 0L
#define mskHasMask 1L
#define mskHasTransparentColor 2L
#define mskLasso 3L
typedef UBYTE Compression;
#define cmpNone 0L
#define cmpByteRun1 1L
#define x320x200Aspect 10L
#define y320x200Aspect 11L
#define x320x400Aspect 20L
#define y320x400Aspect 11L
#define x640x200Aspect 5L
#define y640x200Aspect 11L
#define x640x400Aspect 10L
#define y640x400Aspect 11L
typedef struct {
   ULONG ViewModes;
   } CamgChunk;
typedef struct {
UWORD w, h;
WORD x, y;
UBYTE nPlanes;
Masking masking;
Compression compression;
UBYTE pad1;
UWORD transparentColor;
UBYTE xAspect, yAspect;
WORD pageWidth, pageHeight;
} BitMapHeader;
#define RowBytes(w) (((w) + 15) >> 4 << 1)
typedef struct {
UBYTE red, green, blue;
} ColorRegister;
#define sizeofColorRegister 3
typedef WORD Color4;
#define MaxAmDepth 6
typedef struct {
WORD x, y;
} Point2D;
typedef struct {
UBYTE depth;
UBYTE pad1;
UWORD planePick;
UWORD planeOnOff;
UWORD planeMask;
} DestMerge;
typedef UWORD SpritePrecedence;
typedef struct {
WORD pad1;
WORD rate;
WORD active;
UBYTE low, high;
} CRange;
#define PutCAMG(context, modes) \
PutCk(context, ID_CAMG, (long)sizeof(CamgChunk), (BYTE *)modes)
#define PutBMHD(context, bmHdr) \
PutCk(context, ID_BMHD, (long)sizeof(BitMapHeader), (BYTE *)bmHdr)
#define PutGRAB(context, point2D) \
PutCk(context, ID_GRAB, (long)sizeof(Point2D), (BYTE *)point2D)
#define PutDEST(context, destMerge) \
PutCk(context, ID_DEST, (long)sizeof(DestMerge), (BYTE *)destMerge)
#define PutSPRT(context, spritePrec) \
PutCk(context, ID_SPRT, (long)sizeof(SpritePrecedence), (BYTE *)spritePrec)
#ifdef FDwAT
extern IFFP InitBMHdr(BitMapHeader *, struct BitMap *,
WORD, WORD, WORD, WORD, WORD);
extern IFFP PutCMAP(GroupContext *, WORD *, UBYTE);
extern IFFP PutBODY(
GroupContext *, struct BitMap *, BYTE *, BitMapHeader *, BYTE *, LONG);
#else
extern IFFP InitBMHdr();
extern IFFP PutCMAP();
extern IFFP PutBODY();
#endif FDwAT
#define GetBMHD(context, bmHdr) \
IFFReadBytes(context, (BYTE *)bmHdr, (long)sizeof(BitMapHeader))
#define GetGRAB(context, point2D) \
IFFReadBytes(context, (BYTE *)point2D, (long)sizeof(Point2D))
#define GetDEST(context, destMerge) \
IFFReadBytes(context, (BYTE *)destMerge, (long)sizeof(DestMerge))
#define GetSPRT(context, spritePrec) \
IFFReadBytes(context, (BYTE *)spritePrec, (long)sizeof(SpritePrecedence))
#define MaxSrcPlanes 16+1
#ifdef FDwAT
extern IFFP GetCMAP(GroupContext *, WORD *, UBYTE *);
extern IFFP GetBODY(
GroupContext *, struct BitMap *, BYTE *, BitMapHeader *, BYTE *, LONG);
#else
extern IFFP GetCMAP();
extern IFFP GetBODY();
#endif FDwAT
#endif ILBM_H
