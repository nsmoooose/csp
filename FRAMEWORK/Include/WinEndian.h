#include "Framework.h"

class WinEndian : public StandardEndian
{

private:

public:

  WinEndian();
  ~WinEndian();

  // functions
  short BigTo2(StandardBuffer *Buffer, unsigned long Start, unsigned long Length);
  short LittleTo2(StandardBuffer *Buffer, unsigned long Start, unsigned long Length);
  short BigTo4(StandardBuffer *Buffer, unsigned long Start, unsigned long Length);
  short LittleTo4(StandardBuffer *Buffer, unsigned long Start, unsigned long Length);

};