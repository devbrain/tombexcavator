#ifndef __GFF_GIO_HPP__
#define __GFF_GIO_HPP__

#include "gff/iff_types.hpp"

#define OFFSET_BEGINNING SEEK_SET
#define OFFSET_CURRENT   SEEK_CUR
/* Present for completeness in the interface.
 * "openmode" is either MODE_OLDFILE to read/write an existing file, or
 * MODE_NEWFILE to write a new file.
 * RETURNs a "file" pointer to a system-supplied structure that describes
 * the open file.  This pointer is passed in to the other routines below.*/
extern BPTR GOpen(char* filename, long openmode);

/* NOTE: Flushes & Frees the write buffer.
 * Returns -1 on error from Write.*/
extern long GClose(BPTR file);

/* Read not speeded-up yet.
 * GOpen the file, then do GReads to get successive chunks of data in
 * the file.  Assumes the system can handle any number of bytes in each
 * call, regardless of any block-structure of the device being read from.
 * When done, GClose to free any system resources associated with an
 * open file.*/
extern iff_size_t GRead(BPTR file, iff_data_t* buffer, iff_size_t nBytes);

/* Writes out any data in write buffer for file.
 * NOTE WHEN have Seeked into middle of buffer:
 * GWriteFlush causes current position to be the end of the data written.
 * -1 on error from Write.*/
extern long GWriteFlush(BPTR file);

/* Sets up variables to describe a write buffer for the file.*/
/* If the buffer already has data in it from an outstanding GWriteDeclare,
 * then that buffer must first be flushed.
 * RETURN -1 on error from Write for that previous buffer flush.
 * See also "GWriteUndeclare".*/
extern long GWriteDeclare(BPTR file, iff_data_t* buffer, iff_size_t nBytes);

/* ANY PROGRAM WHICH USES "GWrite" MUST USE "GSeek" rather than "Seek"
 * TO SEEK ON A FILE BEING WRITTEN WITH "GWrite".
 * "Write" with Generic speed-up.
 * -1 on error from Write.  else returns # bytes written to disk.
 * Call GOpen, then do successive GWrites with GSeeks if required,
 * then GClose when done.  (IFF does require GSeek.)*/
extern long GWrite(BPTR file, iff_data_t* buffer, iff_size_t nBytes);

/* "Seek" with Generic speed-up, for a file being written with GWrite.*/
/* Returns what Seek returns, which appears to be the position BEFORE
 * seeking, though the documentation says it returns the NEW position.
 * In fact, the code now explicitly returns the OLD position when
 * seeking within the buffer.
 * Eventually, will support two independent files, one being read, the
 * other being written.  Or could support even more.  Designed so is safe
 * to call even for files which aren't being buffered.*/
extern iff_file_offset_t GSeek(BPTR file, iff_file_offset_t position, int mode);


/* Release the buffer for that file, flushing it to disk if it has any
 * contents.  GWriteUndeclare(NULL) to release ALL buffers.
 * Currently, only one file can be buffered at a time anyway.*/
#define GWriteUndeclare(file)  GWriteDeclare(file, NULL, 0) 

#endif
