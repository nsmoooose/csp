#include "Framework.h"

class LinuxFramework : public StandardFramework
{

  private:
  //    HANDLE              p_hShutdown;
    bool                p_hShutdown;

  public:

    LinuxFramework();
    ~LinuxFramework();

    // functions
    short               Initialize();
    void                Uninitialize();

    void                Nap(unsigned long Milliseconds);
    void                TriggerShutdown();

    unsigned long       GetTime();
    _HANDLE             GetShutdownHandle();
    bool                GetShutdown();

};
