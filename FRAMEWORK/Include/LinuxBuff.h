#ifndef __LINUXBUFF_H_
#define __LINUXBUFF_H_

#include "Framework.h"

class LinuxBuff : public StandardBuffer
{

private:

public:
  LinuxBuff();
  ~LinuxBuff();

  // functions
  short         Initialize(unsigned long Length);
  void*         GetDataPointer();
  unsigned long GetSize();
  void          Lock();
  void          Unlock();
  void          Uninitialize();


};

#endif
