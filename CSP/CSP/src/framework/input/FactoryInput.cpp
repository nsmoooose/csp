#include "FactoryInput.h"

#include "NoInput.h"

#ifdef DirectInput
  #include "win/DirectInput.h"
#endif


// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
FactoryInput::FactoryInput()
{
  _INPUT_DESCRIPTION *oldid;
  oldid = 0;
  count = 0;

  id = 0;

  // add choice of nothing  count = 0;
  count++;
  id = new _INPUT_DESCRIPTION[count];
  id[count - 1].Name = "<no input>\0";
  id[count - 1].ID = _NOINPUT;

  // DirectInput
  #ifdef DirectInput
    count++;
    oldid = id;
    id = new _INPUT_DESCRIPTION[count];
    if(count > 1)
    {
      memcpy(id, oldid, sizeof(_INPUT_DESCRIPTION) * (count - 1));
    }
    _DELARRAY(oldid);

    id[count - 1].Name = "DirectInput\0";
    id[count - 1].ID = _DIRECTINPUT;
  #endif

}

FactoryInput::~FactoryInput()
{
  _DELARRAY(id);
}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  FactoryInput::SetSetup(void *setup)
{

}

short FactoryInput::GetDependencyCount()
{
  return 0;
}

void  FactoryInput::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short FactoryInput::SetDependencyData(short Index, void* Data)
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

void  FactoryInput::GetSetup(void*setup)
{

}

short FactoryInput::CheckStatus()
{
  return 0;
}


// ------------------------------------------
//  Lets build us some platforms
// ------------------------------------------
short FactoryInput::GetCount()
{


  return count;

}

short FactoryInput::GetDescription(short Index, _INPUT_DESCRIPTION **Description)
{

  if(Index < 0 || Index > count || count == 0)
  {
    return _INDEX_OUT_OF_RANGE;
  }

  *Description = &id[Index];

  return 0;
}

short FactoryInput::Create(short ID, StandardInput** Input)
{
  short x;

  for(x = 0; x < count; x++)
  {
    if(id[x].ID == ID)
    {
      break;
    }
  }

  if(x > count)
  {
    return _BAD_ID;
  }

  switch(id[x].ID)
  {
      case _NOINPUT:
        *Input = new NoInputClass;
        break;

    #ifdef DirectInput
      case _DIRECTINPUT:
        *Input = new DirectInputClass;
        break;
    #endif

    default:
      *Input = 0;
      return _NOT_IMPLEMENTED;
      break;
  }

  return 0;
}
