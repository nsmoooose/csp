#include "WinBuff.h"

WinBuff::WinBuff()
{
	handle = 0;
  data = 0;
}

WinBuff::~WinBuff()
{
	DestroyBuffer();
}

short WinBuff::CreateBuffer(unsigned long Length)
{
  if(handle == 0)
  {
    handle = GlobalAlloc(GHND, Length);
  }
	return 0;
}

short WinBuff::GetDataPointer(void** Location)
{	
  if(handle != 0 && data != 0)
  {
    *Location = data;
  }
	return 0;	
}

short WinBuff::GetSize(unsigned long *size)
{	
  if(handle != 0)
  {
    *size = GlobalSize(handle);
  }
  else
  {
    *size = 0;
  }
	return 0;	
}

short WinBuff::Lock()
{	
  if(data == 0)
  {
    data = GlobalLock(handle);
  }
	return 0;	
}

short WinBuff::Unlock()
{	
  if(data != 0)
  {
    GlobalUnlock(handle);
    data = 0;
  }
	return 0;	
}

short WinBuff::DestroyBuffer()
{	
	if(handle != 0)
	{
    Unlock();
		GlobalFree(handle);
    handle = 0;
	}
	return 0;	
}
