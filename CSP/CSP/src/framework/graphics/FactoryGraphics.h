#ifndef FGTag

  #define FGTag

  // The Factory for Graphics.

  #include "graphics/StandardGraphics.h"
  #include "standard/StandardFramework.h"
  #include "standard/FrameworkError.h"

  class FactoryGraphics: private StandardFramework
  {

  private:

    short count;

    _GRAPHICS_DESCRIPTION *gd;

  public:

    FactoryGraphics();
    ~FactoryGraphics();

    // Standard Framework - required for every framework object
    void  SetSetup(void *setup);
    short GetDependencyCount();
    void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
    short SetDependencyData(short Index, void* Data);
    void  GetSetup(void *setup);
    short CheckStatus();

    // ----

    short GetCount();
    short GetDescription(short Index, _GRAPHICS_DESCRIPTION **Description);
    short Create(short ID, StandardGraphics **Graphics);
  };

#endif
