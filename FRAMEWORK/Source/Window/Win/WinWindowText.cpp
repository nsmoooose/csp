#include "WinWindowText.h"

WinWindowText::WinWindowText()
{
  CSP_LOG(CSP_WINDOW, CSP_TRACE, "WinWindowText::WinWindowText()");

  p_bRun = false;
}

WinWindowText::~WinWindowText()
{
  CSP_LOG(CSP_WINDOW, CSP_TRACE, "WinWindowText::~WinWindowText()");

  Uninitialize();
}


short WinWindowText::Initialize(StandardApp *App, StandardFramework *Framework)
{
  CSP_LOG(CSP_WINDOW, CSP_TRACE, "WinWindowText::Initialize()");

  p_App = App;
  p_AppInit = p_App->GetAppInit();
  p_Framework = Framework;

  //  ---
  AllocConsole();

  SetConsoleTitle(p_AppInit->LongName);

  p_hWriteHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  p_hReadHandle = GetStdHandle(STD_INPUT_HANDLE);

  COORD coord;
  DWORD written;

  coord.X = 0;
  coord.Y = 0;

  FillConsoleOutputAttribute(p_hWriteHandle, BACKGROUND_BLUE | FOREGROUND_INTENSITY, 
                            80 * 25,
                            coord,
                            &written);

  FillConsoleOutputAttribute(p_hWriteHandle, FOREGROUND_INTENSITY, 
                            80 * 2,
                            coord,
                            &written);


  SetConsoleTextAttribute(p_hWriteHandle,   FOREGROUND_RED |
      FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

  PrintText(p_AppInit->LongName);

  SetConsoleTextAttribute(p_hWriteHandle, 
      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  if(stricmp(p_AppInit->CommandLine, "command") == 0)
  {

    PrintText("\nFramework " FRAMEWORK_VERSION " Command Window.\n");

	  Run();

    if(p_bRun == false)
    {
      return _EARLY_EXIT;
    }
  }
  else
  {
    PrintText("\nStatus Window\n");
  }

  SetConsoleTextAttribute(p_hWriteHandle,  BACKGROUND_BLUE | FOREGROUND_RED |
      FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

  return 0;
}

void WinWindowText::Uninitialize()
{
  CSP_LOG(CSP_WINDOW, CSP_TRACE, "WinWindowText::Uninitialize()");

  FreeConsole();
 
  CloseHandle(p_hWriteHandle);
  CloseHandle(p_hReadHandle);

  return;
}

void WinWindowText::PrintText(char *text)
{
  CSP_LOG(CSP_WINDOW, CSP_BULK, 
	  "WinWindowText::PrintText() - " << text );

  DWORD written;

  WriteConsole(p_hWriteHandle, text, strlen(text), &written, NULL);

  return;
}

void WinWindowText::ReadText(char * text)
{
  char buffer[256];
  DWORD charsRead;

  CSP_LOG(CSP_WINDOW, CSP_BULK, 
	  "WinWindowText::ReadText() - " << text );


  ReadConsole(p_hReadHandle, buffer, 256, &charsRead, NULL);

}

void WinWindowText::Run()
{

  CSP_LOG(CSP_WINDOW, CSP_TRACE, "WinWindowText::Run()");


  DWORD read;
  char data[80];
  COORD coord;

  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(p_hWriteHandle, &info);

  SetConsoleTextAttribute(p_hWriteHandle, 
      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  coord.X = 54;
  coord.Y = 1;

  SetConsoleCursorPosition(p_hWriteHandle, coord);

  PrintText("Type '?' for command list.");

  coord.X = 54;
  coord.Y = 0;
  SetConsoleCursorPosition(p_hWriteHandle, coord);

  SetConsoleTextAttribute(p_hWriteHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
  PrintText("Command Mode");

  coord.X = 0;
  coord.Y = info.dwCursorPosition.Y;
  SetConsoleCursorPosition(p_hWriteHandle, coord);

  SetConsoleTextAttribute(p_hWriteHandle,  BACKGROUND_BLUE | FOREGROUND_RED |
      FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

  FlushConsoleInputBuffer(p_hReadHandle);

  while(1)
  {

    if (p_Framework->GetShutdown() == true || p_bRun == true)
    {
      break;
    }

    memset(data, 0, sizeof(data));

    PrintText(">");
    ReadFile(p_hReadHandle, data, sizeof(data) - 1, &read, NULL);

    data[strlen(data) - 2] = 0;

    if(PreprocessInput(data) == false)
    {
      p_App->OnConsoleText((char *)&data);
    }
  }

  return;
}

bool WinWindowText::PreprocessInput(char*InputLine)
{

  CSP_LOG(CSP_WINDOW, CSP_TRACE, 
	  "WinWindowText::PreprocessInput() - " << InputLine );


  bool result = false;

  if (stricmp(InputLine, "exit") == 0)
  {
    PrintText("Shutting down...\n");
    p_Framework->TriggerShutdown();
    result = true;
  }

  if (stricmp(InputLine, "?") == 0 ||
      stricmp(InputLine, "help") == 0)
  {
    PrintText("\nCommands Available\n");
    PrintText("-----------------------------------------------------\n");
    PrintText("exit             - This will exit the program.\n");
    PrintText("run              - This will start the program.\n");

    p_App->RequestCommands();

    PrintText("\n");
    result = true;
  }

  if (stricmp(InputLine, "run") == 0)
  {
    p_bRun = true;
    result = true;
  }

  return result;
}
