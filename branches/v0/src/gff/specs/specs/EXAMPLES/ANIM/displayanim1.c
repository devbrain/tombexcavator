
/* displayanim1.c
 *    library of routines to support and aid the implementation
 *    of multi-buffered graphics on the amiga.
 *
 *    written by Gary Bonham
 *               Sparta, Inc.
 *               Laguna Hills, Ca
 *
 */

#include "intuall.h"
#include "libraries/dos.h"
#include "libraries/dosextens.h"
#include "dbview.h"
#include "functions.h"

/* #define DEBUG */

int Pop,vmode,Active_View,Display_View,Master_View,No_Views,dy,dy2;
int Verbose;

struct DBView DBView[10];
extern struct GfxBase *GfxBase;
extern int ScreenNTSC;

int SaveWW,SaveHH;

struct View v,*graphics;
struct ViewPort vp,vp2;
struct RasInfo ri,ri2;
struct RastPort rp;

extern int Ddx,Ddy;
extern struct Preferences *Prefs;
struct ColorMap *cm[4];
UWORD *col;
struct ColorMap *GetColorMap();
extern BYTE *SavePlanes[2];

/* ==================================== */

DBOpenGraphics(nbufs,ww,hhh,dd)
int nbufs,ww,hhh,dd;
{
   int i,j,hh;
   int DH,DW;

   hh = hhh * dd;
   graphics = GfxBase->ActiView;

   No_Views = nbufs;
   vmode = NULL;

   InitView(&v);
   InitVPort(&vp);

   v.DxOffset += Ddx + Prefs->ViewXOffset;
   v.DyOffset += Ddy + Prefs->ViewYOffset;

   DH = hhh;
   if (DH >= 400) {
      if (ScreenNTSC) DH = (DH - 400) / 2;
      else DH = (DH - 512) / 2;
   }
   else {
      if (ScreenNTSC) DH = (DH - 200) / 2;
      else DH = (DH - 256) / 2;
   }
   DW = ww;
   if (DW >= 640) DW = (DW - 640) / 2;
   else           DW = (DW - 320) / 2;

   vp.DxOffset = -DW;
   vp.DyOffset = -DH;

   ri.RxOffset = 0;
   ri.RyOffset = 0;
   ri.Next = NULL;

   vp.RasInfo = &ri;
   cm[0] = GetColorMap(32L);
   cm[1] = GetColorMap(32L);
   for (i=0;i<2;i++) {
      DBView[i].plane0 = 0;
      DBView[i].plane1 = 0;
      DBView[i].type = 0;
      DBView[i].LOFCprList = 0;
      DBView[i].SHFCprList = 0;
      for (j=0;j<32;j++)
         DBView[i].colorMap[j] = 0;
   }
   for (i=0;i<nbufs;i++) {
      DBView[i].plane0 = (PLANEPTR)SavePlanes[i];
      DBView[i].BitMap.Planes[0] = DBView[i].plane0;
      DBView[i].planewidth = ww;
      DBView[i].planeheight = hh;
      DBView[i].type = 1;
   }
   if (DBView[nbufs-1].plane0 == NULL) GoodBye("Not enough memory for bitmaps");

   SaveWW = ww;
   SaveHH = hh;

   InitRastPort(&rp);

   DBDraw(0);
   DBClear(ww,hhh,dd); /* start on black frame - view 0 */
   DBDisplay(0);
   DBDraw(1);
   DBClear(ww,hhh,dd); /* init Active_View also */
   return(0);
}

/* specify view for draws (output bitmap)
 */

DBDraw(i)
int i;
{
   rp.BitMap = &DBView[i].BitMap;
   Active_View = i;
   return(0);
}

DBCloseGraphics()
{
   int i;

   if (graphics) {
      LoadView(graphics); /* put back the old view */
      graphics = NULL;

      for (i=0;i<2;i++) FreeColorMap(cm[i]);

      FreeVPortCopLists(&vp);
      for (i=0;i<2;i++) {
         if (DBView[i].LOFCprList) FreeCprList(DBView[i].LOFCprList);
         if (DBView[i].SHFCprList) FreeCprList(DBView[i].SHFCprList);
      }
   }
   return(0);
}

DBCloseBuffer(i)
int i;
{
   if (DBView[i].type == 1) {
      if (DBView[i].plane0) FreeRaster(DBView[i].plane0
         ,(long)SaveWW,(long)SaveHH);
   }
   DBView[i].type = 0;
   return(0);
}
 
/* build coplist for given view
 */

MakeDBView()
{
   int DH,DW;
   int i,j;

   i = Active_View;
   if (DBView[i].LOFCprList) FreeCprList(DBView[i].LOFCprList);
   if (DBView[i].SHFCprList) FreeCprList(DBView[i].SHFCprList);
   FreeVPortCopLists(&vp);

   InitView(&v);
   InitVPort(&vp);

   DH = DBView[i].DHeight;
   if (DH >= 400) {
      if (ScreenNTSC) DH = (DH - 400) / 2;
      else DH = (DH - 512) / 2;
   }
   else {
      if (ScreenNTSC) DH = (DH - 200) / 2;
      else DH = (DH - 256) / 2;
   }
   DW = DBView[i].DWidth;
   if (DW >= 640) DW = (DW - 640) / 2;
   else           DW = (DW - 320) / 2;

   vp.DxOffset = -DW;
   vp.DyOffset = -DH;

   v.ViewPort = &vp;
   v.Modes |= vmode;
   v.DxOffset += Ddx + Prefs->ViewXOffset;
   v.DyOffset += Ddy + Prefs->ViewYOffset;

   v.LOFCprList = 0;
   v.SHFCprList = 0;

   vp.DWidth = DBView[i].DWidth;
   vp.DHeight = DBView[i].DHeight;

   vp.DxOffset += DBView[i].DxOffset;
   vp.DyOffset += DBView[i].DyOffset;
   vp.Modes = DBView[i].Modes;
   if (vp.Modes & LACE) v.Modes |= LACE;
   if (vp.Modes & HAM) v.Modes |= HAM;
   vp.RasInfo = &ri;
   col = (UWORD *)cm[i]->ColorTable;
   for (j=0;j<32;j++) {
      *col++ = DBView[i].colorMap[j];
   }
   vp.ColorMap = cm[i];
   ri.BitMap = &DBView[i].BitMap;
   ri.RxOffset = 0;
   ri.RyOffset = 0;
   ri.Next = NULL;
 
   MakeVPort(&v,&vp);
   MrgCop(&v);
   DBView[i].LOFCprList = v.LOFCprList;
   DBView[i].SHFCprList = v.SHFCprList;
   return(0);
}

/*  Display specified view
 */

DBDisplay(i)
int i;
{
   int j;

   j = Active_View;
   Active_View = i;
   MakeDBView();
   LoadView(&v);
   WaitBlit();
   WaitTOF();
   Display_View = i;
   DBDraw(j); /* makes Active_View = j */
   return(0);
}

/* display Active_View
 */

DBflip()
{
   int i;

   i = Display_View;
   DBDisplay(Active_View);
   DBDraw(i); /* set Active_View to prev. Display_View */
   return(0);
}


/*  clear a view to color 0 (black), specified resolution and depth
 */

DBClear(iwidth,iheight,idepth)
int iwidth,iheight,idepth;
{
   long plsize,iview;
   long i,j;
   LONG *p;

   iview = Active_View;
   InitBitMap(&DBView[iview].BitMap,(long)idepth,(long)iwidth,(long)iheight);
   DBView[iview].DWidth = iwidth;
   DBView[iview].DHeight = iheight;
   DBView[iview].Depth = idepth;
   DBView[iview].DxOffset = 0;
   DBView[iview].DyOffset = 0;
   if (iwidth >= 640) DBView[iview].Modes = HIRES;
   else DBView[iview].Modes = NULL;
   if (iheight >= 400) DBView[iview].Modes |= LACE;
   if (idepth == 6)   DBView[iview].Modes |= EXTRA_HALFBRITE;
   DBView[iview].nColorRegs = 1<<idepth;
   if (idepth >= 6) DBView[iview].nColorRegs = 32;
   DBView[iview].colorMap[0] = 0;
   plsize = (long)RowBytes(iwidth) * (long)iheight;
   j = (plsize * (long)idepth) >>2;
   p = (LONG *)DBView[iview].plane0;
   for (i=0;i<j;i++) *(p + i) = 0; /* BltClear but will work in fast ram */
   for (i=0;i<idepth;i++)
      DBView[iview].BitMap.Planes[i]
          = DBView[iview].plane0 + plsize * (long)i;
   return(0);
}
 
DBCopy(dview,sview)
WORD sview,dview;
{
   LONG nl,i,*ss,*dd;

   DBInitView(dview,sview);

   nl =    DBView[sview].BitMap.Rows
        * (DBView[sview].BitMap.BytesPerRow>>2)
        *  DBView[sview].BitMap.Depth;
   ss = (LONG *)DBView[sview].plane0;
   dd = (LONG *)DBView[dview].plane0;

   for (i=0;i<nl;i++) {
      *dd = *ss;
      dd++;
      ss++;
   }
   return(0);
}
 
DBInitView(View,oldview)
WORD View,oldview;
{
   long ia,ip;
   for (ia=0;ia<32;ia++) DBView[View].colorMap[ia] =
                                 DBView[oldview].colorMap[ia];
   DBView[View].DWidth     = DBView[oldview].DWidth;
   DBView[View].DHeight    = DBView[oldview].DHeight;
   DBView[View].Depth      = DBView[oldview].Depth;
   DBView[View].DxOffset   = DBView[oldview].DxOffset;
   DBView[View].DyOffset   = DBView[oldview].DyOffset;
   DBView[View].nColorRegs = DBView[oldview].nColorRegs;
   DBView[View].Modes      = DBView[oldview].Modes;
   InitBitMap(&DBView[View].BitMap,(long)DBView[View].Depth
      ,(long)DBView[View].DWidth,(long)DBView[View].DHeight);
   ia = (long)(DBView[oldview].DWidth>>3) * (long)DBView[oldview].DHeight;
   for (ip=0;ip<DBView[oldview].Depth;ip++) {
      DBView[View].BitMap.Planes[ip] = DBView[View].plane0 + ia * ip;
   }
   return(0);
}

