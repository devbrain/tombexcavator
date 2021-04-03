#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

struct entry_s
{
  char name [32];
  uint32_t unk;
  uint32_t size;

  uint32_t original;
  uint32_t offset;
};

size_t explode_chunk (const unsigned char* buff, size_t buff_size, unsigned char* offset)
{
  unsigned char table [256 + 254 + 1 + 1 + 32];
  unsigned char* tableA	   = table;
  unsigned char* tableB    = tableA + 256;
  unsigned char* table32_2 = tableB + 254;
  unsigned char* table32_1 = table32_2 + 1;
  unsigned char* table32   = table32_1 + 1;

  unsigned char v16;

  unsigned int next_p2 = 0;
  size_t total_decoded = 0;

label1:
  if ( total_decoded < buff_size )
  {
    int i = 0;
    do
    {
      tableB [i] = i;
      ++i;
    }
    while ( i < 256 );

    unsigned int v7 = next_p2;
    unsigned int p2 = next_p2 + 1;
    unsigned char code = *(unsigned char *)(v7 + buff);
    unsigned int p1 = 0;
    while ( 1 )
    {
      unsigned int tmp_code = code;
      if ( code > 127 )
      {
        code = 0;
        p1 += tmp_code - 127;
      }
      if ( p1 == 256 )
        break;
      unsigned int p3 = 0;
      while ( 1 )
      {
        unsigned int curr = p2 + 1;
        if ( p3 > code )
          break;
        unsigned char data = *(unsigned char *)(p2++ + buff);
        tableB[p1] = data;
        if ( p1 != data )
        {
          p2 = curr + 1;
          tableA[p1] = *(unsigned char *)(curr + buff);
        }
        ++p3;
        ++p1;
      }
      if ( p1 == 256 )
        break;
      unsigned int tmp_p2 = p2++;
      code = *(unsigned char *)(tmp_p2 + buff);
    }
    signed short v15 = *(signed short *)(p2 + buff);
    signed int v14 = 0;
    next_p2 = p2 + 2;
    while ( 1 )
    {
      if ( v14 )
      {
        --v14;
        v16 = table32[v14];
      }
      else
      {
        --v15;
        if ( v15 == -1 )
          goto label1;
        unsigned int v17 = next_p2++;
        v16 = *(unsigned char *)(v17 + buff);
      }
      if ( v16 == tableB[v16] )
      {
        size_t v18 = total_decoded++;
        *(unsigned char *)(offset + v18) = v16;
      }
      else
      {
        v14 += 2;
        unsigned char v19 = tableA[v16];
        unsigned char v20 = tableB[v16];
        *(table32_2 + v14) = v19;
        *(table32_1 + v14) = v20;
      }
    }
  }
   std::cout << "DECODED: " << total_decoded << std::endl;
  return total_decoded - buff_size; 
}

bool explode (FILE* f, const entry_s& e)
{
  std::cout << "Exploding:  " << e.name     << std::endl
	    << "Compressed: " << e.size     << std::endl
	    << "Original:   " << e.original << std::endl;
  fseek (f, e.offset, SEEK_SET);
  uint32_t has_bytes = 0;
  uint32_t out_ptr = 0;
  char* output = new char [e.original];
  std::string pth = "data/" + std::string (e.name);
  FILE* ff = fopen (pth.c_str (), "wb");
  size_t total_in = 0;
  while (has_bytes < e.original)
    {
      
      uint16_t sz;
      fread (&sz, 2, 1, f);
       std::cout << "CHUNK IN: " << sz << " "; 
      char* input = new char [sz];
      total_in += 2;
      total_in += sz;
      if (total_in > e.size)
	{
	  std::cout << "Overflow: " << total_in << " : " << has_bytes << std::endl;
	  break;
	}
      size_t chunk_size = 4096;
      if (e.original - has_bytes < 4096)
	{	
		chunk_size = e.original - has_bytes;
	}
      fread (input, sz, 1, f);
      size_t rc = explode_chunk ((unsigned char*)input, chunk_size, (unsigned char*)output + has_bytes);
      if (rc != 0)
	{
		std::cout << "ACHTUNG" << std::endl;
	}
      fwrite (output + has_bytes, chunk_size, 1, ff);
      has_bytes += chunk_size;
      delete [] input;
    }
  delete [] output;
  
  fclose (ff);
  return true;
}


int main (int argc, char* argv [])
{

  FILE* f = fopen ("STARGUN.DLT", "rb");
  char header [8];
  fread (header, 8, 1, f);

  uint32_t id = *(uint32_t*)header;
  uint32_t x = *(uint32_t*)(header + 2);
  uint16_t flag = *(uint16_t*) (header+6);
  uint16_t entries = 50;
  if (flag)
    {
      entries = (*(uint32_t*) (header+4)) >> 16;
    }

  // check header validity
  // id = 0x45564144
  // x >> 16 = 256
  std::cout << std::hex << "0x" << id << std::dec << std::endl;
  std::cout << (x >> 16) << std::endl;
  std::cout << "flag " << flag << std::endl;
  std::cout << "entries " << entries << std::endl;
  std::vector <entry_s> evector;
  for (int i=0; i<entries; i++)
    {
      entry_s e;
      fread (e.name, 32, 1, f);
      fread (&e.unk, 4, 1, f);
      fread (&e.size, 4, 1, f);
      
      for (int j = 1; j < 32; j++) 
	{
	  e.name [j] = ((e.name [j-1] + j) ^ e.name[j]); 
	}
      char data_val [6];
      fread (&data_val, 4, 1, f);
      flag = false;
      int offs = 4;
      uint32_t pos116;
      if (*(uint32_t*)((char*)data_val) == 0x50424750) // PGBP
	{
	  flag = true;
	  
	  fread (&pos116, 4, 1, f);
	  
	  offs += 4;
	}
      
      if (!flag)
	{
	  std::cout << "******** ";
	}
      
      uint32_t p = ftell (f);
      e.offset = p;
      e.original = pos116;
      fseek (f, e.size - offs, SEEK_CUR);
      evector.push_back (e);
    }
  for (int i=0; i<evector.size (); i++)
    {
      explode (f, evector [i]);
       break;
    }
  fclose (f);
  return 0;
}
