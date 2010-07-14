/*--------------------------------------------------------------*/
/* ilbm2raw.c							*/	
/* 		    2/4/86					*/
/* Reads in ILBM, outputs raw format, which is			*/
/* just the planes of bitmap data followed by the color map	*/
/*                                                              */
/* By Jerry Morrison and Steve Shaw, Electronic Arts.           */
/* This software is in the public domain.                       */
/*                                                              */
/* This version for the Commodore-Amiga computer.               */
/*                                                              */
/*	Callable from CLI only					*/
/*--------------------------------------------------------------*/

#include "iff/intuall.h"
#include "libraries/dos.h"
#include "libraries/dosextens.h"
#include "iff/ilbm.h"
#include "iff/readpict.h"
#include "iff/remalloc.h"

#undef NULL
#include "lattice/stdio.h"
/*----------------------------------------------------------------------*/
/*	Iff error messages						*/
/*----------------------------------------------------------------------*/

char MsgOkay[] = { "----- (IFF_OKAY) A good IFF file." };
char MsgEndMark[] = {"----- (END_MARK) How did you get this message??" };
char MsgDone[] = { "----- (IFF_DONE) How did you get this message??" };
char MsgDos[] = { "----- (DOS_ERROR) The DOS gave back an error." };
char MsgNot[] = { "----- (NOT_IFF) not an IFF file." };
char MsgNoFile[] = { "----- (NO_FILE) no such file found." };
char MsgClientError[] = {"----- (CLIENT_ERROR) IFF Checker bug."};
char MsgForm[] = { "----- (BAD_FORM) How did you get this message??" };
char MsgShort[] = { "----- (SHORT_CHUNK) How did you get this message??" };
char MsgBad[] = { "----- (BAD_IFF) a mangled IFF file." };

/* MUST GET THESE IN RIGHT ORDER!!*/
char *IFFPMessages[-LAST_ERROR+1] = {
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

LONG GfxBase;

/*--------------------------------------------------------------*/

SaveBitMap(name,bm,cols)
    UBYTE *name;	
    struct BitMap *bm;
    SHORT *cols;
    {
    SHORT i;
    LONG nb,plsize;
    LONG file = Open( name, MODE_NEWFILE);
    if( file == 0 )  {
	printf(" couldn't open %s \n",name);
	return (-1);	/* couldnt open a load-file */	
	}
    plsize = bm->BytesPerRow*bm->Rows;
    for (i=0; i<bm->Depth; i++) {
	nb =  Write(file, bm->Planes[i], plsize);
	if (nb<plsize) break;
	}
    Write(file, cols, (1<<bm->Depth)*2);	/* save color map */
    Close(file);
    return(0);
    }

struct BitMap bitmap = {0};

char depthString[] = "0";	/* Replaced with desired digit below.*/

ILBMFrame ilbmFrame;	/* Top level "client frame".*/

/** main() ******************************************************************/

UBYTE defSwitch[] = "b";

void main(argc, argv)  int argc;  char **argv;  {
    LONG iffp, file;
    UBYTE fname[40];
    GfxBase = (LONG)OpenLibrary("graphics.library",0);
    if (GfxBase==NULL) exit(0);
    
    if (argc) {
	/* Invoked via CLI.  Make a lock for current directory. */
	if (argc < 2) {
	    printf("Usage from CLI: 'ilbm2raw filename '\n");
	    }
	else {
	    
	    file = Open(argv[1], MODE_OLDFILE);
	    
	    if (file) {
		iffp = ReadPicture(file, &bitmap, &ilbmFrame, ChipAlloc);
		Close(file);
		if (iffp != IFF_DONE) {
		    printf(" Couldn't read file %s \n", argv[1]);
		    printf("%s\n",IFFPMessages[-iffp]);
		    }
		else {
		    
		    strcpy(fname,argv[1]);

		    if (ilbmFrame.bmHdr.pageWidth > 320) {
			if (ilbmFrame.bmHdr.pageHeight > 200)
			     strcat(fname, ".hi");
			else strcat(fname, ".me");
			}
		    else     strcat(fname, ".lo");

		    depthString[0] = '0' + bitmap.Depth;
		    strcat(fname, depthString);

		    printf(" Creating file %s \n", fname);
		    SaveBitMap(fname, &bitmap, ilbmFrame.colorMap);
		    }
		}
	    else printf(" Couldn't open file: %s. \n", argv[1]);

	    if (bitmap.Planes[0])  RemFree(bitmap.Planes[0]);

	    printf("\n");
	    }
	}
    CloseLibrary(GfxBase);
    exit(0);
    }

