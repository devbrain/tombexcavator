/*
 * Copyright (C) 2003 Maxim Stepin ( maxst@hiend3d.com )
 *
 * Copyright (C) 2010 Cameron Zemek ( grom@zeminvaders.net)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __HQX_H_
#define __HQX_H_

/* only use 6 most significant bits from RGB color components
 * (VGA colors) */
#define HQX_VGACOLORS

#if defined( __GNUC__ )
    #ifdef __MINGW32__
        #define HQX_CALLCONV __stdcall
    #else
        #define HQX_CALLCONV
    #endif
#else
    #define HQX_CALLCONV
#endif

#if defined(_WIN32)
    #ifdef DLL_EXPORT
        #define HQX_API __declspec(dllexport)
    #else
        /* #define HQX_API __declspec(dllimport) */
		/* statically link HQX */
	    #define HQX_API
    #endif
#else
    #define HQX_API
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
/* MS Visual C++ 2008 doesn't include stdint.h */
#include "types.h"
typedef uint32 uint32_t;
typedef uint8 uint8_t;
#else
#include <stdint.h>
#endif

HQX_API void HQX_CALLCONV hqxInit(void);
HQX_API void HQX_CALLCONV hqxUnInit(void);
HQX_API void HQX_CALLCONV hq2x_32( uint32_t * src, uint32_t * dest, int width, int height );
HQX_API void HQX_CALLCONV hq3x_32( uint32_t * src, uint32_t * dest, int width, int height );
HQX_API void HQX_CALLCONV hq4x_32( uint32_t * src, uint32_t * dest, int width, int height );

HQX_API void HQX_CALLCONV hq2x_32_rb( uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );
HQX_API void HQX_CALLCONV hq3x_32_rb( uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );
HQX_API void HQX_CALLCONV hq4x_32_rb( uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );

HQX_API void HQX_CALLCONV hq2x_8to32_rb( const uint8_t * sp, uint32_t srb, uint32_t * dp, uint32_t drb, int Xres, int Yres, const uint32_t * rgb_palette );
HQX_API void HQX_CALLCONV hq2x_8to32( const uint8_t * sp, uint32_t * dp, int Xres, int Yres, const uint32_t * rgb_palette );

HQX_API void HQX_CALLCONV hq3x_8to32_rb( const uint8_t * sp, uint32_t srb, uint32_t * dp, uint32_t drb, int Xres, int Yres, const uint32_t * rgb_palette );
HQX_API void HQX_CALLCONV hq3x_8to32( const uint8_t * sp, uint32_t * dp, int Xres, int Yres, const uint32_t * rgb_palette );

HQX_API void HQX_CALLCONV hq4x_8to32_rb( const uint8_t * sp, uint32_t srb, uint32_t * dp, uint32_t drb, int Xres, int Yres, const uint32_t * rgb_palette );
HQX_API void HQX_CALLCONV hq4x_8to32( const uint8_t * sp, uint32_t * dp, int Xres, int Yres, const uint32_t * rgb_palette );
#endif
