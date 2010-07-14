 
/* displayanim
 *
 *      Main program for animation file playing program
 *         Derived from ShowANIM code as a working example of
 *         ANIM playback.
 *
 *      Gary Bonham
 *      SPARTA, Inc.
 *      Laguna Hills, Ca
 *
 */


#include "intuall.h"
#include "graphics/display.h"
#include "libraries/dos.h"
#include "libraries/dosextens.h"
#include "ilbm.h"
#include "readpict.h"
#include "stdio.h"
#include "time.h"
#include "putanim.h"
#include "functions.h"
#include "preloadanim.h"
 
extern int loopplay,continuousplay;

extern int EndFlag;
int Ddx,Ddy;
extern struct IFFfile *IFFfileList;
extern LONG file;
extern LONG OpenArg();
char cc2[40],cc[40],line[81];
struct Preferences *Prefs;
int ScreenNTSC;
 
extern struct GfxBase *GfxBase;

/* ================================================= */

void main (argc,argv)
int argc;
char **argv;
{
   BYTE *ripcord;
   char ch;
   int i,ii,GoOn,delay,loops,loop;

   Prefs = 0;
 
   puts("\n*** SPARTA DisplayANIM ver. 1.00 ***\n");
   puts("\nUse CTRL-C to abort once ANIM is playing\n");
 
   OpenLibs();                         /* open libraries */
   Prefs = (struct Preferences *)AllocMem
                 ((long)sizeof(struct Preferences),0L);
   GetPrefs(Prefs,(long)sizeof(struct Preferences));

   if (GfxBase->DisplayFlags & PAL) ScreenNTSC = 0;
   else ScreenNTSC = 1;

   Ddx = 0;
   Ddy = 0;
   if ((file = OpenArg(argc,argv)) == NULL)
      GoodBye("Can't open ANIM file");
   puts("Patience, Loading ANIM into memory...");
   ripcord = AllocMem(16000L,(long)MEMF_CHIP); /* so ANIM doesn't use it all */
      EndFlag = 0;
      LoadAnim("ANIM");
   FreeMem(ripcord,16000L);
   GfxBase->system_bplcon0 |= INTERLACE;

            delay = 0;
            loops = 1;
            ii = 1;
            if (continuousplay) {
               if (EndFlag) {
                  puts(" *** -c changed to -l ***\n");
                  loopplay = continuousplay;
                  continuousplay = 0;
               }
               else ii = continuousplay;
            }
            if (loopplay)       loops = loopplay;
            DBOpenGraphics(2,IFFfileList->firstframe->bmhd->pageWidth
                            ,IFFfileList->firstframe->bmhd->pageHeight
                            ,IFFfileList->firstframe->bmhd->nPlanes);
               for (loop=0;loop<loops;loop++) {
                  i = ii + 1;
                  while ((i = PlayAnim(IFFfileList,i,delay))) {}
               }
            DBCloseGraphics();
      GfxBase->system_bplcon0 &= ~INTERLACE;
   for (i=0;i<2;i++) DBCloseBuffer(i);
   ClosePreLoadIFF();
   GoodBye("done...");
}
 
getch()
{
   char scr[80];
   if (gets(scr)) {
      if (scr[0] >= 'a' && scr[0] <= 'z')
         scr[0] = scr[0] - 'a' + 'A';
      return ((int)scr[0]);
   }
   return(' ');
}

