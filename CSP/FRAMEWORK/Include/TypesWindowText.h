#ifndef TYPESWINDOWTEXTTAG
#define TYPESWINDOWTEXTTAG

  #include "Framework.h"

  class StandardWindowText
  {

    private:

    public:
  
      virtual ~StandardWindowText() {};

      // functions
      virtual short Initialize(StandardApp *App, StandardFramework *Framework) = 0;
      virtual void  Uninitialize() = 0;

      virtual void  PrintText(char *text) = 0;    
      virtual void  ReadText(char * text) = 0;

      virtual void  Run() = 0;

  };

#endif
