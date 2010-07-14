/*--------------------------------------------------------------*/
/*								*/
/* ILBMDump.c: reads in ILBM, prints out ascii representation, 	*/
/*  for including in C files. 					*/
/*                                                              */
/* By Jerry Morrison and Steve Shaw, Electronic Arts.           */
/* This software is in the public domain.                       */
/*                                                              */
/* This version for the Commodore-Amiga computer.               */
/*                                                              */
/*  Callable from CLI ONLY					*/
/*  Jan 31, 1986						*/
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

/* this returns a string containing characters after the
	 last '/' or ':' */
GetSuffix(to, fr) UBYTE *to, *fr; {
    int i;
    UBYTE c,*s = fr;
    for (i=0; ;i++) {
	c = *s++;
	if (c == 0) break;
	if (c == '/') fr = s;
	else if (c == ':') fr = s;
	}
    strcpy(to,fr);
    }

LONG GfxBase;
struct BitMap bitmap = {0};

ILBMFrame ilbmFrame;	/* Top level "client frame".*/

/** main() ******************************************************************/

UBYTE defSwitch[] = "b";

void main(argc, argv)  int argc;  char **argv;  {
    UBYTE *sw;
    FILE *fp;
    LONG iffp,file;
    UBYTE name[40], fname[40];
    GfxBase = (LONG)OpenLibrary("graphics.library",0);
    if (GfxBase==NULL) exit(0);
    
    if (argc) {
	/* Invoked via CLI.  Make a lock for current directory. */
	if (argc < 2) {
	    printf("Usage from CLI: 'ILBMDump filename switch-string'\n");
	    printf(" where switch-string = \n");
	    printf("  <nothing> : Bob format (default)\n");
	    printf("  s         : Sprite format (with header and trailer words)\n");
	    printf("  sn        : Sprite format (No header and trailer words)\n");
	    printf("  a         : Attached sprite (with header and trailer)\n");
	    printf("  an        : Attached sprite (No header and trailer)\n");
	    printf(" Add 'c' to switch list to output CR's with LF's   \n");
	    }
	else {
	    sw = (argc>2)? argv[2]: defSwitch;
	    
	    file = Open(argv[1], MODE_OLDFILE);
	    
	    if (file) {
		iffp = ReadPicture(file, &bitmap, &ilbmFrame, ChipAlloc);
		Close(file);
		if (iffp != IFF_DONE) {
		    printf(" Couldn't read file %s \n", argv[1]);
		    printf("%s\n",IFFPMessages[-iffp]);
		    }
		else {
		    printf(" Creating file %s.c \n",argv[1]);
		    GetSuffix(name,argv[1]);
		    strcpy(fname,argv[1]);
		    strcat(fname,".c");
		    fp = fopen(fname,"w");
		    BMPrintCRep(&bitmap,fp,name,sw);
		    fclose(fp);
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

