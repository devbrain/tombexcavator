
/** PreLoadAnim.c ***********************************************************
 *
 * Load an ILBM raster image file into memory structures
 * This is a modified version of ReadPict.c dated 23-Jan-86.
 * 
 * Modified by Gary Bonham, SPARTA, Inc.  15 Aug 1986
 *
 ***************************************************************************/

#define LOCAL   static

#include "intuall.h"
#include "libraries/dos.h"
#include "libraries/dosextens.h"
#include "ilbm.h"
#include "readpict.h"
#include "putanim.h"
#include "preloadanim.h"
#include "functions.h"


/* This example's max number of planes in a bitmap. Could use MaxAmDepth. */
#define EXDepth 5
#define maxColorReg (1<<EXDepth)
/*#define MIN(a,b) ((a)<(b)?(a):(b))*/

#define GetANHD(context,anHdr) \
   IFFReadBytes(context,(BYTE *)anHdr,(long)sizeof(AnimationHeader))

/* Define the size of a temporary buffer used in unscrambling the ILBM rows.*/
#define bufSz 512

int EndFlag;
struct IFFfile *IFFfileList;
 
/*------------ ILBM reader -----------------------------------------------*/
/* ILBMFrame is our "client frame" for reading FORMs ILBM in an IFF file.
 * We allocate one of these on the stack for every LIST or FORM encountered
 * in the file and use it to hold BMHD & CMAP properties. We also allocate
 * an initial one for the whole file.
 * We allocate a new GroupContext (and initialize it by OpenRIFF or
 * OpenRGroup) for every group (FORM, CAT, LIST, or PROP) encountered. It's
 * just a context for reading (nested) chunks.
 *
 * If we were to scan the entire example file outlined below:
 *    reading          proc(s)                new               new
 *
 * --whole file--   ReadPicture+ReadIFF   GroupContext        ILBMFrame
 * CAT              ReadICat                GroupContext
 *   LIST           GetLiILBM+ReadIList       GroupContext        ILBMFrame
 *     PROP ILBM    GetPrILBM                   GroupContext
 *       CMAP       GetCMAP
 *       BMHD       GetBMHD
 *     FORM ILBM    GetFoILBM                   GroupContext        ILBMFrame
 *       BODY       GetBODY
 *     FORM ILBM    GetFoILBM                   GroupContext        ILBMFrame
 *       BODY       GetBODY
 *   FORM ILBM      GetFoILBM                 GroupContext        ILBMFrame
 */

/* NOTE: For a small version of this program, set Fancy to 0.
 * That'll compile a program that reads a single FORM ILBM in a file, which
 * is what DeluxePaint produces. It'll skip all LISTs and PROPs in the input
 * file. It will, however, look inside a CAT for a FORM ILBM.
 * That's suitable for 90% of the uses.
 *
 * For a fancier version that handles LISTs and PROPs, set Fancy to 1.
 * That'll compile a program that dives into a LIST, if present, to read
 * the first FORM ILBM. E.g. a DeluxePrint library of images is a LIST of
 * FORMs ILBM.
 *
 * For an even fancier version, set Fancy to 2. That'll compile a program
 * that dives into non-ILBM FORMs, if present, looking for a nested FORM ILBM.
 * E.g. a DeluxeVideo C.S. animated object file is a FORM ANBM containing a
 * FORM ILBM for each image frame. */

/* Global access to client-provided pointers.*/
LOCAL anim;
extern IFFP GetLiILBM(),GetPrILBM();
BYTE *SavePlanes[2];

/** GetFoANIM() *************************************************************
 *
 * Called via ReadPicture to handle every FORM encountered in an IFF file.
 * Reads FORMs ILBM and skips all others.
 * Inside a FORM ILBM, it stops once it reads a BODY. It complains if it
 * finds no BODY or if it has no BMHD to decode the BODY.
 *
 * Once we find a BODY chunk, we'll allocate the BitMap and read the image.
 *
 ****************************************************************************/
LOCAL BYTE bodyBuffer[bufSz];
IFFP GetFoANIM(parent)
GroupContext *parent;
{
   /*compilerBug register*/ IFFP iffp;

   GroupContext formContext;
   ILBMFrame ilbmFrame;      /* only used for non-clientFrame fields.*/
 
   /* Handle a non-ILBM FORM. */
   if (parent->subtype != ID_ILBM) {
      if (parent->subtype == ID_ANIM) anim = 1;
      /* Open a non-ILBM FORM and recursively scan it for ILBMs.*/
      iffp = OpenRGroup(parent, &formContext);
      CheckIFFP();
      do {
          iffp = GetF1ChunkHdr(&formContext);
          if (iffp == IFF_DONE) iffp = IFF_OKAY;
      } while (iffp >= IFF_OKAY);
      if (iffp == END_MARK)  iffp = IFF_DONE;   /* then continue scanning the file */
      CloseRGroup(&formContext);
      return(iffp);
   }

   ilbmFrame = *(ILBMFrame *)parent->clientFrame;
   iffp = OpenRGroup(parent, &formContext);
   CheckIFFP();
 
   if (IFFfileList->lastframe == NULL) {
      IFFfileList->lastframe =
              (struct FrameHD *)AllocMem((long)sizeof(struct FrameHD)
                           ,(long)(MEMF_CLEAR));
      IFFfileList->firstframe = IFFfileList->lastframe;
      IFFfileList->lastframe->prev = NULL;
   }
   else {
      IFFfileList->lastframe->next =
              (struct FrameHD *)AllocMem((long)sizeof(struct FrameHD)
                           ,(long)(MEMF_CLEAR));
      IFFfileList->lastframe->next->prev = IFFfileList->lastframe;
      IFFfileList->lastframe = IFFfileList->lastframe->next;
   }
   if (IFFfileList->lastframe == 0)
      GoodBye("Not enough memory for animation");
   IFFfileList->lastframe->next = NULL;
   IFFfileList->lastframe->bmhd = NULL;
   IFFfileList->lastframe->anhd = NULL;
   IFFfileList->lastframe->camg = NULL;
   IFFfileList->lastframe->nColorRegs = 0;
   IFFfileList->lastframe->body = NULL;
   IFFfileList->lastframe->bodylength = 0;
 
   do {
      switch (iffp = GetFChunkHdr(&formContext)) {
         case ID_ANHD: {
            ilbmFrame.foundBMHD = TRUE;
            IFFfileList->lastframe->anhd =
               (AnimationHeader *)AllocMem((long)sizeof(AnimationHeader),0L);
            if (IFFfileList->lastframe->anhd == 0) {
                  puts("Not enough memory for animation...");
                  puts("\n *** Animation truncated *** \n");
                  iffp = IFF_DONE;
                  break;
            }
	    iffp = GetANHD(&formContext, IFFfileList->lastframe->anhd);
	    break;
         }
         case ID_BMHD: {
            int hh;
            ilbmFrame.foundBMHD = TRUE;
            IFFfileList->lastframe->bmhd =
               (BitMapHeader *)AllocMem((long)sizeof(BitMapHeader),0L);
            if (IFFfileList->lastframe->bmhd == 0)
               GoodBye("Not enough memory for animation");
            iffp = GetBMHD(&formContext, IFFfileList->lastframe->bmhd);
            if (IFFfileList->lastframe->bmhd->pageHeight <
                IFFfileList->lastframe->bmhd->h) 
                    IFFfileList->lastframe->bmhd->pageHeight
                      = IFFfileList->lastframe->bmhd->h;
            if (IFFfileList->lastframe->bmhd->pageWidth <
                IFFfileList->lastframe->bmhd->w) 
                    IFFfileList->lastframe->bmhd->pageWidth
                      = IFFfileList->lastframe->bmhd->w;
            hh = IFFfileList->lastframe->bmhd->nPlanes
               * IFFfileList->lastframe->bmhd->pageHeight;
            SavePlanes[0] = (BYTE *)AllocRaster(
                 (long)IFFfileList->lastframe->bmhd->pageWidth,(long)hh);
            SavePlanes[1] = (BYTE *)AllocRaster(
                 (long)IFFfileList->lastframe->bmhd->pageWidth,(long)hh);
            if (SavePlanes[1] == 0)
               GoodBye("Not enough memory for animation bitmaps");
            break;
         }
         case ID_CAMG: {
            ilbmFrame.foundCAMG = TRUE;
            IFFfileList->lastframe->camg =
               (CamgChunk *)AllocMem((long)sizeof(CamgChunk),0L);
            if (IFFfileList->lastframe->camg == 0)
               GoodBye("Not enough memory for animation");
            iffp = GetCAMG(&formContext
                          ,IFFfileList->lastframe->camg);
            break;
         }
         case ID_CMAP: {
            IFFfileList->lastframe->nColorRegs
               = maxColorReg;  /* we have room for this many */
            iffp = GetCMAP(&formContext
                          ,&IFFfileList->lastframe->cmap[0]
                          ,&IFFfileList->lastframe->nColorRegs);
            break;
         }
         case ID_DLTA: /* at this point just read in delta chunk */
         case ID_BODY: {
            if (!ilbmFrame.foundBMHD)  return(BAD_FORM);   /* No BMHD chunk! */
            IFFfileList->lastframe->bodylength = formContext.ckHdr.ckSize;
            if (IFFfileList->lastframe->bodylength) {
               IFFfileList->lastframe->body = (ULONG *)AllocMem(
                            (long)(IFFfileList->lastframe->bodylength),0L);
               if (IFFfileList->lastframe->body == 0) {
                  puts("Not enough memory for animation body...");
                  puts("\n *** Animation truncated *** \n");
     EndFlag = 1;
                  iffp = END_MARK;
                  break;
               }
               IFFReadBytes(&formContext,(BYTE *)IFFfileList->lastframe->body
                                     ,IFFfileList->lastframe->bodylength);
            }
            else IFFfileList->lastframe->bodylength = 0;
            break;
         }
         case END_MARK: {
            iffp = IFF_DONE; 
            break;
         } /* No BODY chunk! */
      }
   } while (iffp >= IFF_OKAY);  /* loop if valid ID of ignored chunk or a
                               * subroutine returned IFF_OKAY (no errors).*/

   if (iffp != IFF_DONE)  return(iffp);

   /* If we get this far, there were no errors. */
   CloseRGroup(&formContext);
   return(iffp);
}

/** Notes on extending GetFoILBM ********************************************
 *
 * To read more kinds of chunks, just add clauses to the switch statement.
 * To read more kinds of property chunks (GRAB, CAMG, etc.) add clauses to
 * the switch statement in GetPrILBM, too.
 *
 * To read a FORM type that contains a variable number of data chunks--e.g.
 * a FORM FTXT with any number of CHRS chunks--replace the ID_BODY case with
 * an ID_CHRS case that doesn't set iffp = IFF_DONE, and make the END_MARK
 * case do whatever cleanup you need.
 *
 ****************************************************************************/

/** PreLoadAnimation() **********************************************************/
IFFP PreLoadAnimation(filename, file, iFrame)
char *filename;
LONG file;
ILBMFrame *iFrame;   /* Top level "client frame".*/

{
   int i;
   struct IFFfile *IFFptr;
   IFFP iffp = IFF_OKAY;
   anim = 0;

   iFrame->clientFrame.getList = GetLiILBM;
   iFrame->clientFrame.getProp = GetPrILBM;
   iFrame->clientFrame.getForm = GetFoANIM;
   iFrame->clientFrame.getCat  = ReadICat ;

   /* Initialize the top-level client frame's property settings to the
    * program-wide defaults. This example just records that we haven't read
    * any BMHD property or CMAP color registers yet. For the color map, that
    * means the default is to leave the machine's color registers alone.
    * If you want to read a property like GRAB, init it here to (0, 0). */
   iFrame->foundBMHD  = FALSE;
   iFrame->nColorRegs = 0;

  /* Store a pointer to the client's frame in a global variable so that
   * GetFoILBM can update client's frame when done.  Why do we have so
   * many frames & frame pointers floating around causing confusion?
   * Because IFF supports PROPs which apply to all FORMs in a LIST,
   * unless a given FORM overrides some property.  
   * When you write code to read several FORMs,
   * it is ssential to maintain a frame at each level of the syntax
   * so that the properties for the LIST don't get overwritten by any
   * properties specified by individual FORMs.
   * We decided it was best to put that complexity into this one-FORM example,
   * so that those who need it later will have a useful starting place.
   */

   IFFptr = (struct IFFfile *)AllocMem((long)sizeof(struct IFFfile)
                 ,(long)MEMF_CLEAR);
   IFFptr->next = IFFfileList;
   IFFfileList = IFFptr;
   for (i=0;i<40;i++) IFFfileList->name[i] = filename[i];
   IFFfileList->firstframe = NULL;
   IFFfileList->lastframe  = NULL;
 
   iffp = ReadIFF(file, (ClientFrame *)iFrame);
   return(iffp);
}


InitPreLoadIFF()
{
   IFFfileList = NULL;
   return(0);
}

ClosePreLoadIFF()
{
   struct IFFfile *currentfile,*nextfile;
   struct FrameHD *currentframe,*nextframe;
 
   currentfile = IFFfileList;
   while (currentfile) {
      currentframe = currentfile->firstframe;
      while (currentframe) {
         if (currentframe->bmhd) FreeMem(currentframe->bmhd
                                 ,(long)sizeof(BitMapHeader));
         if (currentframe->anhd) FreeMem(currentframe->anhd
                                 ,(long)sizeof(AnimationHeader));
         if (currentframe->body) {
            if (currentframe->bodylength)
               FreeMem(currentframe->body
                                 ,(long)currentframe->bodylength);
         }
         nextframe = currentframe->next;
         FreeMem(currentframe,(long)sizeof(struct FrameHD));
         currentframe = nextframe;
      }
      nextfile = currentfile->next;
      FreeMem(currentfile,(long)sizeof(struct IFFfile));
      currentfile = nextfile;
   }
   IFFfileList = NULL;
   return(0);
}
