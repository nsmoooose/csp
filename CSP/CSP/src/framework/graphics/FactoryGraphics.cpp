#include "graphics/FactoryGraphics.h"

#include "graphics/NoGraphics.h"

#ifdef WIN32
#include "win/D3D.h"
#include "win/openglgraphics.h"
#endif
#ifdef LINUX
#include "linux/OpenGLGraphics.h"
#endif
//#endif

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
FactoryGraphics::FactoryGraphics()
{
  _GRAPHICS_DESCRIPTION *oldgd;
  oldgd = 0;

  gd = 0;
  count = 0;

  // add choice of nothing
  count++;
  gd = new _GRAPHICS_DESCRIPTION[count];
  gd[count - 1].Name = "<no graphics>\0";
  gd[count - 1].ID = _NOGRAPHICS;

  // add D3D choice
  #ifdef D3D
    count++;
    oldgd = gd;
    gd = new _GRAPHICS_DESCRIPTION[count];
    if(count > 1)
    {
      memcpy(gd, oldgd, sizeof(_GRAPHICS_DESCRIPTION) * (count - 1));
    }
    _DELARRAY(oldgd);

    gd[count - 1].Name = "Direct3D 8\0";
    gd[count - 1].ID = _D3D8;

  #endif

  // add OpenGL Choice
  #ifdef OGL
    count++;
    oldgd = gd;
    gd = new _GRAPHICS_DESCRIPTION[count];
    if (count > 1)
      {
	memcpy(gd, oldgd, sizeof(_GRAPHICS_DESCRIPTION) * (count - 1));
      }
    _DELARRAY(oldgd);
    
    gd[count - 1].Name = "OpenGL\0";
    gd[count - 1].ID = _OGL;
  #endif

}

FactoryGraphics::~FactoryGraphics()
{
  _DELARRAY(gd);
}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  FactoryGraphics::SetSetup(void *setup)
{

}

short FactoryGraphics::GetDependencyCount()
{
  return 0;
}

void  FactoryGraphics::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short FactoryGraphics::SetDependencyData(short Index, void* Data)
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

void  FactoryGraphics::GetSetup(void*setup)
{

}

short FactoryGraphics::CheckStatus()
{
  return 0;
}


// ------------------------------------------
//  Lets build us some platforms
// ------------------------------------------
short FactoryGraphics::GetCount()
{
  return count;
}

short FactoryGraphics::GetDescription(short Index, _GRAPHICS_DESCRIPTION **Description)
{

  if(Index < 0 || Index > count || count == 0)
  {
    return _INDEX_OUT_OF_RANGE;
  }

  *Description = &gd[Index];

  return 0;
}

short FactoryGraphics::Create(short ID, StandardGraphics** Graphics)
{
  short x;

  for(x = 0; x < count; x++)
  {
    if(gd[x].ID == ID)
    {
      break;
    }
  }

  if(x > count)
  {
    return _BAD_ID;
  }

  switch(gd[x].ID)
    {
      
    case _NOGRAPHICS:
      *Graphics = new NoGraphicsClass;
      break;
      
    #ifdef D3D
    case _D3D8:
      *Graphics = new D3DGraphics;
      break;
    #endif
    #ifdef OGL
    case _OGL:
      *Graphics = new OpenGLGraphics;
      break;
    #endif

    default:
      *Graphics = 0;
      return _NOT_IMPLEMENTED;
      break;
    }

  return 0;
}
