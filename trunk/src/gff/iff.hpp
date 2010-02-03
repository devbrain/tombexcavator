#ifndef __GFF_IFF_HPP__
#define __GFF_IFF_HPP__




#include "gff/gff_export.hpp"
#include "gff/iff_types.hpp"



typedef long IFFP;	/* Status code result from an IFF procedure */
	/* LONG, because must be type compatable with ID for GetChunkHdr.*/
	/* Note that the error codes below are not legal IDs.*/
#define IFF_OKAY   0L	/* Keep going...*/
#define END_MARK  -1L	/* As if there was a chunk at end of group.*/
#define IFF_DONE  -2L	/* clientProc returns this when it has READ enough.
			 * It means return thru all levels. File is Okay.*/
#define DOS_ERROR -3L
#define NOT_IFF   -4L	/* not an IFF file.*/
#define NO_FILE   -5L	/* Tried to open file, DOS didn't find it.*/
#define CLIENT_ERROR -6L /* Client made invalid request, for instance, write
			 * a negative size chunk.*/
#define BAD_FORM  -7L	/* A client read proc complains about FORM semantics;
			 * e.g. valid IFF, but missing a required chunk.*/
#define SHORT_CHUNK -8L	/* Client asked to IFFReadBytes more bytes than left
			 * in the chunk. Could be client bug or bad form.*/
#define BAD_IFF   -9L	/* mal-formed IFF file. [TBD] Expand this into a
			 * range of error codes.*/
#define LAST_ERROR BAD_IFF 

/* ---------- ID -------------------------------------------------------*/


/* Four-character IDentifier builder.*/
#define MakeID(a,b,c,d)  ( (uint32_t)(a)<<24L | (uint32_t)(b)<<16L | (c)<<8 | (d) )

/* Standard group IDs.  A chunk with one of these IDs contains a
   SubTypeID followed by zero or more chunks.*/
#define FORM   MakeID('F','O','R','M')
#define PROP   MakeID('P','R','O','P')
#define LIST   MakeID('L','I','S','T')
#define CAT    MakeID('C','A','T',' ')
#define FILLER MakeID(' ',' ',' ',' ')
/* The IDs "FOR1".."FOR9", "LIS1".."LIS9", & "CAT1".."CAT9" are reserved
 * for future standardization.*/

/* Pseudo-ID used internally by chunk reader and writer.*/
#define NULL_CHUNK 0L	       /* No current chunk.*/ 


/* ---------- Chunk ----------------------------------------------------*/

/* All chunks start with a type ID and a count of the data bytes that 
   follow--the chunk's "logicl size" or "data size". If that number is odd,
   a 0 pad byte is written, too. */
typedef struct 
{
  iff_id_t    ckID;
  iff_size_t  ckSize;
} ChunkHeader;

typedef struct 
{
  iff_id_t   ckID;
  iff_size_t ckSize;
  iff_data_t ckData[ 1 /*REALLY: ckSize*/ ];
} Chunk;

/* Pass ckSize = szNotYetKnown to the writer to mean "compute the size".*/
#define szNotYetKnown 0x80000001L

/* Need to know whether a value is odd so can word-align.*/
#define IS_ODD(a)   ((a) & 1)

/* This macro rounds up to an even number. */
#define WordAlign(size)   ((size+1)&~1)

/* ALL CHUNKS MUST BE PADDED TO EVEN NUMBER OF BYTES.
 * ChunkPSize computes the total "physical size" of a padded chunk from
 * its "data size" or "logical size". */
#define ChunkPSize(dataSize)  (WordAlign(dataSize) + sizeof(ChunkHeader))

/* The Grouping chunks (LIST, FORM, PROP, & CAT) contain concatenations of
 * chunks after a subtype ID that identifies the content chunks.
 * "FORM type XXXX", "LIST of FORM type XXXX", "PROPerties associated
 * with FORM type XXXX", or "conCATenation of XXXX".*/
typedef struct 
{
  iff_id_t    ckID;
  iff_size_t  ckSize;	/* this ckSize includes "grpSubID".*/
  iff_id_t    grpSubID;
} GroupHeader;

typedef struct 
{
    iff_id_t    ckID;
    iff_size_t  ckSize;
    iff_id_t    grpSubID;
    iff_data_t  grpData[ 1 /*REALLY: ckSize-sizeof(grpSubID)*/ ];
} GroupChunk;
 
/* ---------- IFF Reader -----------------------------------------------*/

/******** Routines to support a stream-oriented IFF file reader *******
 *
 * These routines handle lots of details like error checking and skipping
 * over padding. They're also careful not to read past any containing context.
 *
 * These routines ASSUME they're the only ones reading from the file.
 * Client should check IFFP error codes. Don't press on after an error!
 * These routines try to have no side effects in the error case, except
 * partial I/O is sometimes unavoidable.
 *
 * All of these routines may return DOS_ERROR. In that case, ask DOS for the
 * specific error code.
 *
 * The overall scheme for the low level chunk reader is to open a "group read
 * context" with OpenRIFF or OpenRGroup, read the chunks with GetChunkHdr
 * (and its kin) and IFFReadBytes, and close the context with CloseRGroup.
 *
 * The overall scheme for reading an IFF file is to use ReadIFF, ReadIList,
 * and ReadICat to scan the file. See those procedures, ClientProc (below),
 * and the skeleton IFF reader. */

/* Client passes ptrs to procedures of this type to ReadIFF which call them
 * back to handle LISTs, FORMs, CATs, and PROPs.
 *
 * Use the GroupContext ptr when calling reader routines like GetChunkHdr.
 * Look inside the GroupContext ptr for your ClientFrame ptr. You'll
 * want to type cast it into a ptr to your containing struct to get your
 * private contextual data (stacked property settings). See below. */

struct _GroupContext;

typedef IFFP ClientProc(struct _GroupContext *);

/* Client's context for reading an IFF file or a group.
 * Client should actually make this the first component of a larger struct
 * (it's personal stack "frame") that has a field to store each "interesting"
 * property encountered.
 * Either initialize each such field to a global default or keep a boolean
 * indicating if you've read a property chunk into that field.
 * Your getList and getForm procs should allocate a new "frame" and copy the
 * parent frame's contents. The getProp procedure should store into the frame
 * allocated by getList for the containing LIST. */
typedef struct _ClientFrame 
{
  ClientProc *getList, *getProp, *getForm, *getCat;
    /* client's own data follows; place to stack property settings */
} ClientFrame;

/* Our context for reading a group chunk. */
typedef struct _GroupContext 
{
    struct _GroupContext *parent; /* Containing group; NULL => whole file. */
    ClientFrame *clientFrame;     /* Reader data & client's context state. */
    BPTR file;		/* Byte-stream file handle. */
    iff_file_offset_t position;	/* The context's logical file position. */
    iff_file_offset_t bound;		/* File-absolute context bound
			 * or szNotYetKnown (writer only). */
    ChunkHeader ckHdr;	/* Current chunk header. ckHdr.ckSize = szNotYetKnown
			 * means we need to go back and set the size (writer only).
			 * See also Pseudo-IDs, above. */
    iff_id_t subtype;		/* Group's subtype ID when reading. */
    iff_size_t bytesSoFar;	/* # bytes read/written of current chunk's data. */
    } GroupContext;

/* Computes the number of bytes not yet read from the current chunk, given
 * a group read context gc. */
#define ChunkMoreBytes(gc)  ((gc)->ckHdr.ckSize - (gc)->bytesSoFar)



/***** Low Level IFF Chunk Reader *****/


/* Given an open file, open a read context spanning the whole file.
 * This is normally only called by ReadIFF.
 * This sets new->clientFrame = clientFrame.
 * ASSUME context allocated by caller but not initialized.
 * ASSUME caller doesn't deallocate the context before calling CloseRGroup.
 * NOT_IFF ERROR if the file is too short for even a chunk header.*/
GFF_API IFFP OpenRIFF(BPTR, GroupContext *, ClientFrame *);
	         /*  file, new,            clientFrame  */

/* Open the remainder of the current chunk as a group read context.
 * This will be called just after the group's subtype ID has been read
 * (automatically by GetChunkHdr for LIST, FORM, PROP, and CAT) so the
 * remainder is a sequence of chunks.
 * This sets new->clientFrame = parent->clientFrame. The caller should repoint
 * it at a new clientFrame if opening a LIST context so it'll have a "stack
 * frame" to store PROPs for the LIST. (It's usually convenient to also
 * allocate a new Frame when you encounter FORM of the right type.)
 *
 * ASSUME new context allocated by caller but not initialized.
 * ASSUME caller doesn't deallocate the context or access the parent context
 * before calling CloseRGroup.
 * BAD_IFF ERROR if context end is odd or extends past parent. */
GFF_API IFFP OpenRGroup(GroupContext *, GroupContext *);
		   /*  parent,         new  */

/* Close a group read context, updating its parent context.
 * After calling this, the old context may be deallocated and the parent
 * context can be accessed again. It's okay to call this particular procedure
 * after an error has occurred reading the group.
 * This always returns IFF_OKAY. */
GFF_API IFFP CloseRGroup(GroupContext *);
		    /*  old  */

/* Skip any remaining bytes of the previous chunk and any padding, then
 * read the next chunk header into context.ckHdr.
 * If the ckID is LIST, FORM, CAT, or PROP, this automatically reads the
 * subtype ID into context->subtype.
 * Caller should dispatch on ckID (and subtype) to an appropriate handler.
 *
 * RETURNS context.ckHdr.ckID (the ID of the new chunk header); END_MARK
 * if there are no more chunks in this context; or NOT_IFF if the top level
 * file chunk isn't a FORM, LIST, or CAT; or BAD_IFF if malformed chunk, e.g.
 * ckSize is negative or too big for containing context, ckID isn't positive,
 * or we hit end-of-file.
 *
 * See also GetFChunkHdr, GetF1ChunkHdr, and GetPChunkHdr, below.*/
GFF_API iff_id_t       GetChunkHdr(GroupContext *);
  /*  context.ckHdr.ckID    context  */

/* Read nBytes number of data bytes of current chunk. (Use OpenGroup, etc.
 * instead to read the contents of a group chunk.) You can call this several
 * times to read the data piecemeal.
 * CLIENT_ERROR if nBytes < 0. SHORT_CHUNK if nBytes > ChunkMoreBytes(context)
 * which could be due to a client bug or a chunk that's shorter than it
 * ought to be (bad form). (on either CLIENT_ERROR or SHORT_CHUNK,
 * IFFReadBytes won't read any bytes.) */
GFF_API IFFP IFFReadBytes(GroupContext *, iff_data_t *, iff_size_t);
		     /*  context,        buffer, nBytes  */


/***** IFF File Reader *****/

/* This is a noop ClientProc that you can use for a getList, getForm, getProp,
 * or getCat procedure that just skips the group. A simple reader might just
 * implement getForm, store ReadICat in the getCat field of clientFrame, and
 * use SkipGroup for the getList and getProp procs.*/
GFF_API IFFP SkipGroup(GroupContext *);

/* IFF file reader.
 * Given an open file, allocate a group context and use it to read the FORM,
 * LIST, or CAT and it's contents. The idea is to parse the file's contents,
 * and for each FORM, LIST, CAT, or PROP encountered, call the getForm,
 * getList, getCat, or getProp procedure in clientFrame, passing the
 * GroupContext ptr.
 * This is achieved with the aid of ReadIList (which your getList should
 * call) and ReadICat (which your getCat should call, if you don't just use
 * ReadICat for your getCat). If you want to handle FORMs, LISTs, and CATs
 * nested within FORMs, the getForm procedure must dispatch to getForm,
 * getList, and getCat (it can use GetF1ChunkHdr to make this easy).
 *
 * Normal return is IFF_OKAY (if whole file scanned) or IFF_DONE (if a client
 * proc said "done" first).
 * See the skeletal getList, getForm, getCat, and getProp procedures. */
GFF_API IFFP ReadIFF(BPTR, ClientFrame *);
                /*  file, clientFrame  */

/* IFF LIST reader.
 * Your "getList" procedure should allocate a ClientFrame, copy the parent's
 * ClientFrame, and then call this procedure to do all the work.
 *
 * Normal return is IFF_OKAY (if whole LIST scanned) or IFF_DONE (if a client
 * proc said "done" first).
 * BAD_IFF ERROR if a PROP appears after a non-PROP. */
GFF_API IFFP ReadIList(GroupContext *, ClientFrame *);
		  /*  parent,         clientFrame  */

/* IFF CAT reader.
 * Most clients can simply use this to read their CATs. If you must do extra
 * setup work, put a ptr to your getCat procedure in the clientFrame, and
 * have that procedure call ReadICat to do the detail work.
 *
 * Normal return is IFF_OKAY (if whole CAT scanned) or IFF_DONE (if a client
 * proc said "done" first).
 * BAD_IFF ERROR if a PROP appears in the CAT. */
GFF_API IFFP ReadICat(GroupContext *);
		 /*  parent  */

/* Call GetFChunkHdr instead of GetChunkHdr to read each chunk inside a FORM.
 * It just calls GetChunkHdr and returns BAD_IFF if it gets a PROP chunk. */
GFF_API iff_id_t	GetFChunkHdr(GroupContext *);
  /*  context.ckHdr.ckID    context  */

/* GetF1ChunkHdr is like GetFChunkHdr, but it automatically dispatches to the
 * getForm, getList, and getCat procedure (and returns the result) if it
 * encounters a FORM, LIST, or CAT. */
GFF_API iff_id_t	GetF1ChunkHdr(GroupContext *);
  /*  context.ckHdr.ckID    context  */

/* Call GetPChunkHdr instead of GetChunkHdr to read each chunk inside a PROP.
 * It just calls GetChunkHdr and returns BAD_IFF if it gets a group chunk. */
GFF_API iff_id_t	GetPChunkHdr(GroupContext *);
  /*  context.ckHdr.ckID    context  */


#define CheckIFFP()   { if (iffp != IFF_OKAY) return(iffp); } 
 
#endif
