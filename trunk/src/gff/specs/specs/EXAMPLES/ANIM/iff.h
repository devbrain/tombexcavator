#ifndef IFF_H
#define IFF_H
#ifndef COMPILER_H
#include "compiler.h"
#endif
#ifndef LIBRARIES_DOS_H
#include "libraries/dos.h"
#endif
#ifndef OFFSET_BEGINNING
#define OFFSET_BEGINNING OFFSET_BEGINING
#endif
typedef LONG IFFP;
#define IFF_OKAY 0L
#define END_MARK -1L
#define IFF_DONE -2L
#define DOS_ERROR -3L
#define NOT_IFF -4L
#define NO_FILE -5L
#define CLIENT_ERROR -6L
#define BAD_FORM -7L
#define SHORT_CHUNK -8L
#define BAD_IFF -9L
#define LAST_ERROR BAD_IFF
#define CheckIFFP() { if (iffp != IFF_OKAY) return(iffp); }
typedef LONG ID;
#define MakeID(a,b,c,d) ( (LONG)(a)<<24L | (LONG)(b)<<16L | (c)<<8 | (d) )
#define FORM MakeID('F','O','R','M')
#define PROP MakeID('P','R','O','P')
#define LIST MakeID('L','I','S','T')
#define CAT MakeID('C','A','T',' ')
#define FILLER MakeID(' ',' ',' ',' ')
#define NULL_CHUNK 0L
typedef struct {
ID ckID;
LONG ckSize;
} ChunkHeader;
typedef struct {
ID ckID;
LONG ckSize;
UBYTE ckData[ 1 ];
} Chunk;
#define szNotYetKnown 0x80000001L
#define IS_ODD(a) ((a) & 1)
#define WordAlign(size) ((size+1)&~1)
#define ChunkPSize(dataSize) (long)(WordAlign(dataSize) + sizeof(ChunkHeader))
typedef struct {
ID ckID;
LONG ckSize;
ID grpSubID;
} GroupHeader;
typedef struct {
ID ckID;
LONG ckSize;
ID grpSubID;
UBYTE grpData[ 1 ];
} GroupChunk;
#ifdef FDwAT
typedef IFFP ClientProc(struct _GroupContext *);
#else
typedef IFFP ClientProc();
#endif
typedef struct _ClientFrame {
ClientProc *getList, *getProp, *getForm, *getCat;
} ClientFrame;
typedef struct _GroupContext {
struct _GroupContext *parent;
ClientFrame *clientFrame;
BPTR file;
LONG position;
LONG bound;
ChunkHeader ckHdr;
ID subtype;
LONG bytesSoFar;
} GroupContext;
#define ChunkMoreBytes(gc) ((gc)->ckHdr.ckSize - (gc)->bytesSoFar)
#ifdef FDwAT
extern IFFP OpenRIFF(BPTR, GroupContext *, ClientFrame *);
extern IFFP OpenRGroup(GroupContext *, GroupContext *);
extern IFFP CloseRGroup(GroupContext *);
extern ID GetChunkHdr(GroupContext *);
extern IFFP IFFReadBytes(GroupContext *, BYTE *, LONG);
extern IFFP SkipGroup(GroupContext *);
extern IFFP ReadIFF(BPTR, ClientFrame *);
extern IFFP ReadIList(GroupContext *, ClientFrame *);
extern IFFP ReadICat(GroupContext *);
extern ID GetFChunkHdr(GroupContext *);
extern ID GetF1ChunkHdr(GroupContext *);
extern ID GetPChunkHdr(GroupContext *);
#else
extern IFFP OpenRIFF();
extern IFFP OpenRGroup();
extern IFFP CloseRGroup();
extern ID GetChunkHdr();
extern IFFP IFFReadBytes();
extern IFFP SkipGroup();
extern IFFP ReadIFF();
extern IFFP ReadIList();
extern IFFP ReadICat();
extern ID GetFChunkHdr();
extern ID GetF1ChunkHdr();
extern ID GetPChunkHdr();
#endif
#ifdef FDwAT
extern IFFP OpenWIFF(BPTR, GroupContext *, LONG);
extern IFFP StartWGroup(GroupContext *, ID, LONG, ID, GroupContext *);
extern IFFP EndWGroup(GroupContext *);
extern IFFP OpenWGroup(GroupContext *, GroupContext *);
extern IFFP CloseWGroup(GroupContext *);
extern IFFP PutCk(GroupContext *, ID, LONG, BYTE *);
extern IFFP PutCkHdr(GroupContext *, ID, LONG);
extern IFFP IFFWriteBytes(GroupContext *, BYTE *, LONG);
extern IFFP PutCkEnd(GroupContext *);
#else
extern IFFP OpenWIFF();
extern IFFP StartWGroup();
extern IFFP EndWGroup();
extern IFFP OpenWGroup();
extern IFFP CloseWGroup();
extern IFFP PutCk();
extern IFFP PutCkHdr();
extern IFFP IFFWriteBytes();
extern IFFP PutCkEnd();
#endif
#endif IFF_H
