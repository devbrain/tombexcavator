/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* |_o_o|\\  The Software Distillery					     */
/* |. o.| || Made available to the Amiga development community		     */
/* | .	| || the authors:					   BBS:      */
/* | o	| ||   John Mainwaring, Jim Cooper		     (919)-471-6436  */
/* |  . |//								     */
/* ======								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* this routine reads the traceback dump file and processes it */

#include "tb.h"
#include "setjmp.h"

#define FORM (('F'<<24)|('O'<<16)|('R'<<8)|'M')
#define PGTB (('P'<<24)|('G'<<16)|('T'<<8)|'B')
#define FAIL (('F'<<24)|('A'<<16)|('I'<<8)|'L')
#define REGS (('R'<<24)|('E'<<16)|('G'<<8)|'S')
#define VERS (('V'<<24)|('E'<<16)|('R'<<8)|'S')
#define STAK (('S'<<24)|('T'<<16)|('A'<<8)|'K')
#define UDAT (('U'<<24)|('D'<<16)|('A'<<8)|'T')
#define FMEM (('F'<<24)|('M'<<16)|('E'<<8)|'M')

extern struct tbtemplate *tbdata;
extern jmp_buf bailout;

void readfail(FILE *, long);
void readregs(FILE *, long);
void readvers(FILE *, long);
void readstak(FILE *, long);
void readudat(FILE *, long);
void readfmem(FILE *, long);

int tbread(infile)
/*  ------*/
FILE *infile;
{
long filesize, sectsize;
ULONG sectid;
char sectstr[5];

if (setjmp(bailout))
   {
   /* get here if any read routine fails a little bit.	It already    */
   /* printed a complaint.  Driver program might be smart enough to   */
   /* continue in some way - or maybe not.  Not our problem.	      */
   return(0);
   }

if (getlong(infile) == FORM) {
  fprintf(stderr, "FORM length = %ld\n", getlong(infile));
} else {
  fseek(infile,0L,0);  /* seek back to beginning */
}

if (getlong(infile) != PGTB)
   {
   fprintf(stderr, "wrong file type\n");
   return(0);
   }

filesize = getlong(infile);

while (filesize > 0)
{
sectid = getlong(infile);
sectsize = getlong(infile);
filesize = filesize - sectsize - 2;

switch (sectid)
   {
   case FAIL:
      readfail(infile, sectsize);
      break;
   case REGS:
      readregs(infile, sectsize);
      break;
   case VERS:
      readvers(infile, sectsize);
      break;
   case STAK:
      readstak(infile, sectsize);
      break;
   case UDAT:
      readudat(infile, sectsize);
      break;
   case FMEM:
      readfmem(infile, sectsize);
      break;
   default:
      longtoascii(sectid, sectstr);
      fprintf(stderr, "*** unknown section type %lx = %s size = %ld\n",
	      sectid, sectstr, sectsize);
      skiplong(infile, sectsize);
/*    return(0);*/ /*not willing to guess how to recover from this */
   }

}
return(1);  /* success */
}

void readvers(file, size)
/*   --------	       */
FILE *file;
long size;
{
long nlen;

tbdata->ver = getlong(file);
tbdata->rev = getlong(file);
nlen = getascii(file, &(tbdata->filename));

if (nlen != (size - 3))
   {
   fprintf(stderr, "incorrect VERS block format\n");
   skiplong(file, size - 3);
   }
tbdata->gotvers = 1;
}

void readregs(file, size)
/*   --------	       */
FILE *file;
long size;

{
if (size != 18)
   {
   fprintf(stderr, "incorrect REGS block format\n");
   skiplong(file, size);
   return;
   }
getblock(file, &(tbdata->pc), size);
tbdata->gotregs = 1;
}

void readfail(file, size)
/*   --------	       */
FILE *file;
long size;
{
ULONG nlen;
long i;

size -= 7; /* fixed components in this data element */

nlen = getascii(file, &(tbdata->taskname));
size -= nlen; /* some housekeeeping */

/* the following is replaced by a block fetch
tbdata->environ = getlong(file);
tbdata->vbfreq = getlong(file);
tbdata->psfreq = getlong(file);
tbdata->starter = getlong(file);
tbdata->guru = getlong(file);
tbdata->segcount = getlong(file);
*/
getblock(file, &(tbdata->environ), 6);

if (size != (2 * tbdata->segcount)) /* 2 longwords for each seg descriptor */
   {
   fprintf(stderr, "*** invalid FAIL data element size\n");
   skiplong(file, size);
   return;
   }

if (!((char *)tbdata->segments =
       AllocMem(tbdata->segcount * sizeof(struct segment), MEMF_CLEAR)))
   {
   fprintf(stderr, "*** unable to allocate memory\n");
   exit(FATAL);
   }

/* adjust seglist stuff to give address and size of memory used by program */
for (i = 0; i < tbdata->segcount; i++)
   {
   tbdata->segments[i].addr = getlong(file) + 4;
   tbdata->segments[i].size = getlong(file) - 8;
   }
tbdata->gotfail = 1;
}

void readstak(file, size)
/*   --------*/
FILE *file;
long size;
{
ULONG type;
long formerr = 0;
ULONG *sdptr;

type = getlong(file);
size -= 1;

switch (type)
{
case 0: {  /* stack parms */
   if (size != 3)
      formerr = 1;
   else
      sdptr = &(tbdata->staktop);
   break; }
case 1: {  /* entire stack */
   if (size > 2048)
      formerr = 1;
   else {
      tbdata->topseg = tbdata->botseg = 0;
      tbdata->seglen = size;
      sdptr = tbdata->stak; }
   break;}
case 2: {  /* top 4k of stack */
   if (size != 1024)
      formerr = 1;
   else {
      tbdata->topseg = 1;
      tbdata->seglen = 0;
      sdptr = tbdata->stak; }
   break; }
case 3: {  /* bottom 4k of stack */
   if (size != 1024)
      formerr = 1;
   else {
      tbdata->botseg = 1;
      tbdata->seglen = 0;
      sdptr = &(tbdata->stak[1024]); }
   break; }
default:
   formerr = 1;
}

if (formerr)
   {
   fprintf(stderr, "*** invalid STAK block format, type %ld, size %ld\n",
	  type, size);
   skiplong(file, size);
   return;
   }

getblock(file, sdptr, size);
}

void readudat(file, size)
/*   --------*/
FILE *file;
long size;
{
struct udata *next = tbdata->udhead, *this = (struct udata *)&(tbdata->udhead);

while (next)
   {
   this = next;
   next = next->udptr;
   }

if (!(this->udptr = (struct udata *)AllocMem((size << 2) + 8, MEMF_CLEAR)))
   {
   fprintf(stderr, "***unable to allocate memory\n");
   exit(FATAL);
   }

this->udptr->udsize = size;
getblock(file, &(this->udptr->udat[0]), size);
}

void readfmem(file, size)
/*   --------	       */
FILE *file;
long size;
{
if (size != 6)
   {
   fprintf(stderr, "incorrect FMEM block format\n");
   skiplong(file, size);
   return;
   }
getblock(file, &(tbdata->memca), size);
tbdata->gotfmem = 1;
}
