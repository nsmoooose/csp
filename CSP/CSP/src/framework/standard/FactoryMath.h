#ifndef __FACTORYMATH_H_
#define __FACTORYMATH_H_

// The Factory for networking.

#include "StandardMath.h"
#include "StandardFramework.h"
#include "FrameworkError.h"

class FactoryMath: private StandardFramework
{

private:

  short count;

  _MATH_DESCRIPTION *md;

public:

  FactoryMath();
  ~FactoryMath();

  // Standard Framework - required for every framework object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  // ----

  short GetCount();
  short GetDescription(short Index, _MATH_DESCRIPTION **Description);
  short Create(short ID, StandardMath **Math);
};

#endif

