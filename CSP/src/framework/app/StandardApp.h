#ifndef __STANDARDAPP_H_
#define __STANDARDAPP_H_

// The pure abstract class for Apps.

#include "app/TypesApp.h"

class StandardApp: private StandardFramework
{

private:


public:

  virtual short CreateObjects() = 0;

  virtual short Initialize() = 0;
  virtual void  Run() = 0;
  virtual void  Uninitialize() = 0;

  virtual void  DeleteObjects() = 0;
};

#endif

