#include "StandardFile.h"
#include <stdio.h>

class WinFile : public StandardFile
{

private:

public:

  WinFile();
  ~WinFile();

  short LoadFile(char *Filename, StandardBuffer *Buffer);
  short SaveFile(char *Filename, StandardBuffer *Buffer);

};