#ifndef TYPESENDIANTAG

  #define TYPESENDIANTAG

  #include "Framework.h"

  class StandardEndian
  {

    private:

    public:

      virtual ~StandardEndian() {};

      // functions
      virtual short BigTo2(StandardBuffer *Buffer, unsigned long Start, unsigned long Length) = 0;
      virtual short LittleTo2(StandardBuffer *Buffer, unsigned long Start, unsigned long Length) = 0;
      virtual short BigTo4(StandardBuffer *Buffer, unsigned long Start, unsigned long Length) = 0;
      virtual short LittleTo4(StandardBuffer *Buffer, unsigned long Start, unsigned long Length) = 0;

  };

#endif
