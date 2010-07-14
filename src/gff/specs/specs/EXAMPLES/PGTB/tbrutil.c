/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* |_o_o|\\  The Software Distillery					   */
/* |. o.| || Made available to the Amiga development community		   */
/* | o	| || the authors:					 BBS:	   */
/* |  . |//    John Mainwaring				   (919)-471-6436  */
/* ======								   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Utilities used to read both traceback dump and code files.  Defines a     */
/* longjump buffer used to bail out on read errors.  Users desiring sane     */
/* error handling would be well advised to initialize it via setjmp.	     */

#include "tb.h"
#include "setjmp.h"

jmp_buf bailout;

long getlong(file)
/*   -------	*/
FILE *file;
{
long buffer;

if (fread((char *)&buffer, 1, 4, file) != 4)
   {
   fprintf(stderr, "unexpected EOF\n");
   longjmp(bailout, 1);
   }

return buffer;
}

long forcegetlong(file)
/*   -----------*/
/* don't longjmp on EOF or error - return 0 to caller which is treated  */
/* as a termination condition.						*/
FILE *file;
{
long buffer;

if (fread((char *)&buffer, 1, 4, file) != 4)
   return 0;
else
   return buffer;
}

void getblock(file, buffer, size)
/*   --------		       */
FILE *file;
ULONG *buffer;
long size;
{
size <<= 2;
if (fread((char *)buffer, 1, size, file) != size)
   {
   fprintf(stderr, "unexpected EOF\n");
   longjmp(bailout, 1);
   }
}

ULONG getascii(file, buffer)
/*   --------		       */
FILE *file;
char **buffer;
{
ULONG size;

size = getlong(file);
if (!(*buffer = AllocMem(size<<2, MEMF_CLEAR)))
   {
   fprintf(stderr, "*** unable to allocate memory\n");
   exit(FATAL);
   }
getblock(file, (ULONG *)*buffer, size);
return(size);
}

void skiplong(file, size)
/*   --------	       */
FILE *file;
long size;
{
if (fseek(file, size*4, 1) == -1)
   {
   fprintf(stderr, "unexpected EOF\n");
   longjmp(bailout, 1);
   }
}
