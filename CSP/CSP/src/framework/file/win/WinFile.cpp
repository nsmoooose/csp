#include "WinFile.h"

WinFile::WinFile()
{

}

WinFile::~WinFile()
{

}

short WinFile::LoadFile(char *Filename, StandardBuffer *Buffer)
{

  void *data;
  FILE *file;
  unsigned long size;
  fpos_t pos;
   
  file = fopen(Filename, "rb");

  fseek(file, 0, SEEK_END);

  fgetpos(file, &pos);

  size = (unsigned long)pos;

  fseek(file, 0, SEEK_SET);

  Buffer->CreateBuffer(size);

  Buffer->Lock();
  Buffer->GetDataPointer(&data);

  fread(data, size,1,file);

  fclose(file);

  Buffer->Unlock();

  return 0;
}

short WinFile::SaveFile(char *Filename, StandardBuffer *Buffer)
{

  void *data;
  FILE *file;
  unsigned long size;

  Buffer->GetSize(&size);
  Buffer->Lock();
  Buffer->GetDataPointer(&data);

  file = fopen(Filename, "wb");

  fwrite(data, size,1,file);

  fclose(file);

  Buffer->Unlock();

  return 0;
}