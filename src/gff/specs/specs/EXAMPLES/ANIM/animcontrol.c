 
/* animcontrol.c
 *
 *      functions for animation file manipulation
 *
 * This routine provided for the GrabAnim program by 
 * Gary Bonham, SPARTA, Inc., Laguna Hills.
 *
 * This is also intended to serve as an example of how to
 * generate ANIM files.
 *
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/gfx.h>
#include <graphics/view.h>
#include "functions.h"

#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include "ilbm.h"
#include "readpict.h"
  
extern struct BitMap *mybitmap[];
static LONG fil;
static UBYTE *buffer;
static int Width,Height,Depth;

/* ======================================================= */
 
/* openanim filename */
OpenAnimationFile(nbuf,cc,cm,vp)
struct ViewPort *vp;
struct ColorMap *cm;
int nbuf;
char *cc;
{
   fil = Open(cc,(long)MODE_NEWFILE);
   if (fil == 0) puts("Cannot open animation file");
   buffer = (UBYTE *)AllocMem(16000L,(long)MEMF_CHIP);
   Width = mybitmap[nbuf]->BytesPerRow <<3;
   Height = mybitmap[nbuf]->Rows;
   printf("%d %d %d\n",Width,Height,vp->Modes);
   OpenAnim(fil,mybitmap[nbuf]
          , Width
          , Height
          , cm->ColorTable
          , vp->Modes
          , buffer,16000L);
   return(0);
}

/* addanim */
AddAnimationFrame(nbuf)
int nbuf;
{
   Width = mybitmap[nbuf]->BytesPerRow <<3;
   Height = mybitmap[nbuf]->Rows;
   AddAnim(mybitmap[nbuf]
     , Width
     , Height
     , 1,buffer,16000L);
   return(0);
}

/* addanim */
AddAnimationFrame2(data,ndata,pop)
int pop;
LONG *data,ndata;
{
   AddAnim2(data,ndata,pop);
   return(0);
}

/* closeanim */
CloseAnimationFile()
{
   CloseAnim(fil);
   Close(fil);
   FreeMem(buffer,16000L);
   fil = 0;
   return();
}
