
/*  GrabAnim.c
 * 
 * This program was created from IFFEncode which was obtained
 * from Fish disk #38.
 *
 * Grabanim permits sequences of graphic screens to be grabbed (the highest
 * screen is used) and output to disk in IFF ANIM format.  GrabANIM is designed
 * to run as a task on the Amiga and to "grab" graphic screens and add them
 * to an open ANIM file.  GrabANIM may be controlled either from the CLI or
 * with a controller inserted into the second game port.  Most controllers
 * in use on the Amiga will work including another Amiga mouse, a joystick,
 * or even a Koala pad (don't laugh, it has good quality buttons).  One
 * criteria of a controller are good buttons, since I didn't provide any
 * de-bouncing code.  Old Atari joysticks will usually not work well, giving
 * multiple reports for each try.  The nicest controller is to buy or build
 * a box, with good buttons, that simply provides button input to the up, down,
 * left, and right joystick directions.  Along with this, three more buttons
 * may be added for the left, middle, and right mouse buttons for a total
 * of seven (7) buttons of control off one game port (yes, the Amiga operating
 * system supports a middle mouse button even though there isn't one 
 * available at present).  When using such a controller, one can press the
 * correct button whenever he wants to "grab" the current graphic screen to
 * an open ANIM file.  Just follow the prompts, it isn't bad.
 *
 * To use GrabANIM, simply open a new CLI screen (via newcli, newshell, etc.)
 * and enter the command "grabanim".  One can then select the method of control
 * by responding to interactive prompts on GrabANIMs CLI screen.
 *
 * In addition, two methods are provided for determining which bitmap to grab.
 * Each may work best in a given situation.  The second one was originally
 * added as the first would not work with the player for Juggler.
 *
 * This version only saves ANIMs in the RIFF style compression technique.
 * The enclosed code (skip.c) for compressing was written and placed
 * in the public domain by Jim Kent.
 *
 * CAUTION!! GrabAnim ASSUMES you will be collecting screens with the same
 * size, depth, colors, etc.  GrabAnim may cause nice fireworks if these
 * assumptions are violated!!
 *
 * GrabAnim was created (from iffencode)
 *    by Gary Bonham, SPARTA, Inc.,
 *    Laguna Hills, Calif.
 * Screen-Finding routine taken from Carolyn's Printer Dump source.
 *
 * The code was stripped of excess stuff in Feb 88 for release as freely
 * distributable software.  The intent was to provide an example of ANIM
 * compression code for other developers.  Since there were a number of
 * different compression techniques in GrabANIM prior to this purge, there
 * will be remenants of this code which inadvertently was not removed.
 * The result, however, is a functional piece of software.
 *
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/gfx.h>
#include "exec/devices.h"
#include "devices/gameport.h"
#include "devices/inputevent.h"
#include <graphics/view.h>
#include "functions.h"

#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>

typedef struct IntuitionBase  IBASE;
typedef struct GfxBase        GFXBASE;
typedef struct Window         WIN;
typedef struct Screen         SCR;
typedef struct ViewPort       VP;
typedef struct RastPort       RP;
typedef struct ColorMap       CM;
typedef struct BitMap         BM;

IBASE    *IntuitionBase;
GFXBASE  *GfxBase;
struct BitMap *mybitmap[4];
struct BitMap bitmap2,bitmap3,bitmap4;
int Pop,datasize;
LONG *DeltaData,mxDeltaData,nDeltaData;
int SignalX,ActiveW;
 
struct InputEvent *gamedata;
struct IOStdReq *gameiomsg;
struct MsgPort *gamemsgport;
BYTE buffer[sizeof(struct InputEvent)];
BYTE *buff;
static long rastersize;
RP *rp;
 
/* ================================================ */

main(ac, av)
char *av[];
{
   char notdone = 1;
   char buf[80];
   char flags = 0;
   int fileopen,i,mouse,datamode,frameno;
   int forbidpermit;
   VP *vp;
   CM *cm;

   forbidpermit = 0;
   SignalX = 1; /* default to a visible signal after frame is recorded */
   if ((IntuitionBase=(IBASE *)OpenLibrary("intuition.library",0L)) == NULL)
      cleanexit("Can't open Intuition.");
   if ((GfxBase=(GFXBASE *)OpenLibrary("graphics.library",0L)) == NULL)
      cleanexit("Can't open Graphics.");

   puts ("Animation file screen grab program - ver. 2.00\n");

   ActiveW = 0;
   DeltaData = 0;
   datamode = 0;
   datasize = 2;
   gamemsgport = 0;
   gameiomsg = 0;
   fileopen = 0;
   Pop = 0;
   while(notdone) {
      puts("SELECT:   (J)oystick          (O)pen anim");
      puts("          (M)ouse             (A)dd to anim");
      puts("          (K)oala pad         (C)lose anim");
      puts("          (P) to turn off multicolor signal stripe");
      puts("          ($) to change from top screen to active scr/window");
      puts("          (Q) to quit.");
      mouse = 0;
      switch (getch()) {
      case '$':
         puts("Use Active window for capture");
         ActiveW = 1;
         break;
      case 'P':
         puts("Visible record signal disabled");
         SignalX = 0;
         break;
      case 'K':
         mouse++;  /* double-inc of mouse to =2 for Koala pad buttons */
      case 'M':
         mouse++;  /* single-inc of mouse to =1 for mouse buttons */
      case 'J':
         if (fileopen) {
            printf("Close file, %d frames\n",frameno);
            CloseAnimationFile();
            fileopen = 0;
         }
         puts("Enter file name (blank to abort):"); 
         gets(buf);
         if (buf[0]) {
            if (mouse==1) {
               puts("LH mouse button   - add file");
               puts("RH mouse button   - close file");
            }
            else if (mouse==2) {
               puts("LH Koala button   - add file");
               puts("RH Koala button   - close file");
            }
            else {
               puts("Joystick fire  - add file");
               puts("Joystick up    - close file");
               puts("Joystick left  - forbid");
               puts("Joystick down  - permit");
            }
            buff = &buffer[0];
            if ((gamemsgport = CreatePort(0L,0L)) == 0)
               cleanexit("Error - CreatePort");
            if ((gameiomsg = CreateStdIO(gamemsgport)) == 0)
               cleanexit("Error - CreateStdIO");
            if (OpenDevice("gameport.device",1L,gameiomsg,0L))
               cleanexit("Error - OpenDevice");
            gamedata = (struct InputEvent *)buffer;
            if (SetControllerType((long)GPCT_ABSJOYSTICK))
               cleanexit("Error - SetControllerType");
            if (SetControllerTrigger())
               cleanexit("Error - SetControllerTrigger");
            gameiomsg->io_Command = GPD_READEVENT;
            gameiomsg->io_Data = (APTR)buffer;
            gameiomsg->io_Length = sizeof(struct InputEvent);
            gameiomsg->io_Flags = 0;
            frameno = 0;
            FOREVER {
               SendIO(gameiomsg);
               if (forbidpermit == 0) WaitPort(gamemsgport);
               GetMsg(gamemsgport);

               /* fire button or LH mouse button - add screen to file */
               if (((mouse != 2) && (gamedata->ie_Code == IECODE_LBUTTON))
                  || ((mouse == 2) && (gamedata->ie_X == -1))) {
                  if (fileopen == 0) { /* open anim file */
                     getPointers(&vp, &cm, &rp);
                     mybitmap[0] = rp->BitMap;
                     OpenBitmaps((int)rp->BitMap->BytesPerRow<<3
                                ,(int)rp->BitMap->Rows
                                ,(int)rp->BitMap->Depth);
                     GenViewCopy(0,1);
                     DisplayBeep(0L);
                     DisplayBeep(0L);
                     if (SignalX) { /* multi-color stripe to signal done */
                        for (i=500;i<700;i++) {
                           if (mybitmap[0]->Planes[0])
                              *((LONG *)mybitmap[0]->Planes[0]+i) = 0xCCCCCCCC;
                           if (mybitmap[0]->Planes[1])
                              *((LONG *)mybitmap[0]->Planes[1]+i) = 0xF0F0F0F0;
                           if (mybitmap[0]->Planes[2])
                              *((LONG *)mybitmap[0]->Planes[2]+i) = 0xFF00FF00;
                           if (mybitmap[0]->Planes[3])
                              *((LONG *)mybitmap[0]->Planes[3]+i) = 0xFFFF0000;
                        }
                     }
                     GenViewCopy(1,2);
                     OpenAnimationFile(1,buf,cm,vp);
                     fileopen = 1;
                     frameno++;
                  }
                  else { /* add to anim file */
                     frameno++;
                     getPointers(&vp, &cm, &rp);
                     mybitmap[0] = rp->BitMap;
                     GenViewCopy(2,3);
                     GenViewCopy(1,2);
                     GenViewCopy(0,1);
                     DisplayBeep(0L);
                     DisplayBeep(0L);
                     if (SignalX) {
                        for (i=500;i<700;i++) {
                           if (mybitmap[0]->Planes[0])
                              *((LONG *)mybitmap[0]->Planes[0]+i) = 0xCCCCCCCC;
                           if (mybitmap[0]->Planes[1])
                              *((LONG *)mybitmap[0]->Planes[1]+i) = 0xF0F0F0F0;
                           if (mybitmap[0]->Planes[2])
                              *((LONG *)mybitmap[0]->Planes[2]+i) = 0xFF00FF00;
                           if (mybitmap[0]->Planes[3])
                              *((LONG *)mybitmap[0]->Planes[3]+i) = 0xFFFF0000;
                        }
                     }
                     printf("frame %d\n",frameno);
                     FormRIFF(1,3);
                     AddAnimationFrame2(DeltaData,nDeltaData,5);
                     if (DeltaData) FreeMem(DeltaData,nDeltaData);
                     DeltaData = 0;
                  }
               }
               /* up button or RH mouse button - close file */
               else if (((mouse == 1) && (gamedata->ie_Code == IECODE_RBUTTON))
                     || ((mouse == 0) && (gamedata->ie_Y == -1))
                     || ((mouse == 2) && (gamedata->ie_X ==  1))) {
                  if (fileopen) {
                     printf("Close file, %d frames\n",frameno);
                     CloseAnimationFile();
                     DisplayBeep(0L);
                     DisplayBeep(0L);
                     fileopen = 0;
                  }
                  SetControllerType((long)GPCT_NOCONTROLLER);
                  CloseDevice(gameiomsg);
                  DeleteStdIO(gameiomsg);
                  gameiomsg = 0;
                  DeletePort(gamemsgport);
                  gamemsgport = 0;
                  break;
               }
               /* left joystick button - forbid */
               else if (((mouse == 0) && (gamedata->ie_X == -1))) {
                  if (forbidpermit == 0) {
                     forbidpermit = 1;
                     Forbid();
                     printf("##### FORBID #####\n");
                  }
               }
               /* down joystick button - permit */
               else if (((mouse == 0) && (gamedata->ie_Y == 1))) {
                  if (forbidpermit) {
                     forbidpermit = 0;
                     Permit();
                     printf("##### PERMIT #####\n");
                  }
               }
            }
         }
         break;
      case 'O':
         if (fileopen) {
            printf("Close file, %d frames\n",frameno);
            CloseAnimationFile();
            fileopen = 0;
         }
         puts("Enter file name (blank to abort):"); 
         gets(buf);
         frameno = 1;
         if (buf[0]) {
            do_imopts(&vp, &cm, &rp);
            mybitmap[0] = rp->BitMap;
            OpenBitmaps((int)rp->BitMap->BytesPerRow<<3
                       ,(int)rp->BitMap->Rows
                       ,(int)rp->BitMap->Depth);
            GenViewCopy(0,1);
            DisplayBeep(0L);
            DisplayBeep(0L);
            GenViewCopy(1,2);
            OpenAnimationFile(1,buf,cm,vp);
            fileopen = 1;
            puts("done with frame");
         }
         break;
      case 'A':
         if (fileopen) {
            do_imopts(&vp, &cm, &rp);
            mybitmap[0] = rp->BitMap;
            GenViewCopy(2,3);
            GenViewCopy(1,2);
            GenViewCopy(0,1);
            DisplayBeep(0L);
            DisplayBeep(0L);
            puts("done with frame");
            printf("frame %d\n",frameno);
            FormRIFF(1,3);
            AddAnimationFrame2(DeltaData,nDeltaData,5);
            if (DeltaData) FreeMem(DeltaData,nDeltaData);
            DeltaData = 0;
         }
         break;
      case 'Q':
         notdone = 0;
      case 'C':
         if (fileopen) {
            printf("Close file, %d frames\n",frameno);
            CloseAnimationFile();
            DisplayBeep(0L);
            DisplayBeep(0L);
            fileopen = 0;
         }
         break;
      }
   }
   cleanup();
}

SetControllerType(type)
long type;
{
   gameiomsg->io_Command = GPD_SETCTYPE;
   gameiomsg->io_Length = 1;
   gameiomsg->io_Data = (APTR)buff;
   *buff = type;
   SendIO(gameiomsg);
   WaitPort(gamemsgport);
   GetMsg(gamemsgport);
   return((int)gameiomsg->io_Error);
}

SetControllerTrigger()
{
   struct GamePortTrigger gpt;
   gameiomsg->io_Command = GPD_SETTRIGGER;
   gameiomsg->io_Length = sizeof(gpt);
   gameiomsg->io_Data = (APTR)&gpt;
   gpt.gpt_Keys = GPTF_UPKEYS+GPTF_DOWNKEYS;
   gpt.gpt_Timeout = 0;
   gpt.gpt_XDelta = 1;
   gpt.gpt_YDelta = 1;
   return(DoIO(gameiomsg));
}

GenViewCopy(sbuf,nbuf)
int sbuf,nbuf;
{
   register LONG nl,i,*ss,*dd;
   int ip,Width,Height,Depth;

   Width = mybitmap[sbuf]->BytesPerRow <<3;
   Height = mybitmap[sbuf]->Rows;
   Depth = mybitmap[sbuf]->Depth;

   nl =    ((long)Width * (long)Height) >>5; /* convert bits to longs */
   for (ip=0;ip<Depth;ip++) {
      ss = (LONG *)mybitmap[sbuf]->Planes[ip];
      dd = (LONG *)mybitmap[nbuf]->Planes[ip];

      for (i=0;i<nl;i++) {
         *dd = *ss;
         dd++;
         ss++;
      }
   }
   return(0);
}

FreeBitmaps()
{
   int i;
   if (DeltaData) FreeMem(DeltaData,mxDeltaData);
   for (i=0;i<6;i++) {
      if (bitmap2.Planes[i]) FreeMem(bitmap2.Planes[i],rastersize);
      if (bitmap3.Planes[i]) FreeMem(bitmap3.Planes[i],rastersize);
      if (bitmap4.Planes[i]) FreeMem(bitmap4.Planes[i],rastersize);
   }
   return(0);
}

OpenBitmaps(Width,Height,Depth)
int Width,Height,Depth;
{
   int i;
   for (i=0;i<6;i++) {
      bitmap2.Planes[i] = 0;
      bitmap3.Planes[i] = 0;
      bitmap4.Planes[i] = 0;
   }
   rastersize = ((long)Width*(long)Height)>>3;
   InitBitMap(&bitmap2,(long)Depth,(long)Width,(long)Height);
   for (i=0;i<Depth;i++) {
      bitmap2.Planes[i] = (PLANEPTR)AllocMem(rastersize,(long)MEMF_CLEAR);
      if (bitmap2.Planes[i] == NULL) {
         cleanexit("Not enough room for bitmap2");
      }
   }
   InitBitMap(&bitmap3,(long)Depth,(long)Width,(long)Height);
   for (i=0;i<Depth;i++) {
      bitmap3.Planes[i] = (PLANEPTR)AllocMem(rastersize,(long)(MEMF_CLEAR));
      if (bitmap3.Planes[i] == NULL) {
         cleanexit("Not enough room for bitmap3");
      }
   }
   InitBitMap(&bitmap4,(long)Depth,(long)Width,(long)Height);
   for (i=0;i<Depth;i++) {
      bitmap4.Planes[i] = (PLANEPTR)AllocMem(rastersize,(long)MEMF_CLEAR);
      if (bitmap4.Planes[i] == NULL) {
         cleanexit("Not enough room for bitmap4");
      }
   }
   mybitmap[1] = &bitmap2;
   mybitmap[2] = &bitmap3;
   mybitmap[3] = &bitmap4;
   return(0);
}

do_imopts(r_vp, r_cm, r_rp)
VP **r_vp;
CM **r_cm;
RP **r_rp;
{
   puts("Move Screens so Screen to grab is highest.");
   puts("press <RET> when ready:");
   getch();
   getPointers(r_vp, r_cm, r_rp);
}


getPointers(r_vp, r_cm, r_rp)
VP **r_vp;
CM **r_cm;
RP **r_rp;
{
   SCR *firstscreen, *nextscreen, *highscreen;
   int topEdge;

   Forbid();
   firstscreen  = IntuitionBase->FirstScreen;
   topEdge = 9999;
   nextscreen = highscreen = firstscreen;
   while(nextscreen) {
      if(nextscreen->TopEdge < topEdge) {
         topEdge = nextscreen->TopEdge;
         highscreen = nextscreen;
      }
      nextscreen = nextscreen->NextScreen;
   }
   Permit();

   *r_vp = &(highscreen->ViewPort);
   *r_cm = (*r_vp)->ColorMap;
   if (ActiveW){
      *r_rp = IntuitionBase->ActiveWindow->RPort;
   }
   else {
      *r_rp = &(highscreen->RastPort);
   }
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

cleanexit(errMsg)
char *errMsg;
{
   puts(errMsg);
   cleanup();
   exit(-1);
}

cleanup()
{
   FreeBitmaps();
   if (gameiomsg)         DeleteStdIO(gameiomsg);
   if (gamemsgport)       DeletePort(gamemsgport);
   if (IntuitionBase)     CloseLibrary(IntuitionBase);
   if (GfxBase)           CloseLibrary(GfxBase);
}

/*** FormRIFF(newbuf,oldbuf) ***/

FormRIFF(newbuf,oldbuf)
int newbuf,oldbuf;
{
   unsigned char *outstuff;
   long j,jj[8],nBpP,ib,nb;
   int i;
   int nBpR,nRpP;
   unsigned char *skip_comp_plane();

   nBpR = mybitmap[newbuf]->BytesPerRow;
   nRpP = mybitmap[newbuf]->Rows;
   nBpP = (long)nBpR * (long)nRpP;
   j = 64;
   for (i=0;i<mybitmap[newbuf]->Depth;i++) {
      nb = 0;
      for (ib=0;ib<nBpP;ib++)
         if (*(mybitmap[newbuf]->Planes[i]+ib) !=
             *(mybitmap[oldbuf]->Planes[i]+ib)) nb++;
      if (nb) {
         jj[i] = skip_count_plane(mybitmap[newbuf]->Planes[i]
                             ,mybitmap[oldbuf]->Planes[i]
                             ,nBpR,nRpP);
         j += jj[i];
      }
      else jj[i] = 0;
   }
   if ((DeltaData = AllocMem(j,0L)) == 0) {
      printf("Not enough room for ANIM frame deltas\n");
      return(1);
   }
   outstuff = (unsigned char *)(&DeltaData[16]);
   for (i=0;i<16;i++) DeltaData[i] = 0;
   nDeltaData = 64;
   for (i=0;i<mybitmap[newbuf]->Depth;i++) {
      if (jj[i]) {
         DeltaData[i] = nDeltaData;
         nDeltaData += jj[i];
         outstuff = skip_comp_plane(mybitmap[newbuf]->Planes[i]
                          ,mybitmap[oldbuf]->Planes[i]
                          ,outstuff
                          ,nBpR,nRpP);
      }
      else {
         DeltaData[i] = 0;
      }
   }
   return(0);
}
