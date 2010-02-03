/*----------------------------------------------------------------------*
 * IFFW.C  Support routines for writing IFF-85 files.          1/23/86
 * (IFF is Interchange Format File.)
 *
 * By Jerry Morrison and Steve Shaw, Electronic Arts.
 * This software is in the public domain.
 *
 * This version for the Commodore-Amiga computer.
 *----------------------------------------------------------------------*/
#include "functions.h"
#include "iff.h"
#include "gio.h"
#include "stdio.h"
/* ---------- IFF Writer -----------------------------------------------*/

/* A macro to test if a chunk size is definite, i.e. not szNotYetKnown.*/
#define Known(size)   ( (size) != szNotYetKnown )

/* Yet another weird macro to make the source code simpler...*/
#define IfIffp(expr)  {if (iffp == IFF_OKAY)  iffp = (expr);}

/* ---------- OpenWIFF -------------------------------------------------*/
#if 0
extern FILE *console;
#endif
 
IFFP OpenWIFF(file, new0, limit)  BPTR file; GroupContext *new0; LONG limit; {
    register GroupContext *new = new0;
    register IFFP iffp = IFF_OKAY;

    new->parent       = NULL;
    new->clientFrame  = NULL;
    new->file         = file;
    new->position     = 0;
    new->bound        = limit;
    new->ckHdr.ckID   = NULL_CHUNK;  /* indicates no current chunk */
    new->ckHdr.ckSize = new->bytesSoFar = 0;
#if 0
fprintf(console,"openwiff-1\n");
#endif

#if 0
    if (0 > Seek(file, 0L, OFFSET_BEGINNING))	/* Go to start of the file.*/
	iffp = DOS_ERROR;
    else if ( Known(limit) && IS_ODD(limit) )
	iffp = CLIENT_ERROR;
#endif
#if 0
fprintf(console,"openwiff-2: %d\n",iffp);
#endif
    return(iffp);
    }

/* ---------- StartWGroup ----------------------------------------------*/
IFFP StartWGroup(parent, groupType, groupSize, subtype, new)
      GroupContext *parent, *new; ID groupType, subtype; LONG groupSize;  {
    register IFFP iffp;

    iffp = PutCkHdr(parent, groupType, (long)groupSize);
    IfIffp( IFFWriteBytes(parent, (BYTE *)&subtype, (long)sizeof(ID)) );
    IfIffp( OpenWGroup(parent, new) );
    return(iffp);
    }

/* ---------- OpenWGroup -----------------------------------------------*/
IFFP OpenWGroup(parent0, new0)  GroupContext *parent0, *new0; {
    register GroupContext *parent = parent0;
    register GroupContext *new    = new0;
    register LONG ckEnd;
    register IFFP iffp = IFF_OKAY;

    new->parent       = parent;
    new->clientFrame  = parent->clientFrame;
    new->file         = parent->file;
    new->position     = parent->position;
    new->bound        = parent->bound;
    new->ckHdr.ckID   = NULL_CHUNK;
    new->ckHdr.ckSize = new->bytesSoFar = 0;

    if ( Known(parent->ckHdr.ckSize) ) {
	ckEnd = new->position + ChunkMoreBytes(parent);
	if ( new->bound == szNotYetKnown || new->bound > ckEnd )
	    new->bound = ckEnd;
	};

    if ( parent->ckHdr.ckID == NULL_CHUNK || /* not currently writing a chunk*/
	 IS_ODD(new->position) ||
         (Known(new->bound) && IS_ODD(new->bound)) )
	iffp = CLIENT_ERROR;
    return(iffp);
    }

/* ---------- CloseWGroup ----------------------------------------------*/
IFFP CloseWGroup(old0)  GroupContext *old0; {
    register GroupContext *old = old0;
    IFFP iffp = IFF_OKAY;

    if ( old->ckHdr.ckID != NULL_CHUNK )  /* didn't close the last chunk */
	iffp = CLIENT_ERROR;
    else if ( old->parent == NULL ) {	  /* top level file context */
	if (GWriteFlush(old->file) < 0)  iffp = DOS_ERROR;
	}
    else {				  /* update parent context */
	old->parent->bytesSoFar += old->position - old->parent->position;
	old->parent->position = old->position;
	};
    return(iffp);
    }

/* ---------- EndWGroup ------------------------------------------------*/
IFFP EndWGroup(old)  GroupContext *old;  {
    register GroupContext *parent = old->parent;
    register IFFP iffp;

    iffp = CloseWGroup(old);
    IfIffp( PutCkEnd(parent) );
    return(iffp);
    }

/* ---------- PutCk ----------------------------------------------------*/
IFFP PutCk(context, ckID, ckSize, data)
      GroupContext *context; ID ckID; LONG ckSize; BYTE *data; {
    register IFFP iffp = IFF_OKAY;

    if ( ckSize == szNotYetKnown )
	iffp = CLIENT_ERROR;
    IfIffp( PutCkHdr(context, ckID, ckSize) );
    IfIffp( IFFWriteBytes(context, data, ckSize) );
    IfIffp( PutCkEnd(context) );
    return(iffp);
    }

/* ---------- PutCkHdr -------------------------------------------------*/
IFFP PutCkHdr(context0, ckID, ckSize)
      GroupContext *context0;  ID ckID;  LONG ckSize; {
    register GroupContext *context = context0;
    LONG minPSize = sizeof(ChunkHeader); /* physical chunk >= minPSize bytes*/

    /* CLIENT_ERROR if we're already inside a chunk or asked to write
     * other than one FORM, LIST, or CAT at the top level of a file */
    /* Also, non-positive ID values are illegal and used for error codes.*/
    /* (We could check for other illegal IDs...)*/
    if ( context->ckHdr.ckID != NULL_CHUNK  ||  ckID <= 0 )
	return(CLIENT_ERROR);
    else if (context->parent == NULL)  {
	switch (ckID)  {
	    case FORM:  case LIST:  case CAT:  break;
	    default: return(CLIENT_ERROR);
	    }
	if (context->position != 0)
	    return(CLIENT_ERROR);
	}

    if ( Known(ckSize) ) {
	if ( ckSize < 0 )
	    return(CLIENT_ERROR);
	minPSize += ckSize;
	};
    if ( Known(context->bound)  &&
         context->position + minPSize > context->bound )
	return(CLIENT_ERROR);

    context->ckHdr.ckID   = ckID;
    context->ckHdr.ckSize = ckSize;
    context->bytesSoFar   = 0;
    if (0 >
	GWrite(context->file, (BYTE *)&context->ckHdr
                , (long)sizeof(ChunkHeader))) return(DOS_ERROR);
    context->position += sizeof(ChunkHeader);
    return(IFF_OKAY);
    }

/* ---------- IFFWriteBytes ---------------------------------------------*/
IFFP IFFWriteBytes(context0, data, nBytes)
      GroupContext *context0;  BYTE *data;  LONG nBytes; {
    register GroupContext *context = context0;

    if ( context->ckHdr.ckID == NULL_CHUNK  ||	/* not in a chunk */
	 nBytes < 0  ||				/* negative nBytes */
	 (Known(context->bound)  &&		/* overflow context */
	    context->position + nBytes > context->bound)  ||
	 (Known(context->ckHdr.ckSize)  &&   	/* overflow chunk */
	    context->bytesSoFar + nBytes > context->ckHdr.ckSize) )
	return(CLIENT_ERROR);

    if (0 > GWrite(context->file, data, nBytes))
	return(DOS_ERROR);

    context->bytesSoFar += nBytes;
    context->position   += nBytes;
    return(IFF_OKAY);
    }

/* ---------- PutCkEnd -------------------------------------------------*/
IFFP PutCkEnd(context0)  GroupContext *context0; {
    register GroupContext *context = context0;
    WORD zero = 0;	/* padding source */
    long temp;

    if ( context->ckHdr.ckID == NULL_CHUNK )  /* not in a chunk */
	return(CLIENT_ERROR);

    if ( context->ckHdr.ckSize == szNotYetKnown ) {
	/* go back and set the chunk size to bytesSoFar */
	if ( 0 >
GSeek(context->file, (-((long)context->bytesSoFar + (long)sizeof(LONG)))
                      , (long)OFFSET_CURRENT) ||
	     0 >
GWrite(context->file, (BYTE *)&context->bytesSoFar, (long)sizeof(LONG))  ||
	     0 >
GSeek(context->file, (long)context->bytesSoFar, (long)OFFSET_CURRENT)  )
	    return(DOS_ERROR);
	}
#if 0
        temp = - ((long)context->bytesSoFar + (long)sizeof(LONG));
        printf("gseek: %ld %ld %ld %ld\n",temp
                                     ,(long)context->bytesSoFar
                                     ,(long)sizeof(LONG)
                                     ,(long)OFFSET_CURRENT);
	if ( 0 > GSeek(context->file,temp
                      , (long)OFFSET_CURRENT)) {
           printf("putckend 1:\n");
           return(DOS_ERROR);
        }
	if ( 0 > GWrite(context->file
                 , (BYTE *)&context->bytesSoFar
                 , (long)sizeof(LONG))) {
           printf("putckend 2:\n");
           return(DOS_ERROR);
        }
	if ( 0 > GSeek(context->file
                    , (long)context->bytesSoFar
                    , (long)OFFSET_CURRENT)  ) {
           printf("putckend 3:\n");
           return(DOS_ERROR);
	}
    }
#endif
    else {  /* make sure the client wrote as many bytes as planned */
	if ( context->ckHdr.ckSize != context->bytesSoFar )
	    return(CLIENT_ERROR);
	};

    /* Write a pad byte if needed to bring us up to an even boundary.
     * Since the context end must be even, and since we haven't
     * overwritten the context, if we're on an odd position there must
     * be room for a pad byte. */
    if ( IS_ODD(context->bytesSoFar) ) {
	if ( 0 > GWrite(context->file, (BYTE *)&zero, 1L) ) {
           return(DOS_ERROR);
        }
	context->position += 1;
    };

    context->ckHdr.ckID   = NULL_CHUNK;
    context->ckHdr.ckSize = context->bytesSoFar = 0;
    return(IFF_OKAY);
    }

