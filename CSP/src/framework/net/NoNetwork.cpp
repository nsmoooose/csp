#include "NoNetwork.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
NoNetworkClass::NoNetworkClass()
{
}

NoNetworkClass::~NoNetworkClass()
{
}


// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  NoNetworkClass::SetSetup(void *setup)
{
}

short NoNetworkClass::GetDependencyCount()
{
  return 0;
}

void  NoNetworkClass::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short NoNetworkClass::SetDependencyData(short ID, void* Data)
{
  return 0;
}

void  NoNetworkClass::GetSetup(void*setup)
{
}

short NoNetworkClass::CheckStatus()
{
  return 0;
}