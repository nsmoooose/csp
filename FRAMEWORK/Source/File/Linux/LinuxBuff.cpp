#include "LinuxBuff.h"

LinuxBuff::LinuxBuff()
{
}

LinuxBuff::~LinuxBuff()
{
  Uninitialize();
}

short LinuxBuff::Initialize(unsigned long Length)
{
  return 0;
}

void* LinuxBuff::GetDataPointer()
{	
  return 0;	
}

unsigned long LinuxBuff::GetSize()
{	
  return 0;	
}

void LinuxBuff::Lock()
{	
  return;	
}

void LinuxBuff::Unlock()
{	
  return;	
}

void LinuxBuff::Uninitialize()
{	
  return;	
}
