#include "SDLWindowText.h"
#include "stdlib.h"
#include "string.h"

SDLWindowText::SDLWindowText()
{

  p_bRun = false;
}

SDLWindowText::~SDLWindowText()
{
  Uninitialize();
}


short SDLWindowText::Initialize(StandardApp *App, StandardFramework *Framework)
{

  p_App = App;
  p_AppInit = p_App->GetAppInit();
  p_Framework = Framework;

  //  ---
  //  AllocConsole();

  //  SetConsoleTitle(p_AppInit->LongName);

  //  p_hWriteHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  //  p_hReadHandle = GetStdHandle(STD_INPUT_HANDLE);

  //  COORD coord;
  //  DWORD written;

  //  coord.X = 0;
  //  coord.Y = 0;

  //  FillConsoleOutputAttribute(p_hWriteHandle, BACKGROUND_BLUE | FOREGROUND_INTENSITY, 
  //                            80 * 25,
  //                            coord,
  //                            &written);

  //  FillConsoleOutputAttribute(p_hWriteHandle, FOREGROUND_INTENSITY, 
  //                            80 * 2,
  //                            coord,
  //                            &written);


  //  SetConsoleTextAttribute(p_hWriteHandle,   FOREGROUND_RED |
  //      FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

  PrintText(p_AppInit->LongName);

  //  SetConsoleTextAttribute(p_hWriteHandle, 
  //      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  if(strcmp(p_AppInit->CommandLine, "command") == 0)
  {

    PrintText("\nFramework " FRAMEWORK_VERSION " Command Window.\n");

    //    coord.X = 54;
    //    coord.Y = 1;

    //    SetConsoleCursorPosition(p_hWriteHandle, coord);

    PrintText("Type '?' for command list.");

    //    coord.X = 54;
    //    coord.Y = 0;
    //    SetConsoleCursorPosition(p_hWriteHandle, coord);

    //    SetConsoleTextAttribute(p_hWriteHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
    PrintText("Command Mode");

	  Run();
  }
  else
  {
    PrintText("\nStatus Window\n");
  }

  //  SetConsoleTextAttribute(p_hWriteHandle,  BACKGROUND_BLUE | FOREGROUND_RED |
  //      FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

  return 0;
}

void SDLWindowText::Uninitialize()
{

  //  FreeConsole();
  // 
  //  CloseHandle(p_hWriteHandle);
  //  CloseHandle(p_hReadHandle);

  return;
}

void SDLWindowText::PrintText(char *text)
{
  //  DWORD written;

  //  WriteConsole(p_hWriteHandle, text, strlen(text), &written, NULL);
  printf(text);

  //  return;
}

void SDLWindowText::Run()
{

  //  DWORD read;
    char data[80];
  //  COORD coord;

  //  coord.X = 0;
  //  coord.Y = 2;
  //  SetConsoleCursorPosition(p_hWriteHandle, coord);

  //  SetConsoleTextAttribute(p_hWriteHandle,  BACKGROUND_BLUE | FOREGROUND_RED |
  //      FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

  //  FlushConsoleInputBuffer(p_hReadHandle);

    while(1)
    {
  
      if (p_Framework->GetShutdown() == true || p_bRun == true)
      {
        break;
      }

    memset(data, 0, sizeof(data));

    PrintText(">");
    //    ReadFile(p_hReadHandle, data, sizeof(data) - 1, &read, NULL);

    //    data[strlen(data) - 2] = 0;

    //    if(PreprocessInput(data) == false)
    //    {
    //      p_App->On2D((char *)&data);
    //    }
  }

  return;
}

bool SDLWindowText::PreprocessInput(char*InputLine)
{

  bool result = false;

  if (strcmp(InputLine, "exit") == 0)
  {
    PrintText("Shutting down...\n");
    p_Framework->TriggerShutdown();
    result = true;
  }

  if (strcmp(InputLine, "?") == 0 ||
      strcmp(InputLine, "help") == 0)
  {
    PrintText("\nCommands Available\n");
    PrintText("-----------------------------------------------------\n");
    PrintText("exit             - This will exit the program.\n");
    PrintText("run              - This will start the program.\n");

    p_App->RequestCommands();

    PrintText("\n");
    result = true;
  }

  if (strcmp(InputLine, "run") == 0)
  {
    p_bRun = true;
    result = true;
  }

  return result;
}


void  SDLWindowText::ReadText(char * text)
{


}
