/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* |_o_o|\\   The Software Distillery					     */
/* |. o.| ||  Made available for the Amiga development community	     */
/* | .	| ||   author:						   BBS:      */
/* | o	| ||   John Mainwaring				     (919)-471-6436  */
/* |  . |//								     */
/* ======								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* global definitions for traceback dump utility */

#include "exec/types.h"
#include "exec/memory.h"
#include "proto/exec.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define FATAL 20

/* bit flags for dump options */
#define SYMFLG		1<<0
#define FAILFLG 	1<<1
#define REGFLG		1<<2
#define ENVFLG		1<<3
#define STAKFLG 	1<<4
#define UDATFLG 	1<<5
#define FMEMFLG 	1<<6
#define TRACEFLG	1<<7

struct symbol_node {
   struct symbol_node * sn_next;
   long sn_memsize;
   ULONG sn_value;
   char sn_sym[4]; /* real length determined when allocated */
   };

struct line_elem {
   ULONG le_line;
   ULONG le_off;
   };

struct line_node {
   struct line_node * ln_next;
   ULONG ln_size;	/* byte size of this block			*/
   ULONG ln_offset;	/* offset into segment of this object file	*/
   ULONG ln_nsize;	/* length of name (in longwords)                */
   char  ln_name[4];	/* name of object file lines belong to		*/
			/* a table of line_elem comes after full name	*/
   };

/* element of table of seglist descriptors */
struct segment {
   long addr;
   long size;
   struct symbol_node *symbols;
   struct line_node *lines;
   };

/* element of UDAT chain */
struct udata {
   struct udata *udptr;
   long udsize;
   long udat[1];  /* actual length of array given by udsize */
   };

/* data structure to hold contents of PGTB traceback file */
struct tbtemplate {
   /* FAIL stuff */
   long  gotfail;	/* found FAIL chunk		*/
   char *taskname;	/* name from task block 	*/
   ULONG environ,	/* H/W environment		*/
	 vbfreq,	/* Vertical Blank		*/
	 psfreq,	/* Power Supply 		*/
	 starter,	/* 0 = WB else CLI		*/
	 guru,		/* defined in alerts.h		*/
	 segcount;	/* longword count		*/
   struct segment *segments; /* seglist 		*/
   /* REGS stuff */
   long  gotregs;	/* found REGS chunk		*/
   ULONG pc,		/* program counter		*/
	 cc,		/* condition code reg		*/
	 dregs[8],	/* D0-D7			*/
	 aregs[8];	/* A0-A7			*/
   /* VERS stuff */
   long  gotvers;	/* found VERS chunk		*/
   ULONG ver,		/* version of catch.o		*/
	 rev;		/* revision of catch.o		*/
   char *filename;	/* name of catch.o		*/
   /* FMEM stuff */
   long  gotfmem;	/* got FMEM chunk		*/
   ULONG memca, 	/* available chip		*/
	 memcm, 	/* max chip			*/
	 memcl, 	/* largest chip 		*/
	 memfa, 	/* available fast		*/
	 memfm, 	/* max fast			*/
	 memfl; 	/* largest fast 		*/
   /* STAK stuff (pointer to data chain) */
   ULONG staktop,	/* top of stack 		*/
	 stakptr,/* saved stack pointer 		*/
	 staklen,	/* bottom of stack		*/
	 topseg,	/* bool top present		*/
	 botseg,	/* bool bot present		*/
	 seglen,	/* else entire size*/
	 stak[2048];	/* stack data, 8K bytes 	*/
   /* UDAT stuff */
   struct udata *udhead;
   };

struct addrinfo {
   long hunknum;
   long offset;
   char *name;
   char *objname;
   long line;
   long lineoff;
   };

/* templates for functions called from outside defining section */

/* defined in tb.c */
int tidyproc(int);

/* defined in tbrutil.c */
long getlong(FILE *);
long forcegetlong(FILE *);
void getblock(FILE *, ULONG *, long);
ULONG getascii(FILE *, char **);
void skiplong(FILE *, long);

/* defined in tbread.h */
int  tbread(FILE *);

/* defined in tbsym.h */
int  readsym(FILE *);

/* defined in tbdump.c */
void tbdump(int);

/* defined in tbwutil.c */
void hexdump(FILE *, unsigned char *, long, long);
void longtoascii(ULONG, char *);
int  locaddr(ULONG, struct addrinfo *);
