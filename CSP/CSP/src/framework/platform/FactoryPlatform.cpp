#include "FactoryPlatform.h"

#include "NoPlatform.h"

#ifdef WIN32
  #include "win/WindowsPlatform.h"
#endif

#ifdef LINUX
  #include "linux/LinuxPlatform.h"
#endif

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
FactoryPlatform::FactoryPlatform()
{
  _PLATFORM_DESCRIPTION *oldpd;
  oldpd = 0;

  count = 0;
  pd = 0;

  count++;
  pd = new _PLATFORM_DESCRIPTION[count];
  pd[count - 1].Name = "<no platform>\0";
  pd[count - 1].ID = _NOPLATFORM;

  #ifdef WIN32
    count++;
    oldpd = pd;
    pd = new _PLATFORM_DESCRIPTION[count];
    if(count > 1)
    {
      memcpy(pd, oldpd, sizeof(_PLATFORM_DESCRIPTION) * (count - 1));
    }
    _DELARRAY(oldpd);

    pd[count - 1].Name = "Windows 95/98/2000\0";
    pd[count - 1].ID = _WINDOWS32;
    pd[count - 1].VersionMajor = 0;
    pd[count - 1].VersionMinor = 0;
  #endif

  #ifdef LINUX
    count++;
    oldpd = pd;
    pd = new _PLATFORM_DESCRIPTION[count];
    if(count > 1)
    {
      memcpy(pd, oldpd, sizeof(_PLATFORM_DESCRIPTION) * (count - 1));
    }
    _DELARRAY(oldpd);

    pd[count - 1].Name = "Linux i386\0";
    pd[count - 1].ID = _LINUX;
    pd[count - 1].VersionMajor = 0;
    pd[count - 1].VersionMinor = 0;
    //fprintf(stderr, "added Linux\n");
  #endif
}

FactoryPlatform::~FactoryPlatform()
{
  _DELARRAY(pd);
}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  FactoryPlatform::SetSetup(void *setup)
{

}

short FactoryPlatform::GetDependencyCount()
{
  return 0;
}

void  FactoryPlatform::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short FactoryPlatform::SetDependencyData(short Index, void* Data)
{
  switch(Index)
  {
    case 0:
      return _NO_DEPENDENCY_TO_SET;
      break;
    default:
      return _NO_DEPENDENCY_TO_SET;
      break;
  }
  return 0;
}

void  FactoryPlatform::GetSetup(void*setup)
{

}

short FactoryPlatform::CheckStatus()
{
  return 0;
}


// ------------------------------------------
//  Lets build us some platforms
// ------------------------------------------
short FactoryPlatform::GetCount()
{


  return count;

}

short FactoryPlatform::GetDescription(short Index, _PLATFORM_DESCRIPTION **Description)
{

  if(Index < 0 || Index > count || count == 0)
  {
    return _INDEX_OUT_OF_RANGE;
  }

  *Description = &pd[Index];

  return 0;
}

short FactoryPlatform::Create(short ID, StandardPlatform** Platform)
{
  short x;

  for(x = 0; x < count; x++)
  {
    if(pd[x].ID == ID)
    {
      break;
    }
  }

  if(x >= count)
  {
    return _BAD_ID;
  }

  switch(pd[x].ID)
    {
    case _NOPLATFORM:
      *Platform = new NoPlatformClass;
      break;
      
#ifdef WIN32
    case _WINDOWS32:
      *Platform = new WindowsPlatform;
      break;
#endif
      
#ifdef LINUX
    case _LINUX:
      *Platform = new LinuxPlatform;
      break;
#endif
      
    default:
      *Platform = 0;
      fprintf(stderr,"NO PLATFORM\n");
      return _NOT_IMPLEMENTED;
      break;
    }
  
  return 0;
}
