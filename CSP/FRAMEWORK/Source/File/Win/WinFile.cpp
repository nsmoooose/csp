#include "WinFile.h"

WinFile::WinFile()
{

}

WinFile::~WinFile()
{

}

short WinFile::LoadFile(char *Filename, StandardBuffer *Buffer)
{
  short result = _A_OK;

  void *data;
  FILE *file;
  unsigned long size;
  fpos_t pos;
   
  file = fopen(Filename, "rb");

  if(file == 0)
  {
    fseek(file, 0, SEEK_END);

    fgetpos(file, &pos);

    size = (unsigned long)pos;

    fseek(file, 0, SEEK_SET);

    Buffer->Initialize(size);

    Buffer->Lock();
    data = Buffer->GetDataPointer();

    fread(data, size,1,file);

    fclose(file);

    Buffer->Unlock();
  }
  else
  {
    result = _FILE_NOT_FOUND;
  }

  return result;
}

short WinFile::SaveFile(char *Filename, StandardBuffer *Buffer)
{

  void *data;
  FILE *file;
  unsigned long size;

  size = Buffer->GetSize();
  Buffer->Lock();
  data = Buffer->GetDataPointer();

  file = fopen(Filename, "wb");

  fwrite(data, size,1,file);

  fclose(file);

  Buffer->Unlock();

  return 0;
}