#ifndef __FACTORYPLATFORM_H_
#define __FACTORYPLATFORM_H_

// The Factory for Platform.

#include "platform/StandardPlatform.h"
#include "standard/StandardFramework.h"
#include "standard/FrameworkError.h"

class FactoryPlatform: private StandardFramework
{

private:

  short count;

  _PLATFORM_DESCRIPTION *pd;

public:

  FactoryPlatform();
  ~FactoryPlatform();

  // Standard Framework - required for every framework object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  // ----

  short GetCount();
  short GetDescription(short Index, _PLATFORM_DESCRIPTION **Description);
  short Create(short ID, StandardPlatform **Platform);
};

#endif