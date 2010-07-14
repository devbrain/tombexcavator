 
/* displayanim0.c
 *
 */

#include "intuall.h"
#include <workbench/startup.h>
#include "libraries/dos.h"
#include "libraries/dosextens.h"
#include "ilbm.h"
#include "readpict.h"
#include "stdio.h"
#include "functions.h"
 
/* For WorkBench startup */    
extern struct WBStartup *WBenchMsg;
BOOL   fromWB;
struct FileLock *startLock, *newLock;

LONG file;
extern long marktime;
static ILBMFrame iframe;
extern struct Preferences *Prefs;
int loopplay,continuousplay;
extern int ScreenNTSC;
 
struct IntuitionBase *IntuitionBase;
struct GfxBase       *GfxBase;
struct IconBase      *IconBase;
struct DosBase       *DosBase;
extern struct View   *graphics;
/* ================================================ */

LONG OpenArg(argc,argv)
int argc;
char **argv;
{
   char *filename;
   struct WBArg    *arg;  
   int i,j;
   char  *str;

   j = 0;
   fromWB = (argc==0) ? TRUE : FALSE;
   if((argc>1)&&(*argv[1] != '?')) {  /* Passed filename via command line  */
      for (i=1;i<argc;i++) {
         str = argv[i];
         if (*str == '-') {
            while (*++str) {
               switch (*str) {
                  case 'l':
                  case 'L':
                     loopplay = 0;
                     while (*++str) {
                        loopplay = (loopplay * 10) + (*str - '0');
                     }
                     --str;
                     if (loopplay <= 0) loopplay = 10000;
                     break;
                  case 'c':
                  case 'C':
                     continuousplay = 0;
                     while (*++str) {
                        continuousplay = (continuousplay * 10) + (*str - '0');
                     }
                     --str;
                     if (continuousplay <= 0) continuousplay = 10000;
                     break;
                  default:
                     printf("'%c' bad option\n",*str);
                     break;
               }
            }
         }
         else {
            if (j) {
               puts("Only one file may be specified");
               exit(1L);
            }
            j = i;
         }
      }
   }
   else if ((argc==0)&&(WBenchMsg->sm_NumArgs > 1)) {
      arg = WBenchMsg->sm_ArgList;
      arg++;
      filename   = (char *)arg->wa_Name;
      newLock    = (struct FileLock *)arg->wa_Lock;
      startLock  = (struct FileLock *)CurrentDir(newLock);
      return (Open(filename,(long)MODE_OLDFILE));
   }
   if (j == 0) {
      puts("Usage: 'DisplayANIM [-L#] [-C#] filename");
      puts("            -L#   non-continuous loop play");
      puts("                  # is optional no. of times to loop");
      puts("                  default is 10000 loops");
      puts("            -C#   continuous loop play");
      puts("                  # is optional no. of times to loop");
      puts("                  default is 10000 loops");
      GoodBye("T.T.F.N."); /* contrary to many people's concerns, this is
                              not an obscure error message.  For those not
                              already familiar with Winnie the Pooh, this
                              is the famous parting message by Tigger which
                              stands for "Ta Ta For Now".  There have even
                              been inquiries to the product support people
                              at Aegis regarding this cryptic message. */
   }
   return (Open(argv[j],(long)MODE_OLDFILE));
}

_abort()
{
   int i;

   DBCloseGraphics();
   puts("*** User Abort ***");
  
   ClosePreLoadIFF();
   if (Prefs) FreeMem(Prefs,(long)sizeof(struct Preferences));
   for (i=0;i<2;i++) DBCloseBuffer(i);
   if (GfxBase)       CloseLibrary(GfxBase);
   if (IntuitionBase) CloseLibrary(IntuitionBase);
   if (DosBase)       CloseLibrary(DosBase);
 
   exit(1);
}

GoodBye(msg)
char *msg;
{
   int i;

   DBCloseGraphics();
   puts(msg);
  
   ClosePreLoadIFF();
   if (Prefs) FreeMem(Prefs,(long)sizeof(struct Preferences));
   for (i=0;i<2;i++) DBCloseBuffer(i);
   if (GfxBase)       CloseLibrary(GfxBase);
   if (IntuitionBase) CloseLibrary(IntuitionBase);
   if (DosBase)       CloseLibrary(DosBase);
 
   exit(0);
}

void OpenLibs()
{
   GfxBase       = NULL;
   IntuitionBase = NULL;
   IconBase      = NULL;
   DosBase       = NULL;
   graphics      = NULL;

   if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0L)))
      GoodBye("No graphics library");
   if (!(IntuitionBase
       = (struct IntuitionBase *)OpenLibrary("intuition.library",0L)))
      GoodBye("No intuition library");
   if (!(DosBase = (struct DosBase *)OpenLibrary("dos.library",0L)))
      GoodBye("No dos library");
}
 
 
LoadAnim(fname)
char *fname;
{
   IFFP iffp = NO_FILE;

   if (file) iffp = PreLoadAnimation(fname,file,&iframe);
   if (newLock != startLock)  CurrentDir(startLock);
   Close(file);
 
   if (iffp != IFF_DONE) {
      printf("Load of %s failed: Iff status %ld\n",fname,(long)iffp);
      return(1);
   }
   return(0);
}
 
