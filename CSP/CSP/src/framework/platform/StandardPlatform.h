// The pure abstract class for Platform.

#ifndef SPTag

  #define SPTag

  #include "TypesPlatform.h"

  class StandardPlatform: public StandardFramework
  {

  private:

  public:

    virtual short         CreateAppWindow() = 0;
    virtual short         CreateRenderingDevice(StandardGraphics **display) = 0;

    virtual char*         GetPlatformVersion() = 0;
    virtual void          TriggerShutdown() = 0;
    virtual void          HandleSystemMessages() = 0;

    virtual short         GetRegainedFocus() = 0;
    virtual short         GetQuit() = 0;
    virtual unsigned long GetTime() = 0;
    virtual void*         GetWindowHandle() = 0;
    virtual void*         GetShutdown() = 0;

    virtual void          Nap(short Milliseconds) = 0;
    virtual void          Flip() = 0;

    virtual void          DeleteRenderingDevice(StandardGraphics *display) = 0;
    virtual void          DeleteAppWindow() = 0;
  };

#endif
