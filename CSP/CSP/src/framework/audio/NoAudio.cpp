#include "NoAudio.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
NoAudioClass::NoAudioClass()
{
}

NoAudioClass::~NoAudioClass()
{
}


// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  NoAudioClass::SetSetup(void *setup)
{
}

short NoAudioClass::GetDependencyCount()
{
  return 0;
}

void  NoAudioClass::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short NoAudioClass::SetDependencyData(short ID, void* Data)
{
  return 0;
}

void  NoAudioClass::GetSetup(void*setup)
{
}

short NoAudioClass::CheckStatus()
{
  return 0;
}
