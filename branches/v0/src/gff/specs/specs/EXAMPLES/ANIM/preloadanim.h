 
/* preloadanim.h
 */
 
#ifndef PRELOADANIM_H
#define PRELOADANIM_H
 
struct FrameHD
   {
   struct FrameHD *next;
   struct FrameHD *prev;
   BitMapHeader *bmhd;
   AnimationHeader *anhd;
   CamgChunk *camg;
   WORD cmap[32];
   ULONG *body;
   ULONG bodylength;
   UBYTE nColorRegs;
   UBYTE pad;
   };
 
struct IFFfile
   {
   struct IFFfile *next;
   char name[40];
   struct FrameHD *firstframe;
   struct FrameHD *lastframe;
   };
 
#endif PRELOADANIM_H
