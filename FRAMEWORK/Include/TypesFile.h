#ifndef TYPESFILETAG

  #define TYPESFILETAG

  #include "Framework.h"

  class StandardFile
  {

    private:

    public:

      virtual ~StandardFile() {};

      // functions
      virtual short LoadFile(char *Filename, StandardBuffer *Buffer) = 0;
      virtual short SaveFile(char *FileName, StandardBuffer *Buffer) = 0;

  };

#endif
