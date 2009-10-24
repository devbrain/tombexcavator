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

#include "SDL/SDL.h"
#include "convert.h"

unsigned int loaduint16(unsigned char c1, unsigned char c2){
    return (((unsigned int)c1 * 256) & 0xFF00) + (unsigned int)c2;
}

int loadint16(unsigned char c1, unsigned char c2){
    short int tmpint = ((short int)c1 << 8) + (short int)c2;
    return (int)tmpint;
}
