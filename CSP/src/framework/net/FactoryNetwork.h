#ifndef __FACTORYNETWORK_H_
#define __FACTORYNETWORK_H_

// The Factory for networking.

#include "net/StandardNetwork.h"
#include "standard/StandardFramework.h"
#include "standard/FrameworkError.h"

class FactoryNetwork: private StandardFramework
{

private:

  short count;

  _NETWORK_DESCRIPTION *nd;

public:

  FactoryNetwork();
  ~FactoryNetwork();

  // Standard Framework - required for every framework object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  // ----

  short GetCount();
  short GetDescription(short Index, _NETWORK_DESCRIPTION **Description);
  short Create(short ID, StandardNetwork **Network);
};

#endif
