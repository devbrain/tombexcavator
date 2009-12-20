#include <stdio.h>
#include <string.h>
#include <iostream>


const unsigned char one_time_key [28] = 
  {
    67, 111, 112, 121, 114, 105, 103, 104, 116,  32,  49, 57,  57, 49,
    32,  80, 101, 100, 101, 114,  32,  74, 117, 110, 103, 99, 107,  0
  };

const unsigned char pal [64] = 
  {
    0,  0,   0, 0, 168,  0,   0, 0,  0, 168,   0, 0, 168, 168,   0, 0,
    0,  0, 168, 0, 168,  0, 168, 0,  0,  84, 168, 0, 168, 168, 168, 0,
    84, 84,  84, 0, 252, 84,  84, 0, 84, 252,  84, 0, 252, 252,  84, 0,
    84, 84, 252, 0, 252, 84, 252, 0, 84, 252, 252, 0, 252, 252, 252, 0
  };
// ---------------------------------------------------------------------------
void decrypt (unsigned char* data, size_t size)
{
  for (size_t m = 0; m<size; m++)
    {
      unsigned char byte = data [m];
      unsigned char j = 0;
      for (unsigned char i=0; i<8; i++)
	{
	  const unsigned char mask = 1 << i;
	  if (byte & mask)
	    {
	      j = j | (1 << (i ^ 7));
	    }
	}
      data [m] = j ^ one_time_key [m % 28];
    }
}
// ---------------------------------------------------------------------------
bool from_planar_to_flat (unsigned char* output, const unsigned char* input,
			 unsigned int width, unsigned int height, size_t offset, 
			 size_t input_size)
{
  const unsigned int bytes = width / 8;
  const unsigned char* tiledata = input + offset;
  unsigned int j = 0;
  for (unsigned int ty=0; ty<height; ty++)
    {
      unsigned int start_x = 0;
      std::cout << ty*width << " ::: ";
      for (unsigned int x=0; x<bytes; x++)
	{
	  
	  unsigned char nn = tiledata [j+0];
	  unsigned char c0 = tiledata [j+1];
	  unsigned char c1 = tiledata [j+2];
	  unsigned char c2 = tiledata [j+3];
	  unsigned char	c3 = tiledata [j+4];

	  for (unsigned int tx=0; tx<8; tx++)
	    {
	      unsigned char bit   = 7 - (tx & 7);
	      unsigned char mask  = 1 << bit;
	      unsigned char color = 0;
	      if (c3 & mask)
		{
		  color |= 1 << 3;
		}
	      if (c2 & mask)
		{
		  color |= 1 << 2;
		}
	      if (c1 & mask)
		{
		  color |= 1 << 1;
		}
	      if (c0 & mask)
		{
		  color |= 1;
		}
	      
	      output [ty*width + start_x + tx] = color;
	      std::cout << (color & 0xFF) << " "; 
	    }
	  start_x += 8;
	  j += 5;
	}
      std::cout << std::endl;
    }
  return true;
}
// ---------------------------------------------------------------------------
unsigned char* convert_from_ega (unsigned char* data, size_t size)
{
  const unsigned int sprites = data [0] & 0xFF;
  const unsigned int w       = data [1] & 0xFF;
  const unsigned int height  = data [2] & 0xFF;
  const unsigned int width   = 8*w;
  const size_t bitmap_size = height*width;
  unsigned char* bitmap    = new unsigned char [bitmap_size];
  
  memset (bitmap, 0, bitmap_size);
  from_planar_to_flat (bitmap, data, width, height, 3, size);
  return bitmap;
}
// ---------------------------------------------------------------------------

struct bmp_header
{
  bmp_header (int w, int h)
  {
    id [0] = 'B';
    id [1] = 'M';
    reserved = 0;
    offset = 118;
    sizeheader = 40;
    width = w;
    height = h;
    planes = 1;
    bpp = 8;
    compression = 0;
    sizeimage = width * height;
    size = sizeimage + offset;
    xres = 0;
    yres = 0;
    colorused = 16;
    colorimportant = 16;
  }
  char id [2];
  int  size;
  int  reserved;
  int  offset;
  int  sizeheader;
  int  width;
  int  height;
  short planes;
  short bpp;
  int   compression;
  int   sizeimage;
  int   xres;
  int   yres;
  int   colorused;
  int   colorimportant;
};

#define WR(V) fwrite (&V, 1, sizeof (V), f)

void write_bmp (const char* fname, const unsigned char* data, int w, int h)
{
  FILE* f = fopen (fname, "wb");
  bmp_header bh (w, h);
  fwrite (bh.id, 1, sizeof (bh.id), f);
  WR (bh.size);
  WR (bh.reserved);
  WR (bh.offset);
  WR (bh.sizeheader);
  WR (bh.width);
  WR (bh.height);
  WR (bh.planes);
  WR (bh.bpp);
  WR (bh.compression);
  WR (bh.sizeimage);
  WR (bh.xres);
  WR (bh.yres);
  WR (bh.colorused);
  WR (bh.colorimportant);
  fwrite (pal, 1, 64, f);
  fwrite (data, 1, bh.sizeimage, f);
  fclose (f);
  delete [] data;
  
}
// ---------------------------------------------------------------------------
int main (int argc, char* argv [])
{
  const char* fname = "SAM101.GFX";
  FILE* f = fopen (fname, "rb");
  if (!f)
    {
      std::cerr << "Failed to open " << fname << std::endl;
      return 1;
    }
  const size_t tile_size = 8064;
  unsigned char data [tile_size];
  fread (data, 1, tile_size, f);
  fclose (f);
  decrypt (data, tile_size);
  unsigned char* d = convert_from_ega (data, tile_size);
  write_bmp ("1.bmp", d, 16, 16);
  return 0;
}
