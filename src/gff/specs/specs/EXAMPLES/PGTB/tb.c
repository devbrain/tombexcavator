/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* |_o_o|\\  The Software Distillery					     */
/* |. o.| || Made available to the Amiga development community		     */
/* | .	| || the authors:					   BBS:      */
/* | o	| ||   John Mainwaring, Jim Cooper		     (919)-471-6436  */
/* |  . |//								     */
/* ======								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Driver program to open files for Traceback file dump */

/* version identification for copyright notice		*/
char *TBVers = "0.91";

#include "tb.h"

struct tbtemplate *tbdata;

int tidyproc(ret)
/*  --------*/
int ret;
{
struct udata *thisud, *nextud;
struct symbol_node *thissn, *nextsn;
struct line_node *thisln, *nextln;
long i;

if (tbdata)
   {
   if(tbdata->taskname)
      FreeMem(tbdata->taskname, strlen(tbdata->taskname)+1);
   if(tbdata->segments)
      {
      for (i = 0; i < tbdata->segcount; i++)
	 {
	 thissn = tbdata->segments[i].symbols;
	 while (thissn)
	    {
	    nextsn = thissn->sn_next;
	    FreeMem((char *)thissn, thissn->sn_memsize);
	    thissn = nextsn;
	    }
	 thisln = tbdata->segments[i].lines;
	 while (thisln)
	    {
	    nextln = thisln->ln_next;
	    FreeMem((char *)thisln, thisln->ln_size);
	    thisln = nextln;
	    }
	 }
      FreeMem((char *)tbdata->segments,
	 tbdata->segcount * sizeof(struct segment));
      }
   if(tbdata->filename)
      FreeMem(tbdata->filename, strlen(tbdata->filename)+1);
   thisud = tbdata->udhead;
   while (thisud)
      {
      nextud = thisud->udptr;
      FreeMem((char *)thisud, (thisud->udsize << 2) + 8);
      thisud = nextud;
      }
   FreeMem((char *)tbdata, sizeof(struct tbtemplate));
   }

return(ret);
}

void main(argc, argv)
/*   ----*/
int argc;
char *argv[];
{
char *tbfname = "Snapshot.TB", *exfname = 0;
FILE *tbfile, *exfile = 0;
int flags = FAILFLG; /*default - include fail address*/

if (argc >= 1)
   {
   if (argv[1][0] == '?')
      {
      fprintf(stderr,
"Usage: %s [-lxsrvmu] [[TB file] executable file]\n", argv[0]);
      fprintf(stderr, "  default TB file is %s\n", tbfname);
      fprintf(stderr,
"  -a all (life the universe and everything)    -x symbol xref -s stack\n");
      fprintf(stderr,
"  -r registers   -v environment -m free memory -u user data   -t trace\n");
      exit(FATAL);
      }
   while (argv[1][0] == '-')
      {
      argv[1]++;
      while (argv[1][0])
	 {
	 switch (argv[1][0])
	 {
	 case 'a':
	    flags = -1; /* sloppy way to turn everything on */
	    break;
	 case 'x':
	    flags = flags | SYMFLG;
	    break;
	 case 's':
	    flags = flags | STAKFLG;
	    break;
	 case 'r':
	    flags = flags | REGFLG;
	    break;
	 case 't':
	    flags = flags | TRACEFLG;
	    break;
	 case 'v':
	    flags = flags | ENVFLG;
	    break;
	 case 'm':
	    flags = flags | FMEMFLG;
	    break;
	 case 'u':
	    flags = flags | UDATFLG;
	    break;
	 }
	 argv[1]++;
	 }
      argv++;
      argc--;
      }
   if (argc == 2)
      exfname = argv[1];
   else if (argc > 2)
      {
      tbfname = argv[1];
      exfname = argv[2];
      }
   }

if (!onexit(&tidyproc))
   {
   fprintf(stderr, "*** unable to set exit proc\n");
   exit(FATAL);
   }

if (!(tbdata = (struct tbtemplate *)
	       AllocMem(sizeof(struct tbtemplate), MEMF_CLEAR)))
   {
   fprintf(stderr, "*** unable to allocate memory\n");
   exit(FATAL);
   }

printf("TraceDump %s Copyright (c) 1988 The Software Distillery\n\n", TBVers);

tbfile = fopen(tbfname, "r");

if (tbfile == 0)
   {
   fprintf(stderr, "*** unable to open traceback file %s\n", tbfname);
   exit(FATAL);
   }

if (!tbread(tbfile))  exit(FATAL); /* it already printed a complaint */
fclose(tbfile);

if (exfname == 0)
   exfname = tbdata->taskname;

if (exfname)
   {
   exfile = fopen(exfname, "r");

   if (exfile == 0)
      {
      fprintf(stderr, "*** unable to open executable file %s\n", exfname);
      /* exit(FATAL); or prompt for another try? */
      }
   else
      if (!readsym(exfile))  exit(FATAL); /* it already printed a complaint */
      fclose(exfile);
   }

tbdump(flags);
}

