#include "standard/FactoryMath.h"

#ifdef DirectMath
  #include "win/DirectMath.h"
#endif
#include "standard/SimpleMath.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
FactoryMath::FactoryMath()
{
  _MATH_DESCRIPTION *oldmd;
  oldmd = 0;

  count = 0;
  md = 0;

  count++;
  md = new _MATH_DESCRIPTION[count];
  md[count - 1].Name = "<no math>\0";
  md[count - 1].ID = _NOMATH;

#ifdef DirectMath
  count++;
  oldmd = md;
  md = new _MATH_DESCRIPTION[count];
  if(count > 1)
    {
      memcpy(md, oldmd, sizeof(_MATH_DESCRIPTION) * (count - 1));
    }
  _DELARRAY(oldmd);
  
  md[count - 1].Name = "DirectMath\0";
  md[count - 1].ID = _DIRECTMATH;
#endif
  
  count++;
  oldmd = md;
  md = new _MATH_DESCRIPTION[count];
  if(count > 1)
    {
      memcpy(md, oldmd, sizeof(_MATH_DESCRIPTION) * (count - 1));
    }
  _DELARRAY(oldmd);
  
  md[count - 1].Name = "SimpleMath\0";
  md[count - 1].ID = _SIMPLEMATH;

    
}

FactoryMath::~FactoryMath()
{
  _DELARRAY(md);
}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  FactoryMath::SetSetup(void *setup)
{

}

short FactoryMath::GetDependencyCount()
{
  return 0;
}

void  FactoryMath::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short FactoryMath::SetDependencyData(short Index, void* Data)
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

void  FactoryMath::GetSetup(void*setup)
{

}

short FactoryMath::CheckStatus()
{
  return 0;
}


// ------------------------------------------
//  Lets build us some platforms
// ------------------------------------------
short FactoryMath::GetCount()
{

  return count;

}

short FactoryMath::GetDescription(short Index, _MATH_DESCRIPTION **Description)
{

  if(Index < 0 || Index > count || count == 0)
  {
    return _INDEX_OUT_OF_RANGE;
  }

  *Description = &md[Index];

  return 0;
}

short FactoryMath::Create(short ID, StandardMath** Math)
{
  short x;

  for(x = 0; x < count; x++)
  {
    if(md[x].ID == ID)
    {
      break;
    }
  }

  if(x > count)
  {
    return _BAD_ID;
  }

  switch(md[x].ID)
    {
      
    case _NOMATH:
      *Math = 0;
      break;
      
#ifdef DirectMath
    case _DIRECTMATH:
      *Math = new DirectMathClass;
      break;
#endif
      
    case _SIMPLEMATH:
      *Math = new SimpleMath;
      break;

    default:
      *Math = 0;
      return _NOT_IMPLEMENTED;
      break;
  }

  return 0;
}
