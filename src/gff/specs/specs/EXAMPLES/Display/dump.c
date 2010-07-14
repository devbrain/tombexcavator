/*
 * dump.c    - routine to dump rastport
 *
 */


#include "exec/types.h"
#include "intuition/intuition.h"
#include "devices/printer.h"

extern struct IODRPReq  *CreateExtIO();
extern struct MsgPort   *CreatePort();

dump(screen)
struct Screen *screen;
   {
   struct IODRPReq *iodrp;
   struct MsgPort  *printerPort;
   struct ViewPort *vp;
   int error = 1;

   if(printerPort = CreatePort("CAS_ddmp",0))
      {
      if(iodrp=CreateExtIO(printerPort,sizeof(struct IODRPReq)))
         {
         if(!(error=OpenDevice("printer.device",0,iodrp,0)))
            {
            vp = &screen->ViewPort;
            iodrp->io_Command = PRD_DUMPRPORT;
            iodrp->io_RastPort = &screen->RastPort;
            iodrp->io_ColorMap = vp->ColorMap;
            iodrp->io_Modes = (ULONG)vp->Modes;
       /*   iodrp->io_SrcX = 0;     MEMF_CLEAR zeroed this  */
       /*   iodrp->io_SrcY = 0;     MEMF_CLEAR zeroed this  */
            iodrp->io_SrcWidth = screen->Width;
            iodrp->io_SrcHeight = screen->Height;
       /*   iodrp->io_DestCols = 0; MEMF_CLEAR zeroed this  */
       /*   iodrp->io_DestRows = 0; MEMF_CLEAR zeroed this  */
            iodrp->io_Special = SPECIAL_FULLCOLS|SPECIAL_ASPECT;

            error = DoIO(iodrp);
            CloseDevice(iodrp);
            }
         DeleteExtIO(iodrp, sizeof(struct IODRPReq));
         }
      DeletePort(printerPort);
      }
   return(error);
   }



