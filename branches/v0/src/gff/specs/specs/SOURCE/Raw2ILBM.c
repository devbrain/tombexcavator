/** raw2ilbm.c **************************************************************
/*  Read in a "raw" bitmap (dump of the bitplanes in a screen)  */
/*  Display it, and write it out as an ILBM file.		*/
/*  23-Jan-86							*/
/*								*/
/*  Usage from CLI: 'Raw2ILBM  source dest fmt(low,med,hi) 	*/
/*     nplanes'							*/
/*  Supports the three common Amiga screen formats.  		*/
/* 		'low' is 320x200,				*/
/*  		'med' is 640x200, 				*/
/* 		'hi' is 640x400.  				*/
/*		 'nplanes' is the number of bitplanes.		*/
/*  The default is low-resolution, 5 bitplanes 			*/
/* 		(32 colors per pixel).				*/
/*                                                              */
/* By Jerry Morrison and Steve Shaw, Electronic Arts.           */
/* This software is in the public domain.                       */
/*                                                              */
/* This version for the Commodore-Amiga computer.               */
/*                                                              */
/****************************************************************/

#include "iff/intuall.h"
#include "libraries/dos.h"
#include "libraries/dosextens.h"
#include "iff/ilbm.h"
#include "iff/putpict.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

/* general usage pointers */
LONG IconBase;	/* Actually, "struct IconBase *" if you've got some ".h" file*/
struct GfxBase *GfxBase;


/* Globals for displaying an image */
struct RastPort rP;
struct RasInfo rasinfo;
struct View v = {0};
struct ViewPort vp = {0};
struct View *oldView = 0;	/* so we can restore it */

/* ---------------------------------- */
DisplayPic(bm, colorMap) struct BitMap *bm; UWORD *colorMap;  {

    oldView = GfxBase->ActiView;	/* so we can restore it */
	    
    InitView(&v);
    InitVPort(&vp);
    v.ViewPort = &vp;
    InitRastPort(&rP);
    rP.BitMap = bm;
    rasinfo.BitMap = bm;

    /* Always show the upper left-hand corner of this picture. */
    rasinfo.RxOffset = 0;
    rasinfo.RyOffset = 0;

    vp.DWidth = bm->BytesPerRow*8;	/* Physical display WIDTH */
    vp.DHeight = bm->Rows;	/* Display height */

    /* Always display it in upper left corner of screen.*/

    if (vp.DWidth <= 320) vp.Modes = 0;
	else vp.Modes = HIRES;
    if (vp.DHeight > 200) {
	v.Modes |= LACE;
	vp.Modes |= LACE;
	}
    vp.RasInfo = &rasinfo;
    MakeVPort(&v,&vp);
    MrgCop(&v);
    LoadView(&v);	/* show the picture */
    WaitBlit();
    WaitTOF();
    if (colorMap) LoadRGB4(&vp, colorMap,(1 << bm->Depth));
    }

UnDispPict() {
    if (oldView) {
	LoadView(oldView);	/* switch back to old view */
	FreeVPortCopLists(&vp);
	FreeCprList(v.LOFCprList);
	}
    }

PrintS(msg)  char *msg; {   printf(msg);    }

void GoodBye(msg)  char *msg; {   PrintS(msg);   PrintS("\n");   exit(0);   }

struct BitMap bitmap = {0};
SHORT cmap[32];

AllocBitMap(bm) struct BitMap *bm; {
    int i;
    LONG psz = bm->BytesPerRow*bm->Rows;
    UBYTE *p = (UBYTE *)AllocMem(bm->Depth*psz, MEMF_CHIP|MEMF_PUBLIC);
    for (i=0; i<bm->Depth; i++)  { 
	bm->Planes[i] = p;
	p += psz;
	}
    }

FreeBitMap(bm) struct BitMap *bm;  {
    if (bitmap.Planes[0])  {
	FreeMem(bitmap.Planes[0],
		bitmap.BytesPerRow * bitmap.Rows * bitmap.Depth);
	}
    }

BOOL LoadBitMap(file,bm,cols)
    LONG file;	
    struct BitMap *bm;
    SHORT *cols;
    {
    SHORT i;
    LONG nb,plsize;
    plsize = bm->BytesPerRow*bm->Rows;
    for (i=0; i<bm->Depth; i++) {
	nb =  Read(file, bm->Planes[i], plsize);
	if (nb<plsize) BltClear(bm->Planes[i],plsize,1);
	}
    if (cols) {
	nb = Read(file, cols, (1<<bm->Depth)*2); 	/* load color map */
	return( (BOOL) (nb == (1<<bm->Depth)*2) );
	}
    return((BOOL) FALSE);
    }


/** main() ******************************************************************/

UBYTE defSwitch[] = "b";
	
#define BUFSIZE  16000

static SHORT maxDepth[3] = {5,4,4};

void main(argc, argv)  int argc;  char **argv;  {
    SHORT fmt,depth,pwidth,pheight;
    UBYTE *buffer;
    BOOL hadCmap;
    LONG file;
    if( !(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0)) )
	GoodBye("No graphics.library");
    if( !(IconBase = OpenLibrary("icon.library",0)) )
	GoodBye("No icon.library");
    if (argc) {
	if (argc < 3) {
	    printf(
"Usage from CLI: 'Raw2ILBM  source dest fmt(low,med,hi) nplanes'\n");
	    goto bailout;
	    }
	fmt = 0;
	depth = 5;
	if (argc>3)
	    switch(*argv[3]) {
		case 'l': fmt = 0; break;
		case 'm': fmt = 1; break;
		case 'h': fmt = 2; break;
		}
	if (argc>4) depth = *argv[4]-'0';
	depth = MAX(1, MIN(maxDepth[fmt],depth));
	pwidth = fmt? 640: 320;
	pheight = (fmt>1)? 400: 200;
	InitBitMap(&bitmap, depth, pwidth, pheight);
	AllocBitMap(&bitmap);
	
	file = Open(argv[1], MODE_OLDFILE);
	
	if (file)  { 
	    DisplayPic(&bitmap,NULL);
	    hadCmap = LoadBitMap(file,&bitmap, cmap);    
	    if (hadCmap) LoadRGB4(&vp, cmap, 1<<bitmap.Depth);
	    Close(file);
	    file = Open(argv[2], MODE_NEWFILE);
	    buffer = (UBYTE *)AllocMem(BUFSIZE, MEMF_CHIP|MEMF_PUBLIC);
	    PutPict(file, &bitmap, pwidth, pheight,
		hadCmap? cmap: NULL, buffer, BUFSIZE);
	    Close(file);
	    FreeMem(buffer,BUFSIZE);
	    }
	else printf(" Couldn't open file '%s' \n",argv[2]);
	}

    UnDispPict();
    FreeBitMap(&bitmap);
	
    bailout:
	CloseLibrary(GfxBase);
	CloseLibrary(IconBase);
	exit(0);
    }

