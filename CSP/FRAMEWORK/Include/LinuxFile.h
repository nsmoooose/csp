#include "Framework.h"

class LinuxFile : public StandardFile
{

private:

public:

  LinuxFile();
  ~LinuxFile();

  // functions
  short LoadFile(char *Filename, StandardBuffer *Buffer);
  short SaveFile(char *Filename, StandardBuffer *Buffer);

};
