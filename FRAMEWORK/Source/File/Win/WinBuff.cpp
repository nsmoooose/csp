#include "WinBuff.h"

WinBuff::WinBuff()
{
	p_hHandle = 0;
  p_vpData = 0;
}

WinBuff::~WinBuff()
{
	Uninitialize();
}

short WinBuff::Initialize(unsigned long Length)
{
  if(p_hHandle == 0)
  {
    p_hHandle = GlobalAlloc(GHND, Length);
  }
	return 0;
}

void* WinBuff::GetDataPointer()
{	
  if(p_hHandle != 0 && p_vpData != 0)
  {
    return p_vpData;
  }
	return 0;	
}

unsigned long WinBuff::GetSize()
{	
  if(p_hHandle != 0)
  {
    return GlobalSize(p_hHandle);
  }

	return 0;	
}

void WinBuff::Lock()
{	
  if(p_vpData == 0)
  {
    p_vpData = GlobalLock(p_hHandle);
  }
	return;	
}

void WinBuff::Unlock()
{	
  if(p_vpData != 0)
  {
    GlobalUnlock(p_hHandle);
    p_vpData = 0;
  }
	return;	
}

void WinBuff::Uninitialize()
{	
	if(p_hHandle != 0)
	{
    Unlock();
		GlobalFree(p_hHandle);
    p_hHandle = 0;
	}
	return;	
}
