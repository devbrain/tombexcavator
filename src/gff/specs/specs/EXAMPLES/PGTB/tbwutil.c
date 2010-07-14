/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* |_o_o|\\  The Software Distillery					     */
/* |. o.| || Made available to the Amiga development community		     */
/* | .	| || the authors:					   BBS:      */
/* | o	| ||   Jim Cooper, John Mainwaring		     (919)-471-6436  */
/* |  . |//								     */
/* ======								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* routines used during output of traceback	*/

#include "tb.h"

extern struct tbtemplate *tbdata;

int valid_display(char);
void hexconv(unsigned char *, unsigned char);

int valid_display(byte) /* return filtered ASCII char */
/*  -------------    */
char byte;

/* Filter all CTRL and CSI chars which could mess things up */
{
  if (((byte>31) && (byte<127)) || (byte>159)) return(int)byte;
  else return(int)'.';
}

void hexconv(result,byte) /* byte to 2 ASCII hex digits */
/*   -------						*/
unsigned char result[2],byte;
{
  result[0]=(byte>>4)+48+7*((byte>>4)>9);
  result[1]=(byte & 0xf)+48+7*((byte & 0xf)>9);
}

void hexdump(fh, secbuf, len, addr)
/*   -------			 */
FILE *fh;
unsigned char *secbuf;
long len;
long addr;
{
  register int row,col,i;
  register char strbuf[100];

  /* record display core routine : Due to the slowness of the
     console file drivers, display is line buffered for speed. */

  for (row=0;row*16<len;row++) {
    i=0;
    for (col=0;row*16+col<len && col<16;col++) {
      hexconv(&strbuf[i],secbuf[row*16+col]); i+=2;
      if (((col+1)%4)==0) strbuf[i++]=' ';
    }
    while (i<36)
      strbuf[i++] = ' ';
    strbuf[i++]=':'; strbuf[i++]=' ';
    for (col=0;row*16+col<len && col<16;col++)
      strbuf[i++]=valid_display(secbuf[row*16+col]);

    strbuf[i]='\0';
    fprintf(fh, "%06X: %s\n", addr, strbuf);
    addr += 16;
  }
}

void longtoascii(l, str)
/*   -----------*/
ULONG l;
char *str;
{
char *lptr;
int i;

lptr = (char *)&l;
for (i=0; i<4; i++)
   *str++ = valid_display(*lptr++);
*str = '\0';
}

int locaddr(addr, info)
/*  -------*/
/* Find what hunk an address is in.  If hunk has a symbol chain, find	*/
/* offset from symbol, else return offset in hunk. Return false if	*/
/* address is not in any of the hunks.					*/

ULONG addr;
struct addrinfo *info;
{
int i, j, offset, tabsize;
struct symbol_node *symaddr;
struct line_node *lineaddr;
struct segment *seg;
struct line_elem *curln;

info->name = 0;
info->objname = 0;

for (i = 0; i < tbdata->segcount; i++)
   {
   seg = &tbdata->segments[i];
   if ((seg->addr <= addr) && (addr < seg->addr + seg->size))
      {
      info->hunknum = i;
      offset = info->offset = addr - seg->addr;

      symaddr = (struct symbol_node *)&(seg->symbols);
      while (symaddr->sn_next)
	 {
	 if (symaddr->sn_next->sn_value <= addr)
	    info->name = symaddr->sn_next->sn_sym;
	 else
	    break;
	 symaddr = symaddr->sn_next;
	 }
      if (info->name)
	 info->offset = addr - symaddr->sn_value;

      lineaddr = (struct line_node *)&(seg->lines);
      while (lineaddr->ln_next)
	 {
	 if (lineaddr->ln_next->ln_offset <= offset)
	    info->objname = lineaddr->ln_next->ln_name;
	 else
	    break;
	 lineaddr = lineaddr->ln_next;
	 }
      if (info->objname)
	 {
	 offset = offset - lineaddr->ln_offset;
	 curln = (struct line_elem *)&lineaddr->ln_name[lineaddr->ln_nsize<<2];
	 tabsize = (lineaddr->ln_size
	    - (sizeof(struct line_node) - 4 + (lineaddr->ln_nsize<<2)) >> 3);
	 for (j=1; j<=tabsize; j++)
	    {
	    if (curln->le_off > offset)
	       {
	       break;
	       }
	    curln += 1;
	    }
	 curln -= 1;  /* could die if first offset != 0???		*/
	 info->line = curln->le_line;
	 info->lineoff = offset - curln->le_off;
	 if (j > tabsize)
	    if (info->lineoff > 32)     /* probably outside this file   */
	       info->objname = 0;	/* deny knowledge of line	*/
	 }
      return (1);
      }
   }
return (0);
}
