#include <stdio.h>
#include "abl/types/types.hpp"

struct cps_header_s
{
  uint16_t m_file_size;
  uint16_t m_compression_type;
  uint32_t m_uncompressed_size;
  uint16_t m_palette_size;
};

cps_header_s load_header (FILE* f)
{
  cps_header_s hdr;
  fread (&hdr.m_file_size, sizeof (hdr.m_file_size), 1, f);
  fread (&hdr.m_compression_type, sizeof (hdr.m_compression_type), 1, f);
  fread (&hdr.m_uncompressed_size, sizeof (hdr.m_uncompressed_size), 1, f);
  fread (&hdr.m_palette_size, sizeof (hdr.m_palette_size), 1, f);

  return hdr;
}

void print_hdr (const cps_header_s& hdr)
{
  printf ("file size: %d\n", hdr.m_file_size);
  printf ("compression type: %d\n", hdr.m_compression_type);
  printf ("uncompressed size: %d\n", hdr.m_uncompressed_size);
  printf ("palette size: %d\n", hdr.m_palette_size);
}

int main (int argc, char* argv [])
{
  if (argc != 2)
    {
      printf ("USAGE: %s <file.cps>\n", argv [0]);
      return 1;
    }
  
  FILE* f = fopen (argv [1], "rb");
  cps_header_s hdr = load_header (f);
  print_hdr (hdr);
  return 0;
  
}

