// The pure abstract class for the Framework.

#ifndef SFTag

  #define SFTag

  #include "TypesFramework.h"

  class StandardFramework
  {

  private:

  public:
    virtual ~StandardFramework() {};

    virtual void  SetSetup(void *setup) = 0;
    virtual short GetDependencyCount() = 0;
    virtual void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency) = 0;
    virtual short SetDependencyData(short Index, void* Data) = 0;
    virtual void  GetSetup(void*setup) = 0;
    virtual short CheckStatus() = 0;
  };

#endif
