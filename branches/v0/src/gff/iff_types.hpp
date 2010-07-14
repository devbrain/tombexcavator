#ifndef __GFF_IFF_TYPES_HPP__
#define __GFF_IFF_TYPES_HPP__

#include <stdio.h>
#include "abl/types/types.hpp"

typedef uint32_t iff_id_t;	/* An ID is four printable ASCII chars but
			 * stored as a LONG for efficient copy & compare.*/
typedef uint32_t iff_size_t;
typedef uint8_t  iff_data_t;
typedef long     iff_file_offset_t;

typedef FILE* BPTR;


#endif
