#ifndef GIO_H
#define GIO_H
#define GIO_ACTIVE 0
#ifndef COMPILER_H
#include "compiler.h"
#endif
#ifndef LIBRARIES_DOS_H
#include "libraries/dos.h"
#endif
#ifndef OFFSET_BEGINNING
#define OFFSET_BEGINNING OFFSET_BEGINING
#endif
#if GIO_ACTIVE
#ifdef FDwAT
extern BPTR GOpen(char *, LONG);
extern LONG GClose(BPTR);
extern LONG GRead(BPTR, BYTE *, LONG);
extern LONG GWriteFlush(BPTR);
extern LONG GWriteDeclare(BPTR, BYTE *, LONG);
extern LONG GWrite(BPTR, BYTE *, LONG);
extern LONG GSeek(BPTR, LONG, LONG);
#else
extern BPTR GOpen();
extern LONG GClose();
extern LONG GRead();
extern LONG GWriteFlush();
extern LONG GWriteDeclare();
extern LONG GWrite();
extern LONG GSeek();
#endif FDwAT
#else
#define GOpen(filename, openmode) Open(filename, (long)openmode)
#define GClose(file) Close(file)
#define GRead(file, buffer, nBytes) Read(file, buffer, (long)nBytes)
#define GWriteFlush(file) (0L)
#define GWriteDeclare(file, buffer, nBytes) (0L)
#define GWrite(file, buffer, nBytes) Write(file, buffer, (long)nBytes)
#define GSeek(file, position, mode) Seek(file, (long)position, (long)mode)
#endif GIO_ACTIVE
#define GWriteUndeclare(file) GWriteDeclare(file, (long)NULL, 0L)
#endif
