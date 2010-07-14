/*
 * Display v1.06 - 11/88 Carolyn Scheppner   CBM
 *
 *   Read an ILBM file and display as a screen/window until closed.
 *   Simulated close gadget in upper left corner of window.
 *   Clicking below title bar area toggles screen bar for dragging.
 *   Handles normal and HAM ILBM's
 *   Now has options for backscreen, timer, cycling, printing
 *
 *   Options:
 *
 *     opt  b     means come up behind other screens
 *          c     means cycle colors
 *          p     where P means dump to printer
 *          e     default 6 planes to extra-halfbrite
 *          t=n   where n = display time in seconds (without or after dump)
 *
 *  By Carolyn Scheppner   CBM  01/15/88
 *
 *  Modified 09/02/86 - Only global frame is iFrame
 *                      Use message->MouseX and Y
 *                      Wait() for IDCMP
 *  Modified 10/15/86 - For HAM
 *                      Name changed from SeeILBM to ViewILBM
 *  Modified 11/01/86 - Revised for linkage with myreadpict.c
 *  Modified 11/18/86 - For Astartup ... Amiga.lib, LC.lib linkage
 *  Modified 12/12/86 - Added color cycling at request of Mimetics
 *  Modified 01/06/87 - Tab toggles cycling
 *  Modified 03/03/87 - Recognizes RNG_NORATE (36) as non-active DP CRNG
 *                      Changed name to Display
 *  Modified 03/13/87 - Accepts display time in seconds as 2nd CLI arg
 *  Modified 01/15/88 - New command line options, now prints
 *  Modified 04/20/88 - Mask troublesome flags from Viewmodes
 *  Modified 05/06/88 - (v1.04) Add CTRL/D to exit returning failure, e flag
 *  Modified 09/27/88 - (v1.05) Use CAMG, CRNG, and CCRT defs in new ilbm.h
 *  Modified 11/08/88 - (v1.06) Explicitly mask high word of CAMG
 *
 *     Display supports cycling, timed display, printing, and backscreen.
 *     See usage lines. Type Display<RET> or double-click Display for help.
 *     If the command line opt c or picture tooltype CYCLE=TRUE are used,
 *     this viewer will cycle any ILBM that contains cycling chunks
 *     (CCRT or CRNG) which are marked as active and do not have a CRNG
 *     cycle rate of 36. (To DPaint, rate 36 = don't cycle).  Note that
 *     by default, DPaint saves its pics with CRNG (cycling) chunks
 *     flagged as active and with a rate not equal to 36.  
 *
 * Based on ShowILBM.c, readpict.c    1/86
 *  By Jerry Morrison, Steve Shaw, and Steve Hayes, Electronic Arts.
 *  This software is in the public domain.
 *
 * >>NOTE<<: This example must be linked with additional IFF rtn files.
 *           See linkage information below.
 *
 * The display portion is specific to the Commodore Amiga computer.
 *
 * Linkage Information:
 *  (NOTE: All modules including iff stuff compiled with -v on LC2)
 *
 * FROM     LIB:AStartup.obj,Display.o,myreadpict.o,dump.o,iffmsgs.o*
 *          iffr.o,ilbmr.o,unpacker.o
 * TO       Display
 * LIBRARY  LIB:Amiga.lib, LIB:LC.lib
 * 
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

#include "iff/ilbm.h"
#include "myreadpict.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif  MIN

#define TOUPPER(c)      ((c)>='a'&&(c)<='z'?(c)-'a'+'A':(c)) 

/* Bits we must mask out of CAMG.Viewmodes */
#define BADFLAGS  (SPRITES|VP_HIDE|GENLOCK_AUDIO|GENLOCK_VIDEO)
#define FLAGMASK  (~BADFLAGS)
#define CAMGMASK  (FLAGMASK & 0x0000FFFFL)

/* The screendump routine */
extern int dump();

/* For wbStdio rtns */
extern LONG stdin, stdout, stderr;  /* in Astartup.obj */

char conSpec[] = "CON:0/40/640/140/";
BOOL wbHasStdio = NULL;


/* general usage pointers */
struct GfxBase       *GfxBase;
struct IntuitionBase *IntuitionBase;
ULONG  IconBase = 0;

/* Globals for displaying an image */
struct Screen   *screen1;
struct Window   *window1;
struct RastPort *rport1;
struct ViewPort *vport1;

struct BitMap   tBitMap;      /* Temp BitMap struct for small pics  */

/* For WorkBench startup */    
extern struct WBStartup *WBenchMsg;
struct FileLock *startLock, *newLock;

/* Other globals */
BOOL  FromWb, TBtoggle, Done;
BOOL  Cycle=FALSE, Print=FALSE, Timer=FALSE, Back=FALSE, EHB=FALSE;

char u1[]  = "\nDISPLAY   v1.06  C. Scheppner  CBM  11/88\n";

char u1c[] = "\nCLI Usage: Display ilbmfile [opt [b][c][e][p] [t=n]]\n";
char u2c[] =   "     opts: b=backscreen c=cycle e=ehb p=print t=seconds\n";

char u1w[] = "\n WB Usage: Click this icon, SHIFT and DoubleClick on pic\n";
char u2w[] =   "ToolTypes: Display  TIMER=n,PRINT=TRUE,BACK=TRUE\n";
char u3w[] =   "           Picture  CYCLE=TRUE, EHB=TRUE\n";

char u2[]  = "\nClick toggles bar, Tab toggles cycling, P prints screen\n";
char u3[]  = "Close upper left or CTRL/C, or CTRL/D to break a script\n";

char *cliUsage[] = {u1,u1c,u2c,u2,u3,""};
char *wbUsage[]  = {u1,u1w,u2w,u3w,u2,u3,""};

/* Structures for new Screen, new Window */

struct   TextAttr       TextFont = {
   "topaz.font",                    /* Font Name   */
   TOPAZ_EIGHTY,                    /* Font Height */
   FS_NORMAL,                       /* Style       */
   FPF_ROMFONT,                     /* Preferences */
   };

struct   NewScreen      ns = {
   0, 0,                                  /* LeftEdge and TopEdge   */
   0, 0,                                  /* Width and Height       */
   0,                                     /* Depth                  */
   1, 0,                                  /* DetailPen and BlockPen */
   NULL,                                  /* Special display modes  */
   CUSTOMSCREEN,                          /* Screen Type            */
   &TextFont,                             /* Use my font            */
   " <- Close here after clicking below",                  /* Title */
   NULL,                                  /* No gadgets yet         */
   NULL,                                  /* Ptr to CustomBitmap    */
   };

struct   NewWindow      nw = {
   0, 0,                                  /* LeftEdge and TopEdge */
   0, 0,                                  /* Width and Height */
   -1, -1,                                /* DetailPen and BlockPen */
   MOUSEBUTTONS|VANILLAKEY,               /* IDCMP Flags */
   BACKDROP
   |BORDERLESS,                           /* Flags */
   NULL, NULL,                            /* Gadget and Image pointers */
   NULL,                                  /* Title string */
   NULL,                                  /* Put Screen ptr here */
   NULL,                                  /* SuperBitMap pointer */
   0, 0,                                  /* MinWidth and MinHeight */
   0, 0,                                  /* MaxWidth and MaxHeight */
   CUSTOMSCREEN,                          /* Type of window */
   };

USHORT  allBlack[maxColorReg] = {0};

/* For alloc to define new pointer */
#define PDATASZ 12
UWORD   *pdata;

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif  MIN

extern char *IFFPMessages[];
ILBMFrame   iFrame;       /* my global frame */


/* Cycle Task stuff */
#define CYCLETIME  16384L
#define REVERSE    0x02
#define ACTIVE     0x01

extern VOID cycleTask();
char *cyTaskName = "CAS_D1.04cyTask";
struct Task *cyTask;

/* Data shared with cycle/timer Task */
CRange *cyCrngs;
struct ViewPort *cyVport;
int    cyRegs, cyCnt;
USHORT cyMap[maxColorReg];
LONG   cyClocks[maxCycles];
LONG   cyRates[maxCycles];
LONG   dTimer;
BOOL   TimerOn, CycleOn, PrepareToDie;
struct Task *mainTask;
LONG   tSigNum = -1, retcode = RETURN_OK;
ULONG  tSig;

/*
 *  main
 */

main(argc, argv)
int argc;
char **argv;
   {
   ULONG           signals, wSig;
   LONG            file;
   IFFP            iffp = NO_FILE;
   struct WBArg    *arg;  
   char            *filename;
   int error;

   FromWb = (argc==0) ? TRUE : FALSE;
   TimerOn = FALSE;

   if((FromWb)&&(WBenchMsg->sm_NumArgs > 1))
      {
      /* Passed filename via Workbench */
      arg = WBenchMsg->sm_ArgList;
      arg++;
      filename   = (char *)arg->wa_Name;
      newLock    = (struct FileLock *)arg->wa_Lock;
      startLock  = (struct FileLock *)CurrentDir(newLock);
      /* Get ToolTypes */
      getWbOpts(WBenchMsg);
      }
   else if((!FromWb)&&(argc>1)&&(*argv[1] != '?'))
      {
      /* Passed filename via command line */
      filename = argv[1];

      if(argc>2)
         {
         if(strEqu(argv[2],"opt"))  getCliOpts(argc,argv);
         else  cleanexit("Bad args\n",RETURN_FAIL);
         }
      }
   else
      {
      usage();
      cleanexit(" ",RETURN_OK); /* Space forces wait for keypress if WB */
      }


   if(!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0)))
      cleanexit("Can't open graphics",RETURN_FAIL);

   if(!(IntuitionBase=
          (struct IntuitionBase *)OpenLibrary("intuition.library",0)))
      cleanexit("Can't open intuition",RETURN_FAIL);

   if(!(file = Open(filename, MODE_OLDFILE)))
      cleanexit("Picture file not found",RETURN_WARN);

   iffp = myReadPicture(file,&iFrame);
   Close(file);

   if (!(iffp == IFF_DONE))
      cleanexit("Not an IFF ILBM",RETURN_WARN);


   error = DisplayPic(&iFrame);
   if(error)  cleanexit("Can't open screen or window",RETURN_WARN);

   if(pdata = (UWORD *)AllocMem(PDATASZ,MEMF_CHIP|MEMF_CLEAR))
      {
      pdata[2] = 0x8000;  /* 1 pixel */
      SetPointer(window1,pdata,1,16,0,0);
      }

   /* Set up cycle/timer task */

   mainTask = (struct Task *)FindTask(NULL);
   if((tSigNum = AllocSignal(-1)) == -1)
      cleanexit("Can't alloc timerSig",RETURN_FAIL);
   tSig = 1 << tSigNum;
   wSig = 1<<window1->UserPort->mp_SigBit;

   initCycle(&iFrame,vport1);
   cyTask = (struct Task *)CreateTask(cyTaskName,0,cycleTask,4000);
   if(!cyTask)  cleanexit("Can't create timimg task",RETURN_FAIL);

   /* Dump screen if requested before starting timer */
   if(Print)  dump(screen1);

   if(Timer)  TimerOn = TRUE;
   if(Cycle)  CycleOn = TRUE;

   TBtoggle   = FALSE;      /* Title bar toggle */
   Done       = FALSE;      /* Close flag       */

   while (!Done)
      {
      signals = Wait(SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_C|wSig|tSig);
      if(signals & wSig)  chkmsg();
      if(signals & tSig)  Done = TRUE;
      if(signals & SIGBREAKF_CTRL_C)  Done = TRUE;
      if(signals & SIGBREAKF_CTRL_D)  Done = TRUE, retcode=RETURN_FAIL;
      }
   cleanexit("",retcode);
   }


getCliOpts(argc,argv)
int  argc;
char **argv;
   {
   int k,i;
   UBYTE c;

   for(k=3; k<argc; k++)
      {
      c = argv[k][0] | 0x20;
      switch(c)
         {
         case 't':
            i=0;
            while((argv[k][i])&&(argv[k][i] != '=')) i++;
            i++;
            dTimer = 60 * atoi(&argv[k][i]);
            Timer = TRUE;
            break;
         default:
            for(i=0; argv[k][i]; i++)
               {
               c = argv[k][i] | 0x20;
               switch(c)
                  {
                  case 'b':
                     Back = TRUE;
                     break;
                  case 'p':
                     Print = TRUE;
                     break;
                  case 'c':
                     Cycle = TRUE;
                     break;
                  case 'e':
                     EHB = TRUE;
                     break;
                  default:
                     break;
                  }
               }

         }
      }
   }



getWbOpts(wbMsg)
struct WBStartup *wbMsg;
   {
   struct WBArg  *wbArg;
   struct DiskObject *diskobj;
   char **toolarray;
   char *s;

   if((IconBase = OpenLibrary("icon.library", 0)))
      {
      /* First get ToolTypes from Display.info */
      wbArg = wbMsg->sm_ArgList;
      diskobj=(struct DiskObject *)GetDiskObject(wbArg->wa_Name);
      if(diskobj)
         {
         toolarray = (char **)diskobj->do_ToolTypes;

         if(s=(char *)FindToolType(toolarray,"PRINT"))
            {
            if(strEqu(s,"TRUE"))  Print = TRUE;
            }
         if(s=(char *)FindToolType(toolarray,"BACK"))
            {
            if(strEqu(s,"TRUE"))  Back = TRUE;
            }
         if(s=(char *)FindToolType(toolarray,"TIMER"))
            {
            Timer = TRUE;
            dTimer = 60 * atoi(s);
            }
         FreeDiskObject(diskobj);
         }

      if(wbMsg->sm_NumArgs > 1)
         {
         wbArg++;

         diskobj=(struct DiskObject *)GetDiskObject(wbArg->wa_Name);
         if(diskobj)
            {
            toolarray = (char **)diskobj->do_ToolTypes;

            if(s=(char *)FindToolType(toolarray,"CYCLE"))
               {
               if(strEqu(s,"TRUE"))  Cycle = TRUE;
               }
            if(s=(char *)FindToolType(toolarray,"EHB"))
               {
               if(strEqu(s,"TRUE"))  EHB = TRUE;
               }
            FreeDiskObject(diskobj);
            }
         }
      CloseLibrary(IconBase);
      }
   }


initCycle(ptFrame,vp)
ILBMFrame *ptFrame;
struct ViewPort *vp;
   {
   int k;

   CycleOn  = FALSE;
   PrepareToDie = FALSE;
   cyCrngs  = ptFrame->crngChunks;
   cyVport  = vp;
   cyRegs   = ptFrame->nColorRegs;
   cyCnt    = ptFrame->cycleCnt;

   for(k=0; k<cyRegs; k++)
      {
      cyMap[k] = ptFrame->colorMap[k];
      }

   /* Init Rates and Clocks */
   for(k=0; k<cyCnt; k++)
      {
      /* In DPaint CRNG, rate = RNG_NORATE (36) means don't cycle */
      if(cyCrngs[k].rate == RNG_NORATE)
         {
         cyCrngs[k].rate = 0;
         cyCrngs[k].active &= ~ACTIVE;
         }

      if((cyCrngs[k].active & ACTIVE)&&(cyCrngs[k].rate))
         {
         cyRates[k] = cyCrngs[k].rate;
         }
      else
         {
         cyRates[k] = 0;  /* Means don't cycle to my cycleTask */
         }
      cyClocks[k] = 0;
      }
   }


VOID cycleTask()
   {
   int    k, i, j;
   UBYTE  low, high;
   USHORT cyTmp;
   BOOL   Cycled;

   while(!PrepareToDie)
      {
      WaitTOF();
      if(CycleOn)
         {
         Cycled = FALSE;
         for(k=0; k<cyCnt; k++)
            {
            if(cyRates[k])  /* cyRate 0 = inactive */
               {
               cyClocks[k] += cyRates[k];
               if(cyClocks[k] >= CYCLETIME)
                  {
                  Cycled = TRUE;
                  cyClocks[k] -= CYCLETIME;
                  low = cyCrngs[k].low;
                  high= cyCrngs[k].high;
                  if(cyCrngs[k].active & REVERSE)  /* Reverse cycle */
                     {
                     cyTmp = cyMap[low];
                     for(i=low,j=low+1; i < high; i++,j++)
                        {
                        cyMap[i] = cyMap[j];
                        }
                     cyMap[high] = cyTmp;
                     }
                  else     /* Forward cycle */
                     {
                     cyTmp = cyMap[high];
                     for(i=high,j=high-1; i > low; i--,j--)
                        {
                        cyMap[i] = cyMap[j];
                        }
                     cyMap[low] = cyTmp;
                     }
                  }
               }
            }
         if(Cycled)
            {
            LoadRGB4(cyVport,cyMap,cyRegs);
            }
         }
      if(TimerOn)
         {
         if(--dTimer <= 0)  Signal(mainTask,tSig);
         }
      }
   PrepareToDie = FALSE;
   Wait(0L);  /* Wait to die */
   }


chkmsg()
   {
   struct IntuiMessage *msg;
   ULONG class, code;
   SHORT mouseX, mouseY;

   while(msg=(struct IntuiMessage *)GetMsg(window1->UserPort))
      {
      class = msg->Class;
      code  = msg->Code;
      mouseX = msg->MouseX;
      mouseY = msg->MouseY;

      ReplyMsg(msg);
      switch(class)
         {
         case MOUSEBUTTONS:
            if ((code == SELECTDOWN)&&
                  (mouseX < 10)&&(mouseY<10))
               {
               Done = TRUE;
               }
            else if ((code == SELECTDOWN)&&
                       ((mouseY>10)||(mouseX>10))&&
                          (TBtoggle==FALSE))
               {
               TBtoggle = TRUE;
               ShowTitle(screen1,TRUE);
               ClearPointer(window1);
               }
            else if ((code == SELECTDOWN)&&
                       (mouseY>10)&&(TBtoggle==TRUE))
               {
               TBtoggle = FALSE;
               ShowTitle(screen1,FALSE);
               SetPointer(window1,pdata,1,16,0,0);
               }
            break;
         case VANILLAKEY:
            switch(code)
               {
               case 0x03:    /* CTRL/C */
                  Done = TRUE;
                  break;
               case 0x04:    /* CTRL/D */
                  Done = TRUE;
                  retcode = RETURN_FAIL;
                  break;
               case 'p': case 'P':
                  dump(screen1);
                  break;
               case 0x09:    /* Tab toggles Cycle */
                  if(CycleOn)
                     {
                     CycleOn = FALSE;
                     WaitTOF();        /* Make sure cyTask saw FALSE */
                     WaitBOVP(vport1);
                     LoadRGB4(vport1,iFrame.colorMap,maxColorReg);
                     }
                  else
                     {
                     initCycle(&iFrame,vport1);
                     CycleOn = TRUE;
                     }
                  break;
               default:
                  break;
               }
            break;
         default:
            break;
         }
      }
   }


usage()
   {
   char **ulines;
   int k;

   if((FromWb)&&(! wbHasStdio))  wbHasStdio = openStdio(conSpec);

   if((!FromWb)||(wbHasStdio))
      {
      ulines = FromWb ? wbUsage : cliUsage;
      for(k=0; ulines[k][0]; k++)
         {
         Write(stdout,ulines[k],strlen(ulines[k]));
         }
      }
   }


cleanexit(s,rcode)
char  *s;
LONG  rcode;
   {
   if(*s)
      {
      if((FromWb)&&(!wbHasStdio))  wbHasStdio = openStdio(conSpec);

      if((!FromWb)||(wbHasStdio))
         {
         Write(stdout,s,strlen(s));
         Write(stdout,"\n",1);
         }
      if(wbHasStdio)
         {
         Write(stdout,"\nPRESS RETURN TO EXIT\n",22);
         while (getchar() != '\n');
         }
      }
   cleanup();
   if(wbHasStdio) closeStdio();
   exit(rcode);
   }


cleanup()
   {
   struct IntuiMessage *msg;

   if(cyTask)
      {
      CycleOn = FALSE;
      PrepareToDie = TRUE;
      while(PrepareToDie)  Delay(10);
      DeleteTask(cyTask);
      }

   /* Free timer signal */
   if (tSigNum > -1)  FreeSignal(tSigNum);

   /* Note - tBitMap planes were deallocated in DisplayPic() */
   if (window1)
      {
      while(msg=(struct IntuiMessage *)GetMsg(window1->UserPort))
         {
         ReplyMsg(msg);
         }
      CloseWindow(window1);
      }
   if (screen1) CloseScreen(screen1);
   if (pdata)   FreeMem(pdata,PDATASZ);
   if (IntuitionBase) CloseLibrary(IntuitionBase);
   if (GfxBase)       CloseLibrary(GfxBase);
   if (newLock != startLock)  CurrentDir(startLock);
   }


strlen(s)
char *s;
   {
   int i = 0;
   while(*s++) i++;
   return(i);
   }



/** getBitMap() *********************************************************
 *
 * Open screen or temp bitmap.
 *   Returns ptr destBitMap  or  0 = error
 *
 *************************************************************************/
struct BitMap *getBitMap(ptilbmFrame)
   ILBMFrame *ptilbmFrame;
   {
   int     i, nPlanes, plsize;
   SHORT  sWidth, sHeight, dWidth, dHeight;
   struct BitMap *destBitMap;

   sWidth  = ptilbmFrame->bmHdr.w;
   sHeight = ptilbmFrame->bmHdr.h;
   dWidth  = ptilbmFrame->bmHdr.pageWidth;
   dHeight = ptilbmFrame->bmHdr.pageHeight;
   nPlanes = MIN(ptilbmFrame->bmHdr.nPlanes, EXDepth);

   ns.Width  = dWidth;
   ns.Height = dHeight;
   ns.Depth  = nPlanes;

   if (ptilbmFrame->foundCAMG)
      {
      ns.ViewModes = ptilbmFrame->camgChunk.ViewModes & CAMGMASK;
      }
   else
      {
      if (ptilbmFrame->bmHdr.pageWidth >= 640)
         ns.ViewModes = HIRES;
      else
         ns.ViewModes = 0;

      if (ptilbmFrame->bmHdr.pageHeight >= 400)
         ns.ViewModes |= LACE;

      /* EHB is kludgey flag for ExtraHalbrite ILBMs with no CAMG */
      if(ns.Depth == 6)
         {
         if(EHB) ns.ViewModes |= EXTRA_HALFBRITE;
         else ns.ViewModes |= HAM;
         }
      }
   

   if(Back) ns.Type |= SCREENBEHIND;
   
   if ((screen1 = (struct Screen *)OpenScreen(&ns))==NULL)    return(0);

   vport1 = &screen1->ViewPort;
   LoadRGB4(vport1, &allBlack[0], MIN(1<<ns.Depth,maxColorReg));

   if((ns.ViewModes)&(HAM))  setHam(screen1,FALSE);

   nw.Width  = dWidth;
   nw.Height = dHeight;
   nw.Screen = screen1;

   if(!Back)  nw.Flags |= ACTIVATE;

   if ((window1 = (struct Window *)OpenWindow(&nw))==NULL)
      {
      CloseScreen(screen1);
      screen1 = NULL;
      return(0);
      }

   ShowTitle(screen1, FALSE);

   if ((sWidth == dWidth) && (sHeight == dHeight))
      {
      destBitMap = (struct BitMap *)screen1->RastPort.BitMap;
      }
   else
      {
      InitBitMap( &tBitMap,
                  nPlanes,
                  sWidth,
                  sHeight);

      plsize = RowBytes(ptilbmFrame->bmHdr.w) * ptilbmFrame->bmHdr.h;
      if (tBitMap.Planes[0] =
       (PLANEPTR)AllocMem(nPlanes * plsize, MEMF_CHIP))
         {
         for (i = 1; i < nPlanes; i++)
            tBitMap.Planes[i] = (PLANEPTR)tBitMap.Planes[0] + plsize*i;
         destBitMap = &tBitMap;
         }
      else
         {
         CloseWindow(window1);
         window1 = NULL;
         CloseScreen(screen1);
         screen1 = NULL;
         return(0);  /* can't allocate temp BitMap */
         }
      }
   return(destBitMap);          /* destBitMap allocated */
   }


/** DisplayPic() *********************************************************
 *
 * Display loaded bitmap.  If tBitMap, first transfer to screen.
 *
 *************************************************************************/
DisplayPic(ptilbmFrame)
   ILBMFrame *ptilbmFrame;
   {
   int    i, row, byte, nrows, nbytes;
   struct BitMap  *tbp, *sbp; /* temp and screen BitMap ptrs */
   UBYTE  *tpp, *spp;         /* temp and screen plane ptrs  */

   if (tBitMap.Planes[0])     /* transfer from tBitMap if nec. */
      {
      tbp = &tBitMap;
      sbp = screen1->RastPort.BitMap;
      nrows  = MIN(tbp->Rows, sbp->Rows);
      nbytes = MIN(tbp->BytesPerRow, sbp->BytesPerRow);

      for (i = 0; i < sbp->Depth; i++)
         {
         tpp = (UBYTE *)tbp->Planes[i];
         spp = (UBYTE *)sbp->Planes[i];
         for (row = 0; row < nrows; row++)
            {
            tpp = tbp->Planes[i] + (row * tbp->BytesPerRow);
            spp = sbp->Planes[i] + (row * sbp->BytesPerRow);
            for (byte = 0; byte < nbytes; byte++)
               {
               *spp++ = *tpp++;
               }
            }
         }
      /*  Can now deallocate the temp BitMap  */
      FreeMem(tBitMap.Planes[0],
                 tBitMap.BytesPerRow * tBitMap.Rows * tBitMap.Depth);
      }

   vport1 = &screen1->ViewPort;
   LoadRGB4(vport1, ptilbmFrame->colorMap, ptilbmFrame->nColorRegs);
   if((ns.ViewModes)&(HAM))  setHam(screen1,TRUE);

   return(0);
   }


/* setHam --- For toggling HAM so HAM pic invisible while loading */
setHam(scr,toggle)
struct Screen *scr;
BOOL   toggle;
   {
   struct ViewPort *vp;
   struct View     *v;

   vp = &(scr->ViewPort);
   v = (struct View *)ViewAddress();
   Forbid();
   if(toggle)
      {
      v->Modes  |= HAM;
      vp->Modes |= HAM;
      }
   else
      {
      v->Modes  &= ~HAM;
      vp->Modes &= ~HAM;
      }
   MakeScreen(scr);
   RethinkDisplay();
   Permit();
   }

strEqu(p, q) 
TEXT *p, *q; 
   { 
   while(TOUPPER(*p) == TOUPPER(*q))
      {
      if (*(p++) == 0)  return(TRUE);
      ++q; 
      }
   return(FALSE);
   } 

/* wbStdio.c --- Open an Amiga stdio window under workbench
 *               For use with AStartup.obj
 */

openStdio(conspec)
char *conspec;
   {
   LONG wfile;
   struct Process *proc;
   struct FileHandle *handle;

   if (wbHasStdio)  return(1);

   if (!(wfile = Open(conspec,MODE_NEWFILE)))  return(0);
   stdin  = wfile;
   stdout = wfile;
   stderr = wfile;
   handle = (struct FileHandle *)(wfile << 2);
   proc = (struct Process *)FindTask(NULL);

   proc->pr_ConsoleTask = (APTR)(handle->fh_Type);
   proc->pr_CIS = (BPTR)stdin;
   proc->pr_COS = (BPTR)stdout;
   return(1);
   }

closeStdio()
   {
   struct Process *proc;
   struct FileHandle *handle;

   if (! wbHasStdio) return(0);

   if (stdin > 0)  Close(stdin);
   stdin  = -1;
   stdout = -1;
   stderr = -1;
   handle = (struct FileHandle *)(stdin << 2);
   proc = (struct Process *)FindTask(NULL);
   proc->pr_ConsoleTask = NULL;
   proc->pr_CIS = NULL;
   proc->pr_COS = NULL;
   wbHasStdio = NULL;
   }
