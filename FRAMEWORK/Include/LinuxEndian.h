#include "Framework.h"

class LinuxEndian : public StandardEndian
{

private:

public:

  LinuxEndian();
  ~LinuxEndian();

  // functions
  short BigTo2(StandardBuffer *Buffer, unsigned long Start, unsigned long Length);
  short LittleTo2(StandardBuffer *Buffer, unsigned long Start, unsigned long Length);
  short BigTo4(StandardBuffer *Buffer, unsigned long Start, unsigned long Length);
  short LittleTo4(StandardBuffer *Buffer, unsigned long Start, unsigned long Length);

};
