#include "StandardBuffer.h"

class StandardFile
{

private:

public:

  virtual short LoadFile(char *Filename, StandardBuffer *Buffer) = 0;
  virtual short SaveFile(char *FileName, StandardBuffer *Buffer) = 0;

};