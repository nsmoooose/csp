#include "Framework.h"

class LinuxWindowText : public StandardWindowText
{

private:

  // functions
  bool PreprocessInput(char*InputLine);

  // members
  StandardApp           *p_App;
  _APP_INIT_STRUCT      *p_AppInit;
  StandardFramework     *p_Framework;

  //  HANDLE                 p_hWriteHandle;
  //  HANDLE                 p_hReadHandle;
  bool                   p_bRun;

public:

  LinuxWindowText();
  ~LinuxWindowText();

  // functions
  short Initialize(StandardApp *App, StandardFramework *Framework);
  void  Uninitialize();
  void  PrintText(char *text);  
  void  Run();

};
