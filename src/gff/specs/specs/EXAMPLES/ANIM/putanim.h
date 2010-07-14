#ifndef PUTANIM_H
#define PUTANIM_H
#ifndef COMPILER_H
#include "compiler.h"
#endif
#ifndef ILBM_H
#include "ilbm.h"
#endif
typedef struct {
   UBYTE operation; /* =5 for RIFF style */
   UBYTE mask;
   UWORD w,h;
   WORD  x,y;
   ULONG abstime; /* for timing w.r.t. start of anim file in jiffies (notused)*/
   ULONG reltime; /* for timing w.r.t. last frame in jiffies */
   UBYTE interleave; /* =0 for XORing to two frames back, =1 for last frame
                             (not used yet)  */
   UBYTE pad0;
   ULONG bits;
   UBYTE pad[16];
   } AnimationHeader;
extern BOOL OpenAnim();
extern BOOL AddAnim();
extern BOOL CloseAnim();
extern BOOL InitAnimHdr();
#endif PUTANIM_H
 
