/*   
 * Copyright (C) 2008 Eirik Stople
 *
 * OpenTitus is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 3  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "sqz.h"

int main(int argc, char *argv[]){
    unsigned char *test;
    int retval;

    retval = unSQZ(argv[1], &test);
    printf("retval = %x\n", retval);
    printf("MORDI\n");
/*
    printf("test[0] = %d\n", test[0]);
    printf("test[1] = %d\n", test[1]);
return (-1);
*/

if (test == NULL)
    printf("test = NULL\n");

 /*   test[1] = 5;
    test[1] = 2;
    test[2] = 7;
    test[3] = 6;*/

    if (retval > 0) {
        printf("MORDI2\n");
        FILE *ofp;
        ofp = fopen("output.lvl", "wb");
        if (ofp == NULL) {
            fprintf(stderr, "Can't open output file!\n");
            free (test);
            return(-1);
        }
        printf("MORDI3\n");
        printf("retval = %d\n", retval);
        fwrite(test, 1, retval, ofp);
        /*fprintf(ofp, "%*s", retval, test); / *18344retval);*/
        printf("MORDI4\n");
        fclose (ofp);
    }
    printf("MORDI5\n");
    printf("b1 = %u\n", test[1]);
if (test == NULL)
    printf("test(2) = NULL\n");
    free (test);
    printf("MORDI6\n");
    return 0;
}
