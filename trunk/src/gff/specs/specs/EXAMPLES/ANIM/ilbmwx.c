/*----------------------------------------------------------------------*
 * ILBMW.C  Support routines for writing ILBM files.            1/23/86
 * (IFF is Interchange Format File.)
 *
 * revised for VideoStage animation IFF files by Gary Bonham,
 * Sparta Inc., 15 Aug 86.
 *
 * By Jerry Morrison and Steve Shaw, Electronic Arts.
 * This software is in the public domain.
 *
 * This version for the Commodore-Amiga computer.
 *----------------------------------------------------------------------*/
#include "functions.h"
#include "packer.h"
#include "ilbm.h"
#include "stdio.h"

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

extern int Pop;
UBYTE BMmask;

/*---------- InitBMHdr -------------------------------------------------*/
IFFP InitBMHdr(bmHdr0, bitmap, masking, compression, transparentColor,
	    pageWidth, pageHeight)
        BitMapHeader *bmHdr0;  struct BitMap *bitmap;
        WORD masking;		/* Masking */
	WORD compression;	/* Compression */
	WORD transparentColor;	/* UWORD */
	WORD pageWidth, pageHeight;
    {
    register BitMapHeader *bmHdr = bmHdr0;
    register WORD rowBytes = bitmap->BytesPerRow;
    long maxx,minx,maxy,miny,ip,ib,ir;

    bmHdr->w = rowBytes << 3;
    bmHdr->h = bitmap->Rows;
    bmHdr->x = bmHdr->y = 0;	/* Default position is (0,0).*/
    bmHdr->nPlanes = bitmap->Depth;
    bmHdr->masking = masking;
    bmHdr->compression = compression;
    bmHdr->pad1 = 0;
    bmHdr->transparentColor = transparentColor;
    bmHdr->xAspect = bmHdr->yAspect = 1;
    bmHdr->pageWidth = pageWidth;
    bmHdr->pageHeight = pageHeight;
    BMmask = 0xFF;

    if (Pop) {
       maxx = 0;
       minx = 10000;
       maxy = 0;
       miny = 10000;
       BMmask = 0;
       for (ip=0;ip<bitmap->Depth;ip++) {
          for (ir=0;ir<bitmap->Rows;ir++) {
             for (ib=0;ib<rowBytes;ib++) {
                if (*(bitmap->Planes[ip]+ib+ir*rowBytes)) {
                   BMmask |= 1<<ip;
                   maxx = MAX(maxx,ib);
                   minx = MIN(minx,ib);
                   maxy = MAX(maxy,ir);
                   miny = MIN(miny,ir);
                }
             }
          }
       }
       if (BMmask == 0) {
          minx = 0;
          miny = 0;
          maxx = rowBytes - 1;
          maxy = bitmap->Rows - 1;
       }
       rowBytes = maxx - minx + 1;
       if (rowBytes & 1) {
          rowBytes++;
          if (minx & 1) minx--;
          else          maxx++;
       }
       bmHdr->x = minx << 3;
       bmHdr->y = miny;
       bmHdr->w = rowBytes << 3;
       bmHdr->h = maxy - miny + 1;
    }

    if (pageWidth < 400) {
        if (pageHeight < 300) {
           bmHdr->xAspect = x320x200Aspect;
	   bmHdr->yAspect = y320x200Aspect;
        }
        else {
           bmHdr->xAspect = x320x400Aspect;
	   bmHdr->yAspect = y320x400Aspect;
        }
    }
    else {
        if (pageHeight < 300) {
           bmHdr->xAspect = x640x200Aspect;
	   bmHdr->yAspect = y640x200Aspect;
        }
	else {
           bmHdr->xAspect = x640x400Aspect;
	   bmHdr->yAspect = y640x400Aspect;
        }
    }

    return( IS_ODD(rowBytes) ? CLIENT_ERROR : IFF_OKAY );
    }

/*---------- PutCMAP ---------------------------------------------------*/
IFFP PutCMAP(context, colorMap, depth)   
      GroupContext *context;  WORD *colorMap;  UBYTE depth;
   {
   register LONG nColorRegs;   
   IFFP iffp;
   ColorRegister colorReg;

   if (depth > 5)   depth = 5;
   nColorRegs = 1 << depth;

   iffp = PutCkHdr(context, ID_CMAP
       , (long)(nColorRegs * sizeofColorRegister));
   CheckIFFP();

   for ( ;  nColorRegs;  --nColorRegs)  {
      colorReg.red   = ( *colorMap >> 4 ) & 0xf0;
      colorReg.green = ( *colorMap      ) & 0xf0;
      colorReg.blue  = ( *colorMap << 4 ) & 0xf0;
      iffp = IFFWriteBytes(context, (BYTE *)&colorReg
                 , (long)sizeofColorRegister);
      CheckIFFP();
      ++colorMap;
      }

   iffp = PutCkEnd(context);
   return(iffp);
   }

/*---------- PutBODY ---------------------------------------------------*/
/* NOTE: This implementation could be a LOT faster if it used more of the
 * supplied buffer. It would make far fewer calls to IFFWriteBytes (and
 * therefore to DOS Write). */
IFFP PutBODY(context, bitmap, mask, bmHdr, buffer, bufsize)
      GroupContext *context;  struct BitMap *bitmap;  BYTE *mask;
      BitMapHeader *bmHdr;  BYTE *buffer;  LONG bufsize;
   {         
   IFFP iffp;
   LONG rowBytes = bitmap->BytesPerRow;
   long dstDepth = bmHdr->nPlanes;
   Compression compression = bmHdr->compression;
   long planeCnt;		/* number of bit planes including mask */
   long deltabyte;
   register long iPlane, iRow;
   register LONG packedRowBytes;
   BYTE *buf;
   BYTE *planes[MaxAmDepth + 1]; /* array of ptrs to planes & mask */

   deltabyte = (bmHdr->pageWidth>>3) - (bmHdr->w>>3);

   if ( bufsize < MaxPackedSize(rowBytes)  ||	/* Must buffer a comprsd row*/
        compression > cmpByteRun1  ||		/* bad arg */
	bitmap->Depth < dstDepth   ||		/* inconsistent */
	dstDepth > MaxAmDepth )			/* too many for this routine*/
      return(CLIENT_ERROR);

   planeCnt = dstDepth + (mask == NULL ? 0 : 1);

   /* Copy the ptrs to bit & mask planes into local array "planes" */
   for (iPlane = 0; iPlane < dstDepth; iPlane++)
      if ((Pop == 0) || ((BMmask>>iPlane)&1))
         planes[iPlane] = (BYTE *)bitmap->Planes[iPlane]
                     + bmHdr->y * rowBytes + (bmHdr->x>>3);
      else planes[iPlane] = NULL;
#if 0
      printf("plane %ld: %ld %ld\n",iPlane,planes[iPlane],bitmap->Planes[iPlane]);
#endif
   if (mask != NULL) planes[dstDepth] = mask;

   /* Write out a BODY chunk header */
   iffp = PutCkHdr(context, ID_BODY, (long)szNotYetKnown);
   CheckIFFP();

   /* Write out the BODY contents */
   for (iRow = bmHdr->h; iRow > 0; iRow--)  {
      for (iPlane = 0; iPlane < planeCnt; iPlane++)  {
         if (planes[iPlane]) {
            /* Write next row.*/
            if (compression == cmpNone) {
               iffp = IFFWriteBytes(context, planes[iPlane], (long)rowBytes);
               planes[iPlane] += rowBytes;
            }

            /* Compress and write next row.*/
            else {
               buf = buffer;
               packedRowBytes = PackRow(&planes[iPlane], &buf
                      , (long)(bmHdr->w>>3));
               iffp = IFFWriteBytes(context, buffer, (long)packedRowBytes);
               planes[iPlane] += deltabyte;
            }
         }
         CheckIFFP();
      }
   }

   /* Finish the chunk */
   iffp = PutCkEnd(context);
   return(iffp);
}
