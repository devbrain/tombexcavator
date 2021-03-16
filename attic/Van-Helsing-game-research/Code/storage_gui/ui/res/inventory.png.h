#ifndef INVENTORY_PNG_H
#define INVENTORY_PNG_H

#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/bitmap.h>

static const unsigned char inventory_png[] = 
{
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 
	0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x18, 
	0x00, 0x00, 0x00, 0x18, 0x08, 0x06, 0x00, 0x00, 0x00, 0xE0, 
	0x77, 0x3D, 0xF8, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 
	0x73, 0x00, 0x00, 0x0B, 0x13, 0x00, 0x00, 0x0B, 0x13, 0x01, 
	0x00, 0x9A, 0x9C, 0x18, 0x00, 0x00, 0x02, 0x20, 0x49, 0x44, 
	0x41, 0x54, 0x48, 0xC7, 0xA5, 0x56, 0xED, 0x6D, 0xE3, 0x30, 
	0x0C, 0x7D, 0x05, 0xF2, 0xFF, 0xD8, 0x09, 0xCC, 0x0D, 0xAA, 
	0x0D, 0xCC, 0x0D, 0xA2, 0x0D, 0xEA, 0x6E, 0x90, 0x0D, 0x9C, 
	0x0D, 0x3C, 0x42, 0x9B, 0x09, 0xAC, 0x4E, 0x60, 0x65, 0x02, 
	0xEB, 0x26, 0x90, 0x34, 0x81, 0xD5, 0x09, 0x74, 0x3F, 0x7A, 
	0x0A, 0x9C, 0xD4, 0xF9, 0x2C, 0x01, 0x02, 0x02, 0x2D, 0xF1, 
	0x91, 0x4F, 0x34, 0xA9, 0xA7, 0x9C, 0x33, 0x96, 0xC4, 0x5A, 
	0x2B, 0xCE, 0x39, 0x95, 0x52, 0xA2, 0xB9, 0x5D, 0x44, 0xAC, 
	0x52, 0xCA, 0x11, 0x51, 0xC2, 0x2D, 0x92, 0x73, 0xFE, 0xA1, 
	0x4D, 0xD3, 0xBC, 0x03, 0xC8, 0x97, 0x54, 0x6B, 0xDD, 0xF7, 
	0x7D, 0xAF, 0x97, 0xCE, 0xCF, 0x75, 0xD1, 0xC8, 0xCC, 0xFE, 
	0x1A, 0x40, 0x51, 0x66, 0xF6, 0xC3, 0x30, 0xC8, 0x39, 0x80, 
	0xA7, 0x25, 0x8A, 0x9E, 0x9F, 0x9F, 0x27, 0x22, 0x4A, 0xCC, 
	0x1C, 0xE6, 0x94, 0x31, 0x73, 0x08, 0x21, 0xF0, 0xE9, 0x7E, 
	0x66, 0x0E, 0xAF, 0xAF, 0xAF, 0x3B, 0x66, 0x0E, 0x5A, 0x6B, 
	0x73, 0x44, 0xDF, 0x1C, 0x6D, 0x1C, 0x47, 0x75, 0x4F, 0xF4, 
	0x73, 0xBA, 0xCA, 0x9A, 0x88, 0xA6, 0x71, 0x1C, 0xD5, 0x0F, 
	0x8A, 0xBC, 0xF7, 0x4C, 0x44, 0xD3, 0x3D, 0x8E, 0xE7, 0xFB, 
	0xC7, 0x71, 0x54, 0x6D, 0xDB, 0x6E, 0x8B, 0xDD, 0x7B, 0xCF, 
	0x47, 0x00, 0x9B, 0xCD, 0xA6, 0xFB, 0x4D, 0xE4, 0xE5, 0x1E, 
	0xBA, 0xAE, 0xDB, 0x00, 0xC8, 0x4D, 0xD3, 0xBC, 0x1F, 0x01, 
	0x3C, 0x42, 0x8D, 0x88, 0x0C, 0x65, 0x3D, 0xAF, 0x28, 0x66, 
	0xF6, 0x44, 0x34, 0x1D, 0x00, 0xBC, 0xF7, 0x7C, 0xAF, 0xF3, 
	0x53, 0x80, 0xB6, 0x6D, 0xB7, 0x05, 0xA0, 0x64, 0xE6, 0xBD, 
	0xE7, 0x55, 0xA9, 0x10, 0x3C, 0x20, 0x29, 0x25, 0x12, 0x11, 
	0x7B, 0x7A, 0xFE, 0xE5, 0xE5, 0xE5, 0xAF, 0x31, 0x46, 0x87, 
	0x10, 0xBE, 0x01, 0x96, 0x4A, 0xEF, 0x16, 0x71, 0xCE, 0x29, 
	0x11, 0xB1, 0x00, 0x10, 0x63, 0xAC, 0x96, 0xF6, 0xAC, 0x00, 
	0x60, 0xBF, 0xDF, 0xD7, 0x00, 0xD0, 0x34, 0x0D, 0xEA, 0xBA, 
	0x46, 0x8C, 0x11, 0x00, 0x50, 0x55, 0x15, 0x62, 0x8C, 0xD8, 
	0x6E, 0xB7, 0x57, 0xC1, 0xCE, 0x05, 0xB9, 0x2A, 0xA9, 0x02, 
	0x40, 0x5D, 0xD7, 0xD8, 0xED, 0x76, 0x73, 0x0A, 0x20, 0x22, 
	0x50, 0x4A, 0xC1, 0x39, 0xF7, 0x48, 0x92, 0xDF, 0x00, 0xFF, 
	0x53, 0x45, 0x8C, 0x11, 0x21, 0x04, 0x84, 0x10, 0x4A, 0x63, 
	0x83, 0x31, 0x06, 0x5A, 0xEB, 0xBB, 0x00, 0xBE, 0xBE, 0xBE, 
	0xFE, 0x94, 0xC6, 0xB8, 0x2A, 0x46, 0xA5, 0xD4, 0x91, 0xF3, 
	0x59, 0xEA, 0x20, 0xA2, 0xBB, 0xEF, 0xE6, 0x90, 0x41, 0xA9, 
	0x00, 0x22, 0x7A, 0x98, 0x86, 0xAB, 0x14, 0x5D, 0x12, 0x66, 
	0x46, 0x4A, 0xCB, 0xAD, 0x5F, 0x44, 0xEC, 0x3C, 0xDA, 0xA5, 
	0xEA, 0x5A, 0x9D, 0xD2, 0x21, 0x72, 0xFC, 0x4B, 0x5C, 0xBB, 
	0xE0, 0x52, 0x20, 0x44, 0x94, 0x3E, 0x3E, 0x3E, 0x9A, 0xFD, 
	0x7E, 0x5F, 0xCF, 0x87, 0xD4, 0x01, 0xC0, 0x18, 0x83, 0xB6, 
	0x6D, 0x11, 0x63, 0x84, 0xB5, 0xF6, 0xE0, 0x60, 0xBD, 0x5E, 
	0xE3, 0xED, 0xED, 0x6D, 0xD1, 0x39, 0x11, 0x25, 0x11, 0xB1, 
	0x29, 0x25, 0xB2, 0xD6, 0x8A, 0x31, 0x46, 0x17, 0xFB, 0x61, 
	0xD3, 0x30, 0x0C, 0x72, 0x32, 0x40, 0xB2, 0x88, 0xE4, 0xA6, 
	0x69, 0xB2, 0x88, 0x64, 0x22, 0xBA, 0x38, 0x6C, 0xCE, 0x7D, 
	0xEB, 0xBA, 0x6E, 0x93, 0x73, 0x06, 0xA6, 0x69, 0xA2, 0x7B, 
	0xDB, 0xF4, 0x2D, 0x00, 0x47, 0xED, 0xBA, 0xEF, 0x7B, 0xFD, 
	0x9B, 0x59, 0x70, 0x6A, 0x9F, 0x8F, 0xD0, 0xC3, 0xC8, 0x4C, 
	0x29, 0x91, 0x31, 0x46, 0x7F, 0x7E, 0x7E, 0xAE, 0x9D, 0x73, 
	0xEA, 0x52, 0x7F, 0x22, 0xA2, 0x54, 0x78, 0x66, 0xE6, 0xC0, 
	0xCC, 0xA1, 0xAA, 0xAA, 0xB8, 0x34, 0x32, 0x9F, 0xCE, 0x3D, 
	0x5B, 0xCE, 0x3D, 0x5D, 0x44, 0xC4, 0x96, 0x12, 0xBC, 0x45, 
	0xFE, 0x01, 0x44, 0x06, 0x53, 0x0E, 0x82, 0x24, 0x81, 0xBD, 
	0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 
	0x60, 0x82, 
};

wxBitmap& inventory_png_to_wx_bitmap()
{
	static wxMemoryInputStream memIStream( inventory_png, sizeof( inventory_png ) );
	static wxImage image( memIStream, wxBITMAP_TYPE_PNG );
	static wxBitmap bmp( image );
	return bmp;
};


#endif //INVENTORY_PNG_H
