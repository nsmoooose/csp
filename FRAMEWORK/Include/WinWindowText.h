#include "Framework.h"


class WinWindowText : public StandardWindowText
{

private:

  // functions
  bool PreprocessInput(char*InputLine);

  // members
  StandardApp           *p_App;
  _APP_INIT_STRUCT      *p_AppInit;
  StandardFramework     *p_Framework;

  HANDLE                 p_hWriteHandle;
  HANDLE                 p_hReadHandle;
  bool                   p_bRun;

public:

  WinWindowText();
  ~WinWindowText();

  // functions
  /** This will create and initialize the console window
  @param App This is your instance of the (StandardApp) object.
  @param Framework This is your instance of the (StandardFramework) object.
  */
  short Initialize(StandardApp *App, StandardFramework *Framework);

  /** This will destroy and uninitialize your console window
  */
  void  Uninitialize();

  /** This will print a line of text to the console window
  @param text This is a string of data (null-terminated) that will be printed
              to the console window.
  */
  void  PrintText(char *text);
  void  ReadText(char * text);


  /** This will run the console window. This function will only exit
      when the "run" command is given or the "exit" command is given.
      Any additional commands are executed in the (StandardApp) object.
      A list of additional commands are found in (StandardApp)->RequestCommands().
  */
  void  Run();

};