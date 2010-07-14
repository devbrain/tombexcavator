#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

#include <sys/stat.h>
#include <sys/types.h>



struct fat_entry
{
  std::string  name;
  unsigned int offset;
  unsigned int size;
};

typedef std::vector <fat_entry> fat_t;


unsigned char key_string [12] = 
  { 
    0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1
  };

int main (int argc, char *argv [])
{
  if (argc != 2)
    {
      std::cerr << "usage: unril fname.ril" << std::endl;
      return 1;
    }

  FILE* f = fopen (argv [1], "rb");

  if (!f)
    {
      std::cerr << "Failed to open " << argv [1] << std::endl;
      return 0;
    }

  char buff [16];
  memset (buff, 0, 16);


  fread (buff, 16, 1, f);

  unsigned short entries;
  memcpy (&entries, buff + 14, 2);

  std::cout << entries << std::endl;
  
  unsigned int entries_area_size = 21*entries;

  char* entries_area = new char [entries_area_size];
  fread (entries_area, entries_area_size, 1, f);

  fat_t fat;

  for (int i = 0; i<entries; ++i)
    {
      char k =0;
      while (k<12)
	{
	  char* ptr = (k + (21 * i)) + entries_area;
	  char z = key_string [k++];
	  *ptr ^= z;
	}
      char* curr_entry = entries_area + (21*i);

      fat_entry fe;
      fe.name = std::string (curr_entry, curr_entry + 13);
      memcpy (&fe.offset, curr_entry + 13  , 4);
      memcpy (&fe.size  , curr_entry + 13+4, 4);
      fat.push_back (fe);
    }

  delete [] entries_area;

  std::string odir = std::string (argv [1]) + std::string ("_extracted");
  mkdir (odir.c_str (), 0777);
  for (fat_t::iterator i = fat.begin (); i != fat.end (); i++)
    {
      fat_entry& fe = *i;
      std::string oname = odir + std::string ("/") + fe.name;
      fseek (f, fe.offset, SEEK_SET);
      char* data = new char [fe.size];
      fread (data, fe.size, 1, f);
      FILE* fo = fopen (oname.c_str (), "wb");
      fwrite (data, fe.size, 1, fo);
      fclose (fo);
      delete [] data;
    }
  fclose (f);
  return 0;
}
