#include "Framework.h"

class WinFile : public StandardFile
{

private:

public:

  WinFile();
  ~WinFile();

  // functions
  short LoadFile(char *Filename, StandardBuffer *Buffer);
  short SaveFile(char *Filename, StandardBuffer *Buffer);

};