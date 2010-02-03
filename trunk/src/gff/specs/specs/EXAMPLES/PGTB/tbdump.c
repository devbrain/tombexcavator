/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* |_o_o|\\  The Software Distillery					     */
/* |. o.| || Made available to the Amiga development community		     */
/* | .	| || the authors:					   BBS:      */
/* | o	| ||   John Mainwaring, Jim Cooper		     (919)-471-6436  */
/* |  . |//								     */
/* ======								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* these routines print out the traceback data */

#include "tb.h"
#include "exec/execbase.h"

extern struct tbtemplate *tbdata;

char *desc[] = {
   "???",
   "???",
   "Bus",
   "Address",
   "Illegal Instruction",
   "Divide by Zero",
   "CHK instruction",
   "TRAPV Instruction",
   "Privilege Violation",
   "Trace",
   "Line A Emulation",
   "Line F Emulation",
   "???",
   "???",
   "???",
   "Uninitialized Interrupt Vector",
   "???",
   "???",
   "???",
   "???",
   "???",
   "???",
   "???",
   "???",
   "Spurious Interrupt",
   "Level 1 Interrupt",
   "Level 2 Interrupt",
   "Level 3 Interrupt",
   "Level 4 Interrupt",
   "Level 5 Interrupt",
   "Level 6 Interrupt",
   "Level 7 Interrupt",
   "TRAP #00",
   "TRAP #01",
   "TRAP #02",
   "TRAP #03",
   "TRAP #04",
   "TRAP #05",
   "TRAP #06",
   "TRAP #07",
   "TRAP #08",
   "TRAP #09",
   "TRAP #10",
   "TRAP #11",
   "TRAP #12",
   "TRAP #13",
   "TRAP #14",
   "TRAP #15",
   "???"
};

void dofail(void);
void doregs(void);
void doenv(void);
void dostak(void);
void doudat(void);
void dotrace(void);
void dosymtab(void);
void dofmem(void);

void tbdump(flags)
int flags;
{
if (flags & ENVFLG)
   doenv();
if (flags & SYMFLG)
   dosymtab();
if (flags & FAILFLG)
   dofail();
if (flags & TRACEFLG)
   dotrace();
if (flags & REGFLG)
   doregs();
if (flags & FMEMFLG)
   dofmem();
if (flags & STAKFLG)
   dostak();
if (flags & UDATFLG)
   doudat();
}

void doenv()
/*   -----*/
{
char processortype = {'0'};

if (tbdata->gotvers)
   printf("TraceDump Utility: %s; Version %ld, Revision %ld\n",
	tbdata->filename, tbdata->ver, tbdata->rev);

if (!tbdata->gotfail)
   return;
if (tbdata->environ & AFF_68020)
   processortype = '2';
else if (tbdata->environ & AFF_68010)
   processortype = '1';
printf("Processor type: 680%c%s\n", processortype,
	(tbdata->environ & AFF_68881) ? "0 with 68881" : "0");
printf("VBlankFreq %d, PowerSupFreq %d\n\n", tbdata->vbfreq, tbdata->psfreq);
}

void doregs()
/*   ------*/
{
long i, cc = tbdata->cc;
char *ccbits = "CVZNX";

if (!tbdata->gotregs)
   return;

printf("\nRegisters:\n");

for (i=0; i<8; i++)
   printf("D%d=%08lx%c", i, tbdata->dregs[i], ((i&3)==3?'\n':' '));

for (i=0; i<8; i++)
   printf("A%d=%08lx%c", i, tbdata->aregs[i], ((i&3)==3?'\n':' '));

printf("PC %06lx   ", tbdata->pc);
for (i = 0; i < 5; i++)
   {
   printf("%c=%c ",ccbits[i], cc & 1 ? '1' : '0');
   cc >>= 1;
   }
printf("\n");
}

void dofail()
/*   ------*/
{
long i;
struct addrinfo info;

printf("Program Name: %s; run from %s\n",
	tbdata->taskname, tbdata->starter ? "CLI" : "WorkBench");
printf("Program load map (addresses are APTRs, sizes are in bytes)\n");

for (i = 0; i < tbdata->segcount; i++)
   printf("%06lx $%-5x%c", tbdata->segments[i].addr,
		tbdata->segments[i].size, ((i & 3) == 3) ? '\n' : ' ');
if ((i & 3) != 3) printf("\n"); /* don't leave cursor hanging */

printf("Terminated with GURU number %08lx, %s Error\n",
	tbdata->guru, desc[tbdata->guru]);
if (locaddr(tbdata->pc, &info))
   {
   if (info.name)
      printf("Failed at %lx = %s+$%lx",
	tbdata->pc, info.name, info.offset);
   else
      printf("Failed at %lx, hunk %d+$%lx",
	tbdata->pc, info.hunknum, info.offset);
   if (info.objname)
      printf(", file %s line %ld+$%lx\n",info.objname,info.line,info.lineoff);
   else
      printf("\n");
   }
}

void dostak()
/*   ------*/
{
printf("\nstack top: %lx, stack pointer: %lx, stack length: $%lx\n",
	tbdata->staktop, tbdata->stakptr, tbdata->staklen<<2);

if (tbdata->topseg)
   {
   printf("top 4k segment present\n");
   hexdump(stdout, (unsigned char *)tbdata->stak, 4096, tbdata->stakptr);
   }

if (tbdata->botseg)
   {
   printf("bottom 4k segment present\n");
   hexdump(stdout, (unsigned char *)&tbdata->stak[1024], 4096,
			 tbdata->staktop-4096 /* I hope - could be off */);
   }

if (tbdata->seglen)
   {
   printf("entire stack, size = $%lx bytes\n", tbdata->seglen<<2);
   hexdump(stdout ,(unsigned char *)tbdata->stak, tbdata->staklen<<2,
			 tbdata->stakptr);
   }
}

void doudat()
/*   ------*/
{
struct udata *this = tbdata->udhead;

while (this)
   {
   printf("\nUser data section, size = $%lx bytes\n", this->udsize << 2);
   hexdump(stdout, (unsigned char *)&(this->udat[0]), this->udsize << 2, 0);
   this = this->udptr;
   }
}

void dofmem()
/*   ------*/
{
ULONG ciu, fiu;

if (!tbdata->gotfmem)
   return;
printf("\nMemory Statistics\n      Available    In-Use   Maximum   Largest\n");
printf("chip%11ld%10ld%10ld%10ld\n", tbdata->memca,
  (ciu = tbdata->memcm - tbdata->memca), tbdata->memcm, tbdata->memcl);
printf("fast%11ld%10ld%10ld%10ld\n", tbdata->memfa,
  (fiu = tbdata->memfm - tbdata->memfa), tbdata->memfm, tbdata->memfl);
printf("total%10ld%10ld%10ld%10ld\n", tbdata->memca+tbdata->memfa,
  ciu+fiu, tbdata->memcm+tbdata->memfm, tbdata->memcl+tbdata->memfl);
}

void dotrace()
/*   -------*/
{
ULONG a5;
struct addrinfo info;

if (tbdata->seglen == 0)
   return;	/* only try this trick on entire stack		*/

a5 = tbdata->aregs[5];
while ((a5 < tbdata->staktop) && (a5 >= tbdata->stakptr))
   /* a5 within bounds of stack 				*/
   {
   a5 = (a5 - tbdata->stakptr) >> 2;    /* index to saved stack */
   if (!locaddr(tbdata->stak[a5+1], &info))
      return;	/* doesn't look like a good return address      */

   if (info.name)
      printf("   called from %lx = %s+$%lx",
	  tbdata->stak[a5+1], info.name, info.offset);
   else
      printf("   called from %lx = hunk %d+$%lx",
	  tbdata->stak[a5+1], info.hunknum, info.offset);

   if (info.objname)
      printf(", file %s line %ld+$%lx\n",info.objname,info.line,info.lineoff);
   else
      printf("\n");

   a5 = tbdata->stak[a5];
   }
}

void dosymtab()
/*   --------*/
{
struct symbol_node *symaddr;
long hunkcount, i;

for (hunkcount = 0; hunkcount <tbdata->segcount; hunkcount++)
   {
   symaddr = tbdata->segments[hunkcount].symbols;
   i = 0;
   if (symaddr)
      printf("Symbols for hunk %d\n", hunkcount);
   while (symaddr)
      {
      printf("%20.20s = %6lx%c", symaddr->sn_sym, symaddr->sn_value,
	      (i & 1) == 0 ? ' ' : '\n');
      symaddr = symaddr->sn_next;
      i++;
      }
   if (i & 1)
      printf("\n");
   }
printf("\n");
}

