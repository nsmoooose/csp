#include "Framework.h"

class WinFramework : public StandardFramework
{

  private:
    HANDLE              p_hShutdown;
    bool                p_bShutdown;

  public:

    WinFramework();
    ~WinFramework();

    // functions
    short               Initialize();
    void                Uninitialize();

    void                Nap(unsigned long Milliseconds);
    void                TriggerShutdown();

    unsigned long       GetTime();
    _HANDLE             GetShutdownHandle();
    bool                GetShutdown();

};