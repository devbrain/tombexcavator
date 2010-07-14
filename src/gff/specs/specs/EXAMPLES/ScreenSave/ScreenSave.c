/*
 *  ScreenSave.c --  v1.06 Carolyn Scheppner  CBM
 *                   Saves front screen as ILBM file
 *                   Saves a CAMG chunk for HAM, etc.
 *                   Creates icon for ILBM file
 *  Original 10/86
 *  Modified 9/88 - To mask out unwanted ViewMode bits in CAMG
 *                  and use CAMG defs in new ilbm.h
 *
 *     Uses IFF rtns by J.Morrison and S.Shaw of Electronic Arts
 *
 *       (all C code including IFF modules compiled with -v on LC2)
 * Linkage information:
 * FROM     AStartup.obj, ScreenSave.o, iffw.o, ilbmw.o, packer.o
 * TO       ScreenSave
 * LIBRARY  Amiga.lib, LC.lib
 *
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/gfx.h>
#include <graphics/view.h>

#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>

#include "iff/ilbm.h"

/* From AStartup - used to create stdio on WB startup */
extern  LONG  stdin, stdout, stderr;

/* For masking unwanted Viewmodes bits */
#define BADFLAGS  (SPRITES|VP_HIDE|GENLOCK_AUDIO|GENLOCK_VIDEO)
#define FLAGMASK  (~BADFLAGS)
#define CAMGMASK  (FLAGMASK & 0x0000FFFFL)

/* Other Stuff */

#define bufSize 512

struct IntuitionBase *IntuitionBase;
struct GfxBase       *GfxBase;
ULONG  IconBase;

struct Screen   *frontScreen;

struct ViewPort *picViewPort;
struct BitMap   *picBitMap;
WORD            *picColorTable;
ULONG            picViewModes;
BOOL fromWB, newStdio;

#define INBUFSZ 40
char sbuf[INBUFSZ];
char nbuf[INBUFSZ];

char conSpec[] = "CON:0/40/639/160/ ScreenSave v1.06 ";

/* Definitions for ILBM Icon */
USHORT  ILBMimagedata[] = {
 0xFFFF, 0xFFFC,
 0xC000, 0x000C,
 0xC000, 0x000C,
 0xC1E7, 0x9E0C,
 0xC1F8, 0x7E0C,
 0xC078, 0x780C,
 0xC187, 0x860C,
 0xC078, 0x780C,
 0xC1F8, 0x7E0C,
 0xC1E7, 0x9E0C,
 0xC000, 0x000C,
 0xC000, 0x000C,
 0xFFFF, 0xFFFC,
 0x0000, 0x0000,
 0x0000, 0x0000,
/**/
 0xFFFF, 0xFFFC,
 0xFFFF, 0xFFFC,
 0xF800, 0x007C,
 0xF9E0, 0x1E7C,
 0xF980, 0x067C,
 0xF807, 0x807C,
 0xF81F, 0xE07C,
 0xF807, 0x807C,
 0xF980, 0x067C,
 0xF9E0, 0x1E7C,
 0xF800, 0x007C,
 0xFFFF, 0xFFFC,
 0xFFFF, 0xFFFC,
 0x0000, 0x0000,
 0x0000, 0x0000,
/**/
 };

struct Image ILBMimage = {
   0,0,                     /* Leftedge, Topedge */
   30,15,                   /* Width Height */
   2,                       /* Depth */
   &ILBMimagedata[0],       /* Data for image */
   3,0                      /* PlanePick, PlaneOnOff */
   };

struct DiskObject ILBMobject = {
   WB_DISKMAGIC,
   WB_DISKVERSION,

   /* Gadget Structure */
   NULL,                    /* Ptr to next gadget */
   0,0,                     /* Leftedge, Topedge */
   30,15,                   /* Width, Height */
   GADGHBOX|GADGIMAGE,      /* Flags */
   RELVERIFY|GADGIMMEDIATE, /* Activation */
   BOOLGADGET,              /* Type */
   (APTR)&ILBMimage,        /* Render */
   NULL,                    /* Select Render */
   NULL,                    /* Text */
   NULL,NULL,NULL,NULL,     /* Exclude, Special, ID, UserData */

   4,                       /* WBObject type */
   ":Display",              /* Default tool */
   NULL,                    /* Tool Types */
   NO_ICON_POSITION,        /* Current X */
   NO_ICON_POSITION,        /* Current Y */
   NULL,NULL,NULL,          /* Drawer, ToolWindow, Stack */
   };


main(argc, argv)
int argc;
char **argv;
   {
   LONG            file;
   IFFP            iffp = NO_FILE;
   char            *filename;
   int l;

   newStdio = FALSE;
   fromWB = (argc==0) ? TRUE : FALSE;

   if((fromWB) && (!(newStdio = openStdio(&conSpec[0]))))
      {
      return(0);
      }

   if ((IntuitionBase =
      (struct IntuitionBase *)OpenLibrary("intuition.library",0))==NULL)
         cleanexit("Can't open intuition.library\n");

   if ((GfxBase =
      (struct GfxBase *)OpenLibrary("graphics.library",0))==NULL)
      cleanexit("Can't open graphics.library\n");

   if ((IconBase = OpenLibrary("icon.library",0))==NULL )
      cleanexit("Can't open icon.library\n");

   printf("ScreenSave v 1.06 --- C. Scheppner  CBM  9/88\n");
   printf("   Saves the front screen as an IFF ILBM file\n");
   printf("   A CAMG chunk is saved (for HAM pics, etc.)\n\n");

   if(argc>1)                 /* Passed filename via command line  */
      {
      filename = argv[1];
      }
   else
      {
      printf("Enter filename for save: ");
      l = gets(&nbuf[0]);

      if(l==0)                /* No filename - Exit */
         {
         cleanexit("\nScreen not saved, filename required\n");
         }
      else
         {
         filename = &nbuf[0];
         }
      }

   if (!(file = Open(filename, MODE_NEWFILE)))
      cleanexit("Can't open output file\n");
     
   Write(file,"x",1);  /* 1.1 so Seek to beginning works ? */

   printf("Click here and press <RETURN> when ready: ");
   gets(&sbuf[0]);
   printf("Front screen will be saved in 10 seconds\n");
   Delay(500);

   Forbid();
   frontScreen  = IntuitionBase->FirstScreen;
   Permit();

   picViewPort = &( frontScreen->ViewPort );
   picBitMap = (struct BitMap*)picViewPort->RasInfo->BitMap;
   picColorTable = (WORD *)picViewPort->ColorMap->ColorTable;
   picViewModes = (ULONG)picViewPort->Modes;

   printf("\nSaving...\n");

   iffp = PutPicture(file, picBitMap, picColorTable, picViewModes);
   Close(file);

   if (iffp == IFF_OKAY)
      {
      printf("Screen saved\n");
      if(!(PutDiskObject(filename,&ILBMobject)))
         {
         cleanexit("Error saving icon\n");
         }
      printf("Icon saved\n");
      }
   cleanexit("Done\n");
   }


cleanexit(s)
   char  *s;
   {
   if(*s) printf(s);
   if ((fromWB)&&(*s))    /* Wait so user can read messages */
      {
      printf("\nPRESS RETURN TO EXIT\n");
      gets(&sbuf[0]);
      }
   cleanup();
   exit();
   }

cleanup()
   {
   if (newStdio)  closeStdio();
   if (GfxBase) CloseLibrary(GfxBase);
   if (IntuitionBase) CloseLibrary(IntuitionBase);
   if (IconBase) CloseLibrary(IconBase);
   }


openStdio(conspec)
char *conspec;
   {
   LONG wfile;
   struct Process *proc;
   struct FileHandle *handle;

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

   if (stdin > 0)  Close(stdin);
   stdin  = -1;
   stdout = -1;
   stderr = -1;
   handle = (struct FileHandle *)(stdin << 2);
   proc = (struct Process *)FindTask(NULL);
   proc->pr_ConsoleTask = NULL;
   proc->pr_CIS = NULL;
   proc->pr_COS = NULL;
   }


gets(s)
char *s;
   {
   int l = 0, max = INBUFSZ - 1;

   while (((*s = getchar()) !='\n' )&&(l < max)) s++, l++;
   *s = NULL;
   return(l);
   }


/* String Functions */

strlen(s)
char *s;
   {
   int i = 0;
   while(*s++) i++;
   return(i);
   }

strcpy(to,from)
char *to, *from;
   {
   do
      {
      *to++ = *from;
      }
   while(*from++);
   }


/** PutPicture() ***********************************************************
 *
 * Put a picture into an IFF file.
 * This procedure calls PutAnILBM, passing in an <x, y> location of <0, 0>,
 * a NULL mask, and a locally-allocated buffer. It also assumes you want to
 * write out all the bitplanes in the BitMap.
 *
 ***************************************************************************/
Point2D nullPoint = {0, 0};

IFFP PutPicture(file, bitmap, colorMap, viewmodes)
      LONG file;  struct BitMap *bitmap;
      WORD *colorMap;  ULONG viewmodes;
   {
   BYTE buffer[bufSize];
   return( PutAnILBM(file, bitmap, NULL,
           colorMap, bitmap->Depth, viewmodes,
           &nullPoint, buffer, bufSize) );
   }    

   
/** PutAnILBM() ************************************************************
 *
 * Write an entire BitMap as a FORM ILBM in an IFF file.
 * This version works for any display mode (C. Scheppner).
 *
 * Normal return result is IFF_OKAY.
 *
 * The utility program IFFCheck would print the following outline of the
 * resulting file:
 *
 *   FORM ILBM
 *     BMHD
 *     CAMG
 *     CMAP
 *     BODY       (compressed)
 *
 ***************************************************************************/
#define CkErr(expression)  {if (ifferr == IFF_OKAY) ifferr = (expression);}

IFFP PutAnILBM(file, bitmap, mask, colorMap, depth,
                                viewmodes, xy, buffer, bufsize)
      LONG file;
      struct BitMap *bitmap;
      BYTE *mask;  WORD *colorMap; UBYTE depth;
      ULONG viewmodes;
      Point2D *xy; BYTE *buffer;  LONG bufsize;
   {
   BitMapHeader bmHdr;
   CamgChunk    camgChunk;
   GroupContext fileContext, formContext;
   IFFP ifferr;
   WORD pageWidth, pageHeight;

   pageWidth  = (bitmap->BytesPerRow) << 3;
   pageHeight = bitmap->Rows;

   ifferr = InitBMHdr(&bmHdr, bitmap, mskNone,
                      cmpByteRun1, 0, pageWidth, pageHeight);
   /* You could write an uncompressed image by passing cmpNone instead
    * of cmpByteRun1 to InitBMHdr. */
   bmHdr.nPlanes = depth;   /* This must be  <= bitmap->Depth */
   if (mask != NULL) bmHdr.masking = mskHasMask;
   bmHdr.x = xy->x;   bmHdr.y = xy->y;

   camgChunk.ViewModes = viewmodes & CAMGMASK; /* Mask out unwanted bits! */

   CkErr( OpenWIFF(file, &fileContext, szNotYetKnown) );
   CkErr(StartWGroup(&fileContext, FORM, szNotYetKnown, ID_ILBM, &formContext));

   CkErr( PutBMHD(&formContext, &bmHdr) );
   CkErr( PutCAMG(&formContext, &camgChunk) );
   CkErr( PutCMAP(&formContext, colorMap, depth) );
   CkErr( PutBODY(&formContext, bitmap, mask, &bmHdr, buffer, bufsize) );

   CkErr( EndWGroup(&formContext) );
   CkErr( CloseWGroup(&fileContext) );
   return( ifferr );
   }
   
