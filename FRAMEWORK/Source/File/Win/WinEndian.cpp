#include "WinEndian.h"

WinEndian::WinEndian()
{

}

WinEndian::~WinEndian()
{

}

short WinEndian::BigTo2(StandardBuffer *Buffer, unsigned long Start, unsigned long Length)
{

  char*data;
  unsigned long size;

  if (Buffer == 0)
  {
    return _NO_BUFFER; // return No buffer error.
  }

  size = Buffer->GetSize();

  if(Start > size)
  {
     return _START_OUT_OF_BOUNDS; // return Start Out of bounds error.
  }
  if((Start + Length) > size)
  {
    return _END_OUT_OF_BOUNDS; // return End Out of bounds error.
  }

  Buffer->Lock();
  data = (char *)Buffer->GetDataPointer();

  _swab(data + Start, data + Start, Length);

  Buffer->Unlock();

  return 0;
}

short WinEndian::LittleTo2(StandardBuffer *Buffer, unsigned long Start, unsigned long Length)
{
  // we are already little so ignore.
  return 0;
}

short WinEndian::BigTo4(StandardBuffer *Buffer, unsigned long Start, unsigned long Length)
{
  char*data;
  unsigned long size;

  if (Buffer == 0)
  {
    return _NO_BUFFER; // return No buffer error.
  }

  size = Buffer->GetSize();

  if(Start > size)
  {
     return _START_OUT_OF_BOUNDS; // return Start Out of bounds error.
  }
  if((Start + Length) > size)
  {
    return _END_OUT_OF_BOUNDS; // return End Out of bounds error.
  }

  Buffer->Lock();

  data = (char *)Buffer->GetDataPointer();

  _swab(data + Start, data + Start, Length);

  Buffer->Unlock();

  return 0;
}

short WinEndian::LittleTo4(StandardBuffer *Buffer, unsigned long Start, unsigned long Length)
{

  // we are already little so ignore.
  return 0;
}
