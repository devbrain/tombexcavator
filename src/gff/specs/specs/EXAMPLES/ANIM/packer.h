#ifndef PACKER_H
#define PACKER_H
#ifndef COMPILER_H
#include "compiler.h"
#endif
#define MaxPackedSize(rowSize) ( (rowSize) + ( ((rowSize)+127) >> 7 ) )
#ifdef FDwAT
extern LONG PackRow(BYTE **, BYTE **, LONG);
extern BOOL UnPackRow(BYTE **, BYTE **, WORD, WORD);
#else
extern LONG PackRow();
extern BOOL UnPackRow();
#endif
#endif
