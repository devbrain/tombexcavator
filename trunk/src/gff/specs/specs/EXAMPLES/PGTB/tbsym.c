/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* |_o_o|\\  The Software Distillery					     */
/* |. o.| || Made available to the Amiga development community		     */
/* | .	| || the authors:					   BBS:      */
/* | o	| ||   John Mainwaring				     (919)-471-6436  */
/* |  . |//								     */
/* ======								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* this routine reads the program and extracts symbol and debug information  */

#include "tb.h"
#include "setjmp.h"

#define HUNK_UNIT	 999
#define HUNK_NAME	1000
#define HUNK_CODE	1001
#define HUNK_DATA	1002
#define HUNK_BSS	1003
#define HUNK_RELOC32	1004
#define HUNK_RELOC16	1005
#define HUNK_RELOC8	1006
#define HUNK_EXT	1007
#define HUNK_SYMBOL	1008
#define HUNK_DEBUG	1009
#define HUNK_END	1010
#define HUNK_HEADER	1011
#define HUNK_OVERLAY	1013
#define HUNK_BREAK	1014

#define LINE (('L'<<24)|('I'<<16)|('N'<<8)|'E')

extern struct tbtemplate *tbdata;
extern jmp_buf bailout;

long hunkcount;  /* used to match codefile hunks with PGTB file segments */

ULONG readhunk(FILE *);
void skipheaderhunk(FILE *);
void dosymbols(FILE *);
void dolines(FILE *);

int readsym(file)
FILE *file;
{
ULONG thishunk, lasthunk = 0;

if (setjmp(bailout))
   /* get here if file utilities decide to abort quasi gracefully */
   return(0);

hunkcount = -1;  /* so first hunk will be 0 */

while (thishunk = readhunk(file))
   lasthunk = thishunk;

return(lasthunk == HUNK_END);
}

ULONG readhunk(file)
FILE *file;
{
ULONG hunkid, size;

if (!(hunkid = forcegetlong(file)))
   return(0);  /* signal end of file (or confusion?) */

switch (hunkid) {
case HUNK_CODE: 	/* these hunks should match a segment but are	*/
case HUNK_DATA: 	/* otherwise of no interest			*/
   hunkcount += 1;	/* fall through to skip over hunk		*/
case HUNK_UNIT: 	/* these hunks have size and data which is of	*/
case HUNK_NAME: 	/* no interest to us				*/
   skiplong(file, getlong(file));
   break;
case HUNK_BSS:		/* this hunk has only a size			*/
   getlong(file);
   hunkcount += 1;	/* but it does create a segment 		*/
   break;
case HUNK_RELOC32:	/* not interested in relocation info, which is	*/
case HUNK_RELOC16:	/* size, reloc hunk num, and size longwords of	*/
case HUNK_RELOC8:	/* info.  Ends with size of 0.			*/
   while (size = getlong(file))
      skiplong(file, size + 1);
   break;
case HUNK_EXT:		/* should only be found in unlinked *.o files	*/
   fprintf(stderr, "*** encountered HUNK_EXT: I give up\n");
   return(0);
   /* break;  happens, but confuses compiler  */
case HUNK_SYMBOL:	/* Oh joy!  my life is worthwhile		*/
   dosymbols(file);
   break;
case HUNK_DEBUG:	/* My cup overflows				*/
   dolines(file);
   break;
case HUNK_END:		/* end of a group of hunks - no info here	*/
   break;
case HUNK_HEADER:	/* some neat admin stuff we can skip over	*/
   skipheaderhunk(file);
   break;
case HUNK_OVERLAY:	/* bunt on this for now...			*/
   fprintf(stderr, "*** Overlays confuse me, I give up\n");
   return(0);
   /* break;  happens, but confuses compiler  */
case HUNK_BREAK:	/* end of some overlays 			*/
   break;
default:
   fprintf(stderr, "*** illegal hunkid %ld in program file\n", hunkid);
   return(0);  /* caller will give up now */
}
return(hunkid);
}

void skipheaderhunk(file)
/*   --------------*/
FILE *file;
{
ULONG size, first, last;
long i;

/* do the names, which consist of a (possibly empty) list of    */
/* {size, ASCII} terminated by a null size			*/
while (size = getlong(file))
   skiplong(file, size);

getlong(file);          /* table size - who needs it?           */
first = getlong(file);
last = getlong(file);

if (tbdata->segcount != (last - first + 1))
   {
   fprintf(stderr, "*** code file hunk count does not match traceback\n");
   longjmp(bailout, 1);  /* abort */
   }

for (i = 0; i < tbdata->segcount; i++)
   {
   if ((size = getlong(file)) != tbdata->segments[i].size >> 2)
      {
      fprintf(stderr, "*** segment %ld length $%lx != hunk length $%lx\n",
	       i, size, tbdata->segments[i].size >> 2);
      longjmp(bailout, 1);  /* abort */
      }
   }
}

void dosymbols(file)
/*   ---------*/
FILE *file;
{
struct symbol_node *symaddr, *memaddr;
long memsize;
ULONG size;

while (size = getlong(file))
   {
   /* size includes extra word for null to terminate string	*/
   memsize = sizeof(struct symbol_node) + size << 2;
   if (!(memaddr = (struct symbol_node *)AllocMem(memsize, MEMF_CLEAR)))
      {
      fprintf(stderr, "*** memory allocation failed\n");
      exit(FATAL);
      }
   memaddr->sn_memsize = memsize;
   getblock(file, (ULONG *)&(memaddr->sn_sym[0]), size);
   memaddr->sn_value = getlong(file) + tbdata->segments[hunkcount].addr;
   symaddr = (struct symbol_node *)&(tbdata->segments[hunkcount].symbols);
   while (symaddr->sn_next)
      {
      if (symaddr->sn_next->sn_value > memaddr->sn_value)
	 break;
      else if (symaddr->sn_next->sn_value == memaddr->sn_value)
	 /* see if names match - don't discard aliases          */
	 if (strcmp(symaddr->sn_next->sn_sym, memaddr->sn_sym) == 0)
	    /* we gots this one awready - chuck it	*/
	    {
	    FreeMem((char *)memaddr, memaddr->sn_memsize);
	    memaddr = 0;
	    }
	 /* else drop through - might match another alias	*/
      symaddr = symaddr->sn_next;
      }
   if (memaddr) /* we didn't deallocate */
      {
      memaddr->sn_next = symaddr->sn_next;
      symaddr->sn_next = memaddr;
      }
   }
}

void dolines(file)
/*   -------*/
FILE *file;
{
/* fprintf(stderr, "found debug hunk\n");               */
ULONG size, memsize, htype;
char htypes[5];
struct line_node *memaddr, *lineaddr;

size = getlong(file);
memsize = (size << 2) + sizeof(struct line_node) - 16;
if ((memaddr = (struct line_node *)AllocMem(memsize, MEMF_CLEAR)) == 0)
   {
   fprintf(stderr, "*** unable to allocate line memory\n");
   skiplong(file, size);
   return;
   }
memaddr->ln_size = memsize;
memaddr->ln_offset = getlong(file);

if ((htype = getlong(file)) != LINE)
   {
   longtoascii(htype, htypes);
   fprintf(stderr, "*** unknown debug hunk type %lx = %s\n", htype, htypes);
   skiplong(file, size-2);
   FreeMem((char *)memaddr, memsize);
   return;
   }

getblock(file, (ULONG *)&memaddr->ln_nsize, size-2);
lineaddr = (struct line_node *)&(tbdata->segments[hunkcount].lines);
while (lineaddr->ln_next)
   {
   if (lineaddr->ln_next->ln_offset > memaddr->ln_offset)
      break;
   lineaddr = lineaddr->ln_next;
   }
memaddr->ln_next = lineaddr->ln_next;
lineaddr->ln_next = memaddr;
/* for now just skip the debug hunk			*/
/* skiplong(file, getlong(file));                       */
}
