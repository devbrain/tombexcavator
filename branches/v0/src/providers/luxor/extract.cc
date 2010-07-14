#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


struct mjz_header_s
{
  char magic[8];
  unsigned long records;
  unsigned long unknown;
};


struct record_s
{
  char name[128];
  unsigned int is_dir;
  unsigned int offset;
  unsigned int size1;
  unsigned int size2;
};
// ----------------------------------------------------------------------
FILE * create_file(const char * name)
{
  int n = strlen(name);
  char *file_name;
  int sz =0;
  for (int i=n; i>=0 && name[i] != '/' ; i--,sz++);
  sz--;
  file_name = new char[sz+1];
  for (int i=0; i<sz; i++)
    file_name[i] = name[n-sz+i];
  file_name[sz] = 0;
  int start = 0;
  int end = 0;
  while (end<n-sz)
    {
      for (end = start;name[end] != '/' && end<(n-sz); end++);
      char * path = new char [end+1];
      for (int k=0; k<end; k++)
        path[k] = name[k];
      path[end] = 0;
      if (end != start) {
        if (mkdir(path,0777) == 0)
          {
            std::cout << path << std::endl;
          }
      }
      start = end;
      start ++;
    };
  FILE *f = fopen(name,"wb");
  if (!f)
    {
      std::cout << "GEVALT!!! " << name << std::endl;
      exit(1);
    }
  return f;
}
// ----------------------------------------------------------------------
void extract_file(const record_s & r,FILE * df)
{
  FILE *f = create_file(r.name);
  long pos = ftell(df);
  fseek(df,r.offset,SEEK_SET);
  char * data = new char[r.size1];
  fread(data,r.size1,1,df);
  fwrite(data,r.size1,1,f);
  fclose(f);
  fseek(df,pos,SEEK_SET);
}
// ----------------------------------------------------------------------
int main(int argc,char * argv[])
{
  FILE * f = fopen("data.mjz","rb");
  mjz_header_s header;
  fread(&header,sizeof(header),1,f);
  std::cout << header.magic << std::endl
            << header.records << std::endl;
  record_s * records = new record_s[header.records];
  for (unsigned int i=0; i<header.records; i++)
    {
      fread(&records[i],sizeof(record_s),1,f);
      std::cout << records[i].name << " " << records[i].offset
                << " " << records[i].size1 << std::endl;
      if (!records[i].is_dir)
        {
          extract_file(records[i],f);
        }
    }
  delete[] records;
  fclose(f);
  return 0;
}
