#include "common.h"
// seg 20
void theEnd(int type, char* name)
  printf("Error: %s\n", name);
  exit(1);
char* loadFromItd(char* name)
  fHandle = fopen(name,"rb");
  if(!fHandle)
  fseek(fHandle,0,SEEK_END);
  ptr = (char*)malloc(fileSize);
  fread(ptr,fileSize,1,fHandle);
  fclose(fHandle);
  return(ptr);

char* loadPakSafe(char* name, int index)
  ptr = loadPak(name, index);
  if(!ptr)
  return ptr;