// A Generic Framework error handler.

#ifndef FETag

  #define FETag

  #include "TypesError.h"

  class FrameworkError
  {

    private:

    public:

      FrameworkError();
      ~FrameworkError();

      short   sError;
      long    lError;
      char    String[512]; // UGH. You know the limit, don't exceed it.

      char*   ConvertErrorToString(short sError);
      void    WriteToLog(char *Function, char *Line);
  };

#endif
