#ifndef __FACTORYINPUT_H_
#define __FACTORYINPUT_H_

// The Factory for Input.

#include "input/StandardInput.h"
#include "standard/StandardFramework.h"
#include "standard/FrameworkError.h"

class FactoryInput: private StandardFramework
{

private:

  short count;

  _INPUT_DESCRIPTION *id;

public:

  FactoryInput();
  ~FactoryInput();

  // Standard Framework - required for every framework object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  // ----

  short GetCount();
  short GetDescription(short Index, _INPUT_DESCRIPTION **Description);
  short Create(short ID, StandardInput **Input);
};

#endif