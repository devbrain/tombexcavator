
/* putanim.c
 *   opens an animation iff file by outputing the starting
 *   bitmap.  Then successive change frames may be output
 *   followed by closing the file.
 *
 *   written by G. Bonham      May 1986
 *              SPARTA, Inc.
 *              Laguna Hills, Calif
 */

#include "intuall.h"
#include "gio.h"
#include "ilbm.h"
#include "putanim.h"
#include "functions.h"
#include "stdio.h"

#define BODY_BUFSIZE 512
#define MaxDepth 5

#define CkErr(exp) {if(ifferror==IFF_OKAY)ifferror=(exp);}

extern UBYTE BMmask;
extern int Pop;
GroupContext fileC, form1C, form2C;
static IFFP ifferror = 0;
BYTE *Bufsave;
LONG Bufsizesave;
static UWORD bHw,bHh;
static WORD bHx,bHy;
 
/* =================================================== */
 
BOOL OpenAnim(file,bm,pageW,pageH,colorMap,modes,buffer,bufsize)
UWORD modes;
LONG file;
struct BitMap *bm;
WORD pageW,pageH;
WORD *colorMap;
BYTE *buffer;  LONG bufsize;
{
   UBYTE depth;
   BitMapHeader bmHdr;
   ULONG lmodes;

   lmodes = modes;
   ifferror = InitBMHdr(&bmHdr,bm,(WORD)mskNone,(WORD)cmpByteRun1
                 ,0,pageW,pageH);

   if(ifferror != IFF_OKAY) printf("iff error: %d\n",ifferror);
   if (ifferror == IFF_OKAY && bufsize > 2 * BODY_BUFSIZE) {
      if (GWriteDeclare(file,buffer+BODY_BUFSIZE
                ,(long)(bufsize-BODY_BUFSIZE)) < 0)
         ifferror = DOS_ERROR;
      bufsize = BODY_BUFSIZE;
   }

   depth = bm->Depth;
   if (depth > 5) depth = 5;
   CkErr(OpenWIFF(file,&fileC,(long)szNotYetKnown));
   CkErr(StartWGroup(&fileC,(ID)FORM,(long)szNotYetKnown,ID_ANIM,&form1C));
      CkErr(StartWGroup(&form1C,(ID)FORM,(long)szNotYetKnown,ID_ILBM,&form2C));
         CkErr(PutCk(&form2C,ID_BMHD,(long)sizeof(BitMapHeader),(BYTE *)&bmHdr));
         if (colorMap) CkErr(PutCMAP(&form2C,colorMap,depth));
         CkErr(PutCk(&form2C,ID_CAMG,(long)sizeof(CamgChunk),(BYTE *)&lmodes));
         CkErr(PutBODY(&form2C,bm,(long)NULL,&bmHdr,buffer,(long)bufsize));
      CkErr(EndWGroup(&form2C));
   CkErr(CloseWGroup(&form1C));
   if(ifferror != IFF_OKAY) printf("openanim - iff error: %d\n",ifferror);
   return((BOOL)(ifferror != IFF_OKAY));
}

BOOL AddAnim(bm,pageW,pageH,pop,buffer,bufsize)

struct BitMap *bm;
WORD pageW,pageH,pop;
BYTE *buffer;
LONG bufsize;

{
   AnimationHeader animHdr;
   BitMapHeader bmHdr;

      /* InitBMHdr sets w and h and x and y in bmHdr */
      Pop = pop;
      ifferror = InitBMHdr(&bmHdr,bm,(WORD)mskNone,(WORD)cmpByteRun1
                ,0,pageW,pageH);
      bHx = bmHdr.x;
      bHy = bmHdr.y;
      bHw = bmHdr.w;
      bHh = bmHdr.h;

      ifferror = InitAnimHdr(&animHdr,pop);
      if(ifferror != IFF_OKAY) printf("iff error: %d\n",ifferror);

   CkErr(OpenWGroup(&fileC,&form1C));
      CkErr(StartWGroup(&form1C,(ID)FORM,(long)szNotYetKnown,ID_ILBM,&form2C));
         CkErr(PutCk(&form2C,ID_ANHD,(long)sizeof(AnimationHeader),(BYTE *)&animHdr));
         CkErr(PutBODY(&form2C,bm,(long)NULL,&bmHdr,buffer,(long)bufsize));
      CkErr(EndWGroup(&form2C));
   CkErr(CloseWGroup(&form1C));
   if(ifferror != IFF_OKAY) printf("addanim - iff error: %d\n",ifferror);
   return((BOOL)(ifferror != IFF_OKAY));
}

BOOL AddAnim2(data,ndata,pop)
int pop;
LONG *data;
LONG ndata;
{
   AnimationHeader animHdr;

   ifferror = InitAnimHdr(&animHdr,pop);
   if(ifferror != IFF_OKAY) printf("iff error: %d\n",ifferror);

   CkErr(OpenWGroup(&fileC,&form1C));
      CkErr(StartWGroup(&form1C,(ID)FORM,(long)szNotYetKnown,ID_ILBM,&form2C));
         CkErr(PutCk(&form2C,ID_ANHD,(long)sizeof(AnimationHeader),(BYTE *)&animHdr));
         CkErr(PutCk(&form2C,ID_DLTA,ndata,data));
      CkErr(EndWGroup(&form2C));
   CkErr(CloseWGroup(&form1C));
   if(ifferror != IFF_OKAY) printf("addanim - iff error: %d\n",ifferror);
   return((BOOL)(ifferror != IFF_OKAY));
}

BOOL CloseAnim(file)

LONG file;

{
   Pop = 0;
   CkErr(PutCkEnd(&fileC));
   CkErr(CloseWGroup(&fileC));
   if (GWriteUndeclare(file) < 0 && ifferror == IFF_OKAY) ifferror = DOS_ERROR;
   return ((BOOL)(ifferror != IFF_OKAY));
}

BOOL InitAnimHdr(aH,op)
AnimationHeader *aH;
WORD op;

{
   int i;

   aH->operation = op;
   aH->mask = 0;
   aH->x = 0;
   aH->y = 0;
   aH->w = 0;
   aH->h = 0;
   aH->interleave = 0;
   aH->reltime = 0;
   aH->abstime = 0;
   aH->bits = 0;
   aH->pad0 = 0;
   for (i=0;i<16;i++) aH->pad[i] = 0;
   return(0);

}

