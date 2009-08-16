/* 
 * File:   xload.cpp
 * Author: igorgu
 *
 * Created on 19 ������ 2009, 12:05
 */
/*
  General Layout :

header - 256 bytes
u0 - header.chunks * 32 bytes
u1 - 556 bytes
data

header : 256 bytes
   magic           : 4 bytes : 0x53 0x46 0x53 0x00 ("SFS")
   version         : 4 bytes : 0xC8 0x00 0x00 0x00
   header checksum : 4 bytes
   chunks          : 4 bytes : number of 32 byte chunks in the next u0 block
   start of u0     : 4 bytes
   start of u1     : 4 bytes
   start of u2     : 4 bytes
   unknown 3       : 4 bytes
   unknown 4       : 2 bytes
   Copyright       : 222 bytes : NULL terminated string
 */

#include <stdlib.h>


int main(int argc, char** argv) {

    return (EXIT_SUCCESS);
}

