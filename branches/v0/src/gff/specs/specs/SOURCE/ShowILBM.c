/** ShowILBM.c **************************************************************
 *
 * Read an ILBM raster image file and display it.  	24-Jan-86.
 *
 * By Jerry Morrison, Steve Shaw, and Steve Hayes, Electronic Arts.
 * This software is in the public domain.
 *
 * USE THIS AS AN EXAMPLE PROGRAM FOR AN IFF READER.
 *
 * The IFF reader portion is essentially a recursive-descent parser.
 * The display portion is specific to the Commodore Amiga computer.
 *
 * NOTE: This program displays an image, pauses, then exits.
 *
 * Usage from CLI:
 *   showilbm picture1 [picture2] ...
 *
 * Usage from WorkBench:
 * Click on ShowILBM, hold down shift key, click on each picture to show,
 * Double-click on final picture to complete the selection, release the
 * shift key.
 *
 ****************************************************************************/

/* If you are constructing a Makefile, here are the names of the files 
 * that you'll need to compile and link with to use showilbm:

	showilbm.c
	readpict.c
	remalloc.c
	ilbmr.c
	iffr.c
	unpacker.c
	gio.c

	and you'll have to get movmem() from lc.lib

 * robp.
 * ********************************************************************** */


#include "iff/intuall.h"
#include "libraries/dos.h"
#include "libraries/dosextens.h"
#include "iff/ilbm.h"
#include "workbench/workbench.h"
#include "workbench/startup.h"
#include "iff/readpict.h"
#include "iff/remalloc.h"

#define LOCAL static

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

/* general usage pointers */
struct GfxBase *GfxBase;
LONG IconBase;	/* Actually, "struct IconBase *" if you've got some ".h" file*/

/* For displaying an image */
LOCAL struct RastPort rP;
LOCAL struct BitMap bitmap0;
LOCAL struct RasInfo rasinfo;
LOCAL struct View v = {0};
LOCAL struct ViewPort vp = {0};

LOCAL ILBMFrame iFrame;
    
/* Define the size of a temporary buffer used in unscrambling the ILBM rows.*/
#define bufSz 512

/* Message strings for IFFP codes. */
LOCAL char MsgOkay[]        = {
	"(IFF_OKAY) Didn't find a FORM ILBM in the file." };
LOCAL char MsgEndMark[]     = { "(END_MARK) How did you get this message?" };
LOCAL char MsgDone[]        = { "(IFF_DONE) All done."};
LOCAL char MsgDos[]         = { "(DOS_ERROR) The DOS returned an error." };
LOCAL char MsgNot[]         = { "(NOT_IFF) Not an IFF file." };
LOCAL char MsgNoFile[]      = { "(NO_FILE) No such file found." };
LOCAL char MsgClientError[] = {
	"(CLIENT_ERROR) ShowILBM bug or insufficient RAM."};
LOCAL char MsgForm[]        = { "(BAD_FORM) A malformed FORM ILBM." };
LOCAL char MsgShort[]       = { "(SHORT_CHUNK) A malformed FORM ILBM." };
LOCAL char MsgBad[]         = { "(BAD_IFF) A mangled IFF file." };

/* THESE MUST APPEAR IN RIGHT ORDER!! */
LOCAL char *IFFPMessages[-(int)LAST_ERROR+1] = {
    /*IFF_OKAY*/  MsgOkay,
    /*END_MARK*/  MsgEndMark,
    /*IFF_DONE*/  MsgDone,
    /*DOS_ERROR*/ MsgDos,
    /*NOT_IFF*/   MsgNot,
    /*NO_FILE*/   MsgNoFile,
    /*CLIENT_ERROR*/ MsgClientError,
    /*BAD_FORM*/  MsgForm,
    /*SHORT_CHUNK*/  MsgShort,
    /*BAD_IFF*/   MsgBad
    };

/** DisplayPic() ************************************************************
 *
 * Interface to Amiga graphics ROM routines.
 *
 ****************************************************************************/
DisplayPic(bm, ptilbmFrame)
    struct BitMap *bm;  ILBMFrame *ptilbmFrame;  {
    int i;
    struct View *oldView = GfxBase->ActiView;	/* so we can restore it */

    InitView(&v);
    InitVPort(&vp);
    v.ViewPort = &vp;
    InitRastPort(&rP);
    rP.BitMap = bm;
    rasinfo.BitMap = bm;

    /* Always show the upper left-hand corner of this picture. */
    rasinfo.RxOffset = 0;
    rasinfo.RyOffset = 0;

    vp.DWidth = MAX(ptilbmFrame->bmHdr.w, 4*8);
    vp.DHeight = ptilbmFrame->bmHdr.h;

#if 0
    /* Specify where on screen to put the ViewPort. */
    vp.DxOffset = ptilbmFrame->bmHdr.x;
    vp.DyOffset = ptilbmFrame->bmHdr.y;
#else
    /* Always display it in upper left corner of screen.*/
#endif

    if (ptilbmFrame->bmHdr.pageWidth <= 320) 
	vp.Modes = 0;
    else vp.Modes = HIRES;
    if (ptilbmFrame->bmHdr.pageHeight > 200) {
	v.Modes |= LACE;
	vp.Modes |= LACE;
	}
    vp.RasInfo = &rasinfo;
    MakeVPort(&v,&vp);
    MrgCop(&v);
    LoadView(&v);	/* show the picture */
    WaitBlit();
    WaitTOF();
    LoadRGB4(&vp, ptilbmFrame->colorMap, ptilbmFrame->nColorRegs);

    for (i = 0; i < 5*60; ++i)  WaitTOF();	/* Delay 5 seconds. */

    LoadView(oldView);	/* switch back to old view */
    }

/** stuff for main0() *******************************************************/
LOCAL struct WBStartup *wbStartup = 0;	/* 0 unless started from WorkBench.*/

PrintS(msg)  char *msg; {
    if (!wbStartup)  printf(msg);
    }

void GoodBye(msg)  char *msg; {
/*     PrintS(msg);   PrintS("\n");  */
    printf(msg);   printf("\n");    /* If linked with Lstartup.obj and
				     * NOT compiled with -dTINY, this 
				     * outputs the message to the window
				     * that Lattice opens.
				     * ... carolyn.
				     */
    exit(0);
    }

/** OpenArg() ***************************************************************
 *  Given a "workbench argument" (a file reference) and an I/O mode.
 *  It opens the file.
 ****************************************************************************/
LONG OpenArg(wa, openmode)  struct WBArg *wa;   int openmode; {
    LONG olddir;
    LONG file;
    if (wa->wa_Lock)   olddir = CurrentDir(wa->wa_Lock);
    file = Open(wa->wa_Name, openmode);
    if (wa->wa_Lock)   CurrentDir(olddir);
    return(file);
    }

/** main0() *****************************************************************/
void main0(wa)  struct WBArg *wa;  {
    LONG file;
    IFFP iffp = NO_FILE;

    /* load and display the picture */
    file = OpenArg(wa, MODE_OLDFILE);
    if (file)
	iffp = ReadPicture(file, &bitmap0, &iFrame, ChipAlloc);
	/* Allocates BitMap using ChipAlloc().*/
    Close(file);
    if (iffp == IFF_DONE)
	DisplayPic(&bitmap0, &iFrame);

/*     PrintS(" ");   PrintS(IFFPMessages[-iffp]);   PrintS("\n");   */
     printf(" ");   printf(IFFPMessages[-iffp]);   printf("\n");  
	/* see note near definition of PrintS */

    /* cleanup */
    if (bitmap0.Planes[0])  {
	RemFree(bitmap0.Planes[0]);
		/* ASSUMES allocated all planes via a single ChipAlloc call.*/
	FreeVPortCopLists(&vp);
	FreeCprList(v.LOFCprList);
	}
    }

extern struct WBStartup *WBenchMsg;	/* added: Carolyn Scheppner */

/** main() ******************************************************************/

void main(argc, argv)  int argc;  char **argv;  {
    struct WBArg wbArg, *wbArgs;
    LONG olddir;
/*sss    struct Process *myProcess; */

    if( !(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0)) )
	GoodBye("No graphics.library");
    if( !(IconBase = OpenLibrary("icon.library",0)) )
	GoodBye("No icon.library");
    if (!argc) {
	/* Invoked via workbench */
/* 	wbStartup = (struct WBStartup *)argv;   */
	wbStartup = WBenchMsg;	/* modified by Carolyn Scheppner */
	wbArgs = wbStartup->sm_ArgList;
	argc = wbStartup->sm_NumArgs;
	while (argc >= 2) {
	    olddir = CurrentDir(wbArgs[1].wa_Lock);
	    main0(&wbArgs[1]);
	    argc--;   wbArgs = &wbArgs[1];
	    }
#if 0
	/* [TBD] We want to get an error msg to the Workbench user... */
	if (argc < 2) {
	    printf ("Usage from workbench:\n");
	    printf (" Click mouse on Show-ILBM, Then hold 'SHIFT' key\n");

	/* BOTH OF THESE WERE "PrintS", see note near PrintS definition */

	    GoodBye(" while double-click on file to display.");
	    }
#endif
	}
    else {
	/* Invoked via CLI.  Make a lock for current directory.
	 * Eventually, scan name, separate out directory reference?*/
	if (argc < 2)
	    GoodBye("Usage from CLI: 'Show-ILBM filename'");
/*sss	myProcess = (struct Process *)FindTask(0); */
	wbArg.wa_Lock = 0; /*sss myProcess->pr_CurrentDir; */
	while (argc >= 2) {
	    wbArg.wa_Name = argv[1];
	    printf("Showing file ");   printf(wbArg.wa_Name);   printf(" ...");
	/* THESE WERE "PrintS", see note near PrintS definition */
	    main0(&wbArg);
	    printf("\n");
	/* THIS WAS "PrintS", see note near PrintS definition */
	    argc--;   argv = &argv[1];
	    }
	}
    CloseLibrary(GfxBase);
    CloseLibrary(IconBase);
    exit(0);
    }

