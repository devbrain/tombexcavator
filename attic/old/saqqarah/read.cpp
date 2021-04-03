#include <stdio.h>
#include <string.h>
#include <iostream>

void do_read (FILE* f, char* d, size_t sz)
{
  unsigned char* buff = (unsigned char*) d;
  fread (buff, 1, sz, f);
  for (size_t i=0; i<sz; i++)
    {
      unsigned char xh = buff[i];
      buff [i] = 0xFF - xh;
    }
}

template <typename T>
T read_enc (FILE* f)
{
  char d [sizeof (T)];
  do_read (f, d, sizeof(T));
  T r;
  memcpy (&r, d, sizeof (T));
  return r;
}

int main (int argc, char* argv [])
{
  const char* fname = argv [1];
  FILE* f = fopen (fname, "rb");
  if (!f)
    {
      return 0;
    }

  int items = read_enc<int> (f);
  std::cout << "items: " << items << std::endl;
  for (int i=0; i<items; i++)
    {
      short sz = read_enc<short> (f);
      char* data = new char [sz];
      do_read (f, data, sz);
      std::string name (data, sz);
      delete [] data;
      std::cout << name << " ";
      int offset = read_enc<int> (f);
      int size = read_enc<int> (f);
      std::cout << offset << " , "  << size << std::endl;

      long now_pos = ftell (f);
      
      fseek (f, offset, SEEK_SET);
      data = new char [size];
      do_read (f, data, size);
      
      std::string new_name = std::string ("A_") + name;
      FILE* fo = fopen (new_name.c_str (), "wb");
      fwrite (data, 1, size, fo);
      fclose (fo);
      delete [] data;
      fseek (f, now_pos, SEEK_SET);

    }
}
