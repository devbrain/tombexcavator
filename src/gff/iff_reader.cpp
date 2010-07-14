#include "gff/iff.hpp"
#include "gff/gio.hpp"

/* ----- Private subroutine FileLength() --------------------------------*/
/* Returns the length of the file or else a negative IFFP error code
 * (NO_FILE or DOS_ERROR). AmigaDOS-specific implementation.
 * SIDE EFFECT: Thanks to AmigaDOS, we have to change the file's position
 * to find its length.
 * Now if Amiga DOS maintained fh_End, we'd just do this:
 *    fileLength = (FileHandle *)BADDR(file)->fh_End; */
long FileLength (BPTR file)  {
    long fileLength = NO_FILE;

    if (file > 0)  
      {
	long pos = ftell (file);
	fseek (file, 0, SEEK_END);
	fileLength = ftell (file);
	fseek (file, pos, SEEK_SET);
	if (fileLength < 0)
	    fileLength = DOS_ERROR;	/* DOS being absurd.*/
	}
    return(fileLength);
    }

/* ---------- Read -----------------------------------------------------*/

/* ---------- OpenRIFF --------------------------------------------------*/
IFFP OpenRIFF (BPTR file0, GroupContext* new0, ClientFrame* clientFrame) 
{
  BPTR file          = file0;
  GroupContext* newc = new0;
  IFFP iffp          = IFF_OKAY;
  
  newc->parent       = NULL;		/* "whole file" has no parent.*/
  newc->clientFrame  = clientFrame;
  newc->file         = file;
  newc->position     = 0;
  newc->ckHdr.ckID   = newc->subtype    = NULL_CHUNK;
  newc->ckHdr.ckSize = newc->bytesSoFar = 0;
  
  /* Set newc->bound and go to the file's beginning. */
  newc->bound = FileLength (file);
  if (newc->bound < 0)
    {
      iffp = newc->bound;		   
    }
  else 
    {
      /* File system error! */
      if ( newc->bound < sizeof(ChunkHeader) )
	{
	  iffp = NOT_IFF;			   /* Too small for an IFF file. */
	}
      else
	{
	  GSeek(file, 0, OFFSET_BEGINNING);  /* Go to file start. */
	}
    }
  return(iffp);
}

/* ---------- OpenRGroup -----------------------------------------------*/
IFFP OpenRGroup (GroupContext* parent0, GroupContext* new0) 
{
  GroupContext *parent = parent0;
  GroupContext *newc   = new0;
  IFFP iffp            = IFF_OKAY;

  newc->parent       = parent;
  newc->clientFrame  = parent->clientFrame;
  newc->file         = parent->file;
  newc->position     = parent->position;
  newc->bound        = parent->position + ChunkMoreBytes(parent);
  newc->ckHdr.ckID   = newc->subtype    = NULL_CHUNK;
  newc->ckHdr.ckSize = newc->bytesSoFar = 0;

  if ( newc->bound > parent->bound  ||  IS_ODD(newc->bound) )
    {
      iffp = BAD_IFF;
    }
  return(iffp);
}
/* ---------- CloseRGroup -----------------------------------------------*/
IFFP CloseRGroup (GroupContext* context) 
{
  iff_file_offset_t position;

  if (context->parent == NULL) 
    {
    }  /* Context for whole file.*/
  else 
    {
      position = context->position;
      context->parent->bytesSoFar += position - context->parent->position;
      context->parent->position = position;
    }
  return(IFF_OKAY);
}

/* ---------- SkipFwd --------------------------------------------------*/
/* Skip over bytes in a context. Won't go backwards.*/
/* Updates context->position but not context->bytesSoFar.*/
/* This implementation is AmigaDOS specific.*/
IFFP SkipFwd (GroupContext* context, iff_file_offset_t bytes) 
{
  IFFP iffp = IFF_OKAY;
  
  if (bytes > 0) 
    {
      if (-1 == GSeek(context->file, bytes, OFFSET_CURRENT))
	{
	  iffp = BAD_IFF;	/* Ran out of bytes before chunk complete.*/
	}
      else
	{
	  context->position += bytes;
	}
      }
    return(iffp);
}

/* ---------- GetChunkHdr ----------------------------------------------*/
iff_id_t GetChunkHdr (GroupContext *context0)  {
    GroupContext *context = context0;
    IFFP iffp;
    long remaining;

    /* Skip remainder of previous chunk & padding. */
    iffp = SkipFwd(context,
	ChunkMoreBytes(context) + IS_ODD(context->ckHdr.ckSize));
    CheckIFFP();

    /* Set up to read the new header. */
    context->ckHdr.ckID = BAD_IFF;	/* Until we know it's okay, mark it BAD.*/
    context->subtype    = NULL_CHUNK;
    context->bytesSoFar = 0;

    /* Generate a psuedo-chunk if at end-of-context. */
    remaining = context->bound - context->position;
    if (remaining == 0) {
	context->ckHdr.ckSize = 0;
	context->ckHdr.ckID   = END_MARK;
	}

    /* BAD_IFF if not enough bytes in the context for a ChunkHeader.*/
    else if (sizeof(ChunkHeader) > remaining) {
	context->ckHdr.ckSize = remaining;
	}

    /* Read the chunk header (finally). */
    else {
        switch (
	    GRead(context->file, (iff_data_t *)&context->ckHdr, sizeof(ChunkHeader))
	    ) {
	    case -1: return(context->ckHdr.ckID = DOS_ERROR);
	    case 0:  return(context->ckHdr.ckID = BAD_IFF);
	    }

	/* Check: Top level chunk must be LIST or FORM or CAT. */
	if (context->parent == NULL)
	    switch(context->ckHdr.ckID) {
		case FORM:  case LIST:  case CAT:  break;
		default:    return(context->ckHdr.ckID = NOT_IFF);
		}

	/* Update the context. */
	context->position += sizeof(ChunkHeader);
	remaining         -= sizeof(ChunkHeader);

	/* Non-positive ID values are illegal and used for error codes.*/
	/* We could check for other illegal IDs...*/
	if (context->ckHdr.ckID <= 0)
	     context->ckHdr.ckID = BAD_IFF;

	/* Check: ckSize negative or larger than # bytes left in context? */
	else if (context->ckHdr.ckSize < 0  ||
		 context->ckHdr.ckSize > remaining) {
	    context->ckHdr.ckSize = remaining;
	    context->ckHdr.ckID   = BAD_IFF;
	    }

	/* Automatically read the LIST, FORM, PROP, or CAT subtype ID */
	else switch (context->ckHdr.ckID) {
	    case LIST:  case FORM:  case PROP:  case CAT:  {
		iffp = IFFReadBytes(context,
				    (iff_data_t *)&context->subtype,
				    sizeof(iff_id_t));
		if (iffp != IFF_OKAY)
		    context->ckHdr.ckID = iffp;
		break; }
	    }

	}
    return(context->ckHdr.ckID);
    }

/* ---------- IFFReadBytes ---------------------------------------------*/
IFFP IFFReadBytes(GroupContext *context,   iff_data_t *buffer,   iff_size_t nBytes) {
    IFFP iffp = IFF_OKAY;

    if (nBytes < 0)
	iffp = CLIENT_ERROR;
    else if (nBytes > ChunkMoreBytes(context))
	iffp = SHORT_CHUNK;
    else if (nBytes > 0)
	switch ( GRead(context->file, buffer, nBytes) ) {
	    case -1: {iffp = DOS_ERROR; break; }
	    case 0:  {iffp = BAD_IFF;   break; }
	    default: {
		context->position   += nBytes;
		context->bytesSoFar += nBytes;
		}
	    }

    return(iffp);
    }

/* ---------- SkipGroup ------------------------------------------------*/
IFFP SkipGroup(GroupContext *context)  {
  return IFF_OKAY;
    }	/* Nothing to do, thanks to GetChunkHdr */

/* ---------- ReadIFF --------------------------------------------------*/
IFFP ReadIFF(BPTR file,  ClientFrame *clientFrame)  {
    /*CompilerBug register*/ 
  IFFP iffp;
    GroupContext context;

    iffp = OpenRIFF(file, &context, 0);
    context.clientFrame = clientFrame;

    if (iffp == IFF_OKAY)
	switch (iffp = GetChunkHdr(&context)) {
	    case FORM: { iffp = (*clientFrame->getForm)(&context); break; }
	    case LIST: { iffp = (*clientFrame->getList)(&context); break; }
	    case CAT : { iffp = (*clientFrame->getCat )(&context); break; }
	    /* default: Includes IFF_DONE, BAD_IFF, NOT_IFF... */
	    }

    CloseRGroup(&context);

    if (iffp > 0)		/* Make sure we don't return an ID.*/
	iffp = NOT_IFF;		/* GetChunkHdr should've caught this.*/
    return(iffp);
    }

/* ---------- ReadIList ------------------------------------------------*/
IFFP ReadIList(GroupContext *parent,  ClientFrame *clientFrame) {
    GroupContext listContext;
    IFFP iffp;
    bool propOk = true;

    iffp = OpenRGroup(parent, &listContext);
    CheckIFFP();

    /* One special case test lets us handle CATs as well as LISTs.*/
    if (parent->ckHdr.ckID == CAT)
	propOk = false;
    else
	listContext.clientFrame = clientFrame;

    do {
	switch (iffp = GetChunkHdr(&listContext)) {
	    case PROP: {
		if (propOk)
		    iffp = (*clientFrame->getProp)(&listContext);
		else
		    iffp = BAD_IFF;
		break;
		}
	    case FORM: { iffp = (*clientFrame->getForm)(&listContext); break; }
	    case LIST: { iffp = (*clientFrame->getList)(&listContext); break; }
	    case CAT : { iffp = (*clientFrame->getCat )(&listContext); break; }
	    /* default: Includes END_MARK, IFF_DONE, BAD_IFF, NOT_IFF... */
	    }
	if (listContext.ckHdr.ckID != PROP)
	    propOk = false;	/* No PROPs allowed after this point.*/
	} while (iffp == IFF_OKAY);

    CloseRGroup(&listContext);

    if (iffp > 0)	/* Only chunk types above are allowed in a LIST/CAT.*/
	iffp = BAD_IFF;
    return(iffp == END_MARK ? IFF_OKAY : iffp);
    }

/* ---------- ReadICat -------------------------------------------------*/
/* By special arrangement with the ReadIList implement'n, this is trivial.*/
IFFP ReadICat(GroupContext *parent)  {
    return( ReadIList(parent, NULL) );
    }

/* ---------- GetFChunkHdr ---------------------------------------------*/
iff_id_t GetFChunkHdr(GroupContext *context) {
    iff_id_t id;

    id = GetChunkHdr(context);
    if (id == PROP)
	context->ckHdr.ckID = id = BAD_IFF;
    return(id);
    }

/* ---------- GetF1ChunkHdr ---------------------------------------------*/
iff_id_t GetF1ChunkHdr(GroupContext *context) {
    iff_id_t id;
    ClientFrame *clientFrame = context->clientFrame;

    switch (id = GetChunkHdr(context))  {
	case PROP: { id = BAD_IFF; break; }
	case FORM: { id = (*clientFrame->getForm)(context); break; }
	case LIST: { id = (*clientFrame->getList)(context); break; }
	case CAT : { id = (*clientFrame->getCat )(context); break; }
	/* Default: let the caller handle other chunks */
	}
    return(context->ckHdr.ckID = id);
    }

/* ---------- GetPChunkHdr ---------------------------------------------*/
iff_id_t GetPChunkHdr(GroupContext *context) {
    iff_id_t id;

    id = GetChunkHdr(context);
    switch (id) {
	case LIST:  case FORM:  case PROP:  case CAT:  {
	    id = context->ckHdr.ckID = BAD_IFF;
	    break; }
	}
    return(id);
    }
 
