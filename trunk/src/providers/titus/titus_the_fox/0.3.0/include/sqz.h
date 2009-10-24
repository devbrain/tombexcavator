#ifndef SQZ_H
#define SQZ_H

#define LZW_CLEAR_CODE 0x100
#define LZW_END_CODE   0x101
#define LZW_FIRST      0x102
#define LZW_MAX_TABLE  4096

int unSQZ(char *inputfile, unsigned char **output);

#endif
