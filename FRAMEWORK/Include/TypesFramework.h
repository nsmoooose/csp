#ifndef TYPESFRAMEWORKTAG

  #define TYPESFRAMEWORKTAG

  #include "Framework.h"

  class StandardFramework
  {

    private:

    public:

      virtual ~StandardFramework() {};

      // functions
      virtual short               Initialize() = 0;
      virtual void                Uninitialize() = 0;

      virtual void                Nap(unsigned long Milliseconds) = 0;
      virtual void                TriggerShutdown() = 0;

      virtual unsigned long       GetTime() = 0;
      virtual _HANDLE             GetShutdownHandle() = 0;
      virtual bool                GetShutdown() = 0;

  };

#endif
