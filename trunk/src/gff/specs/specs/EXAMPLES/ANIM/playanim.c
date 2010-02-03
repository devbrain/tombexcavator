
/* PlayAnim.c
 *
 * Load an ILBM raster image file from C structures in memory
 * into an existing bitmap (preloaded files from PreLoadAnim).
 * 
 * Written by Gary Bonham, Sparta, Inc.  15 Aug 1986
 *
 */

#include "intuall.h"
#include "libraries/dos.h"
#include "libraries/dosextens.h"
#include "ilbm.h"
#include "readpict.h"
#include "putanim.h"
#include "preloadanim.h"
#include "dbview.h"
#include "functions.h"
#include "stdio.h"

ULONG Bits;
WORD *ytable,table[500];
extern int loopplay,continuousplay;
int phase = 0;
extern int Pop;
extern int Active_View,Display_View;
extern struct DBView DBView[];
static BitMapHeader masterbmhd;
 
/* ============================================================ */

PlayAnim(fileptr,ii,Nn)
int ii,Nn;
struct IFFfile *fileptr;
{
   char c;
   int nn,i,anim;
   struct FrameHD *currentframe;
   struct BitMap *bm;
   UWORD mod;
   int maxtime,notimes;
 
   maxtime = notimes = 0;
   if (--ii < 0) ii = 0;
   anim = 0;
   currentframe = fileptr->firstframe;
   Pop = 0;
   if (phase) {
      anim = 1;
      currentframe = currentframe->next->next;
   }
   else phase = 1;
 
   while (currentframe) {
      bm = &DBView[Active_View].BitMap;
      if (anim == 0) {
         if (currentframe->bmhd) {
            SetBMHD(bm,currentframe);
            DBView[Active_View].DWidth =
               DBView[Display_View].DWidth = currentframe->bmhd->pageWidth;
            DBView[Active_View].DHeight =
               DBView[Display_View].DHeight = currentframe->bmhd->pageHeight;
            DBView[Active_View].Depth =
               DBView[Display_View].Depth = currentframe->bmhd->nPlanes;
            DBView[Active_View].DxOffset = DBView[Display_View].DxOffset = 0;
            DBView[Active_View].DyOffset = DBView[Display_View].DyOffset = 0;
            if (currentframe->camg) {
               mod = currentframe->camg->ViewModes;
            }
            else {
               mod = 0;
               if (currentframe->bmhd->pageWidth >= 640) mod |= HIRES;
               if (currentframe->bmhd->pageHeight>= 400) mod |= LACE;
            }
            DBView[Active_View].Modes = DBView[Display_View].Modes = mod;
            DBView[Active_View].nColorRegs =
               DBView[Display_View].nColorRegs = 32;
            for (i=0;i<32;i++) 
               DBView[Active_View].colorMap[i] =
                  DBView[Display_View].colorMap[i] = 0;
         }
         else GoodBye("First frame must have BMHD");
      }
      else if (currentframe->bmhd) SetBMHD(bm,currentframe);
      if (currentframe->anhd) SetANHD(bm,currentframe);
      if (currentframe->nColorRegs) SetCMAP(bm,currentframe);
      if (Pop >= 2) {
         if (currentframe->body) {
            if (Pop == 5) SetRIFF(bm,currentframe);
            else GoodBye("illegal op value for delta mode");
         }
      }
      else {
         if (currentframe->body) SetBODY(bm,currentframe);
      }
      DBflip();
      Chk_Abort();
      if (anim == 0) {
         DBCopy(Active_View,Display_View);
         anim++;
         if (ii == 0) phase = 0;
      }
      if (currentframe) currentframe = currentframe->next;
    }
   if (ii <= 1) {
      phase = 0;
      return(0);
   }
   else return(ii);
}

SetBMHD(bm,cfr)
struct BitMap *bm;
struct FrameHD *cfr;
{
   LONG plsize;
   int i;
 
   InitBitMap(bm,(long)cfr->bmhd->nPlanes
                ,(long)cfr->bmhd->w
                ,(long)cfr->bmhd->h);
   plsize = (long)RowBytes(cfr->bmhd->w)
          * (long)cfr->bmhd->h;
   for (i=0;i<cfr->bmhd->nPlanes;i++) {
      bm->Planes[i] = DBView[Active_View].plane0 + plsize*i;
   }
   masterbmhd = *cfr->bmhd;
   return(0);
}
 
SetCMAP(bm,cfr)
struct BitMap *bm;
struct FrameHD *cfr;
{
   int i;
   for (i=0;i<cfr->nColorRegs;i++) {
      DBView[Active_View].colorMap[i] = cfr->cmap[i];
   }
   return(0);
}
 
SetANHD(bm,cfr)
struct BitMap *bm;
struct FrameHD *cfr;
{
   Pop = cfr->anhd->operation;
   Bits= cfr->anhd->bits;
   return(0);
}
 
/*** SetRIFF(bm,cfr) ***/

SetRIFF(bm,cfr)
struct BitMap *bm;
struct FrameHD *cfr;
{
   register int j,jmax;
   int i,nBpR;
   unsigned char *ptr;
   LONG *deltadata;
   register unsigned char *deltabyte;

   nBpR = bm->BytesPerRow;
   make_ytable(nBpR<<3,bm->Rows);
   deltadata = (LONG *)cfr->body;
   for (i=0;i<6;i++) {
      if (deltadata[i]) {
         deltabyte = (unsigned char *)deltadata + deltadata[i];
         ptr = (unsigned char *)(bm->Planes[i]);
         decode_vkplane(deltabyte,ptr,(long)nBpR);
      }
   }
   return(0);
}

/*** make_ytable(width,height) ***/

make_ytable(width,height)
WORD width,height;
{
   register WORD linebytes,*pt,acc;

   linebytes = (((width+15) >>4) <<1);
   ytable = &table[0];
   pt = ytable;
   acc = 0;
   while (--height >= 0) {
      *pt++ = acc;
      acc += linebytes;
   }
   return(1);
}

SetBODY(bm,cfr)
struct BitMap *bm;
struct FrameHD *cfr;
{
   UWORD w,h;
   LONG x,y;
   UBYTE mask;
   register long deltabyte;
   long i,irow;
   register long ip;
   BYTE *planes[6],*ss;
   register BYTE **dd;
   register WORD srcRowBytes;
 
   if (cfr->anhd) {
      w = cfr->anhd->w;
      h = cfr->anhd->h;
      x = cfr->anhd->x;
      y = cfr->anhd->y;
      mask = cfr->anhd->mask;
   }
   else {
      w = cfr->bmhd->w;
      h = cfr->bmhd->h;
      x = cfr->bmhd->x;
      y = cfr->bmhd->y;
      mask = 0xFF;
   }

   srcRowBytes = RowBytes(w);
   deltabyte = (masterbmhd.pageWidth>>3) - (w>>3);
 
   /* assume cmpByteRun1 data compression */
   for (i=0;i<6;i++) {
      if ((mask >>i) &1) planes[i] = (BYTE *)bm->Planes[i]
                                   + y * bm->BytesPerRow + (x>>3);
      else planes[i] = NULL;
   }
 
   ss = (BYTE *)cfr->body;

   for (irow = h;irow>0;irow--) {
      for (ip=0;ip<masterbmhd.nPlanes;ip++) {
         if (planes[ip]) {
            dd = &planes[ip];
            UnPackRow(&ss,dd,10000,srcRowBytes);
            planes[ip] += deltabyte;
         }
      }
   }
   return(0);
}
 
