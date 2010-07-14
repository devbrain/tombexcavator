
/* dbview.h
 *    This is a wierd structure which has been around through
 *    lots of code.  It could be greatly simplified for this
 *    application, but who cares!  If you don't like it, change
 *    it - or better yet, don't use it since it is simply part
 *    of a sample program.
 */

#include "ilbm.h"
#include "readpict.h"

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define MXVIEWS 7

struct DBView {
   struct BitMap  BitMap;
   struct cprlist *LOFCprList;
   struct cprlist *SHFCprList;
   SHORT DWidth;
   SHORT DHeight;
   SHORT Depth;
   SHORT DxOffset;
   SHORT DyOffset;
   UWORD Modes;
   WORD  colorMap[32];
   UBYTE nColorRegs;
   UBYTE pad;
   PLANEPTR plane0;
   WORD  planewidth;
   WORD  planeheight;
   struct BitMap  BitMap1;
   SHORT DWidth1;
   SHORT DHeight1;
   SHORT Depth1;
   SHORT DxOffset1;
   SHORT DyOffset1;
   UWORD Modes12;
   PLANEPTR plane1;
   WORD  planewidth1;
   WORD  planeheight1;
   WORD  type;
};





