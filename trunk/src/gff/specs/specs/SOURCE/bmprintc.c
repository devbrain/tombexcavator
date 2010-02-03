/*--------------------------------------------------------------*/
/*								*/
/*			bmprintc.c				*/
/*								*/
/* print out a C-language representation of data for bitmap	*/
/*								*/ 
/* By Jerry Morrison and Steve Shaw, Electronic Arts.		*/
/* This software is in the public domain.			*/
/*								*/
/* This version for the Commodore-Amiga computer.		*/
/*								*/
/*--------------------------------------------------------------*/

#include <iff/intuall.h>
#undef NULL
#include <lattice/stdio.h>

#define NO 0
#define YES 1

static BOOL doCRLF;

PrCRLF(fp) FILE *fp; {
    if (doCRLF) fprintf(fp,"%c%c",0xD,0xA); else fprintf(fp,"\n");
    }
    
PrintBob(bm, fp, name) 
    struct BitMap *bm; 
    FILE *fp;
    UBYTE *name; 
    {
    UWORD *wp;

    int p,j,nb;
    int nwords = (bm->BytesPerRow/2)*bm->Rows;
    
    fprintf(fp,"/*----- bitmap : w = %ld, h = %ld ------ */",
       bm->BytesPerRow*8, bm->Rows);

    PrCRLF(fp);
    
    for (p=0; p<bm->Depth; ++p) {
	wp = (UWORD *)bm->Planes[p];
	fprintf(fp, "/*------ plane # %ld: --------*/",p);
	PrCRLF(fp);
	fprintf(fp, "UWORD %s%c[%ld] = { ", name, (p?('0'+p):' '), nwords);
	PrCRLF(fp);
	for (j = 0 ;  ; j++) {
	    for (nb = 0; ; ) {
	        fprintf(fp,"  0x%lx", *wp++);
		nb += 2;
		if (nb == bm->BytesPerRow) { 
		    if (j == bm->Rows-1)  goto endplane;
		    else { fprintf(fp,", "); PrCRLF(fp);  break; }
		    }
		else fprintf(fp,", ");
		}
	    }
	endplane: fprintf(fp," };");
	PrCRLF(fp); PrCRLF(fp);
	}
    }

PSprite(bm,fp,name,p,dohead) 
    struct BitMap *bm;
    FILE *fp;
    UBYTE *name;
    int p;
    BOOL dohead;
    {
    UWORD *wp0,*wp1;
    int j,nwords;
    int wpl = bm->BytesPerRow/2;
    nwords =  2*bm->Rows + (dohead?4:0);
    wp0 = (UWORD *)bm->Planes[p];
    wp1 = (UWORD *)bm->Planes[p+1];
    fprintf(fp,"UWORD %s[%ld] = {", name, nwords);
    PrCRLF(fp);
    if (dohead){
	fprintf(fp,"  0x0000, 0x0000, /* VStart, VStop */");
	PrCRLF(fp);
	}
    for (j=0 ; j<bm->Rows; j++) {
	fprintf(fp,"  0x%lx, 0x%lx", *wp0, *wp1);
	if (dohead || (j!=bm->Rows-1)) {
		fprintf(fp, ",");
		PrCRLF(fp);
		}
	wp0 += wpl;
	wp1 += wpl;
	}
    if (dohead) fprintf(fp,"  0x0000, 0x0000 }; /* End of Sprite */");
    else fprintf(fp," };");	
    PrCRLF(fp); PrCRLF(fp);
    }

static UBYTE one[] = "1";

PrintSprite(bm, fp, name, attach, dohdr)
    struct BitMap *bm; FILE *fp;
    UBYTE *name;
    BOOL attach, dohdr;
    {
    fprintf(fp,"/*----- Sprite format: h = %ld ------ */", bm->Rows);
    PrCRLF(fp);
    if (bm->Depth>1) {
	fprintf(fp, "/*--Sprite containing lower order two planes:   */");
        PrCRLF(fp);
	PSprite(bm,fp,name,0,dohdr);
	}
    if (attach && (bm->Depth > 3) ) {
	strcat(name,one);
	fprintf(fp, "/*--Sprite containing higher order two planes:   */");
        PrCRLF(fp);
	PSprite(bm,fp,name,2,dohdr);
	} 
    }

#define BOB 0
#define SPRITE 1
    
BMPrintCRep(bm, fp, name, fmt) 
    struct BitMap *bm; 	/* Contains the image data */
    FILE *fp;		/* file we will write to */
    UBYTE *name;	/* name associated with the bitmap */	
    UBYTE *fmt; 	/* string of characters describing output fmt*/
    {
    BOOL attach, doHdr;
    char c;
    SHORT type;
    doCRLF = NO;
    doHdr = YES;
    type = BOB;
    attach = NO;
    while ( (c=*fmt++) != 0 ) 
	switch (c) {
	    case 'b': type = BOB; break;
	    case 's': type = SPRITE; attach = NO; break;
	    case 'a': type = SPRITE; attach = YES; break;
	    case 'n': doHdr = NO; break;
	    case 'c': doCRLF = YES; break;
	    }
    switch(type) {
	case BOB: PrintBob(bm,fp,name); break;
	case SPRITE:  PrintSprite(bm,fp,name,attach,doHdr); break;
	}
    }
