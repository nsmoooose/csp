#include "FrameworkError.h"

FrameworkError::FrameworkError()
{
}

FrameworkError::~FrameworkError()
{
}

// ----------------------------------------------------
// ConvertErrorToString
// Converts a constant to char*
// ----------------------------------------------------
char* FrameworkError::ConvertErrorToString(short sError)
{
  switch(sError)
  {
    case _DEPENDENCY_NOT_SET:
      return "This component requires another dependency.\0";
      break;
    case _SETUP_NOT_SET:
      return "This component requires it's Setup() struct populated.\0";
      break;
    case _CREATING_3DDEVICE_FAILED:
      return "There was an error trying to create the 3D Device.\0";
  }

  return 0;
}

// ----------------------------------------------------
// WriteToLog
// Writes to a .log file in the application directory
// ----------------------------------------------------
void FrameworkError::WriteToLog(char *Function, char *Line)
{

  FILE *fil;
  char buff2[650];
  time_t ltime;

  time(&ltime);

  // UGH. assume the whole message is less than 650 characters.
  sprintf(buff2, "%s%s\n%s\n\n", ctime(&ltime), Function, Line);
      
  char filename[50];

  // UGH. assume the exename+log is not over 50 characters.
  strcpy(filename, "error.log");

  fil = fopen(filename, "a+");
  fprintf(fil, buff2);
  fclose(fil);

  return;
}
