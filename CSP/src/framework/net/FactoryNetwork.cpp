#include "FactoryNetwork.h"
#include "NoNetwork.h"

#ifdef WIN32
#include "Windows/WindowsNetwork.h"
#endif

#ifdef LINUX
#include "linux/LinuxNetwork.h"
#endif

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
FactoryNetwork::FactoryNetwork()
{
  count = 0;
  nd = 0;

  _NETWORK_DESCRIPTION *oldnd = 0;

  count++;
  nd = new _NETWORK_DESCRIPTION[count];
  nd[count - 1].Name = "<no network>\0";
  nd[count - 1].ID = _NONETWORK;

  #ifdef WIN32
    count++;
    oldnd = nd;
    nd = new _NETWORK_DESCRIPTION[count];
    if(count > 1)
    {
      memcpy(nd, oldnd, sizeof(_NETWORK_DESCRIPTION) * (count - 1));
    }
    _DELARRAY(oldnd);

    nd[count - 1].Name = "A network\0";
    nd[count - 1].ID = _NETWORK;
  #endif
  #ifdef LINUX
    count++;
    oldnd = nd;
    nd = new _NETWORK_DESCRIPTION[count];
    if(count > 1)
    {
      memcpy(nd, oldnd, sizeof(_NETWORK_DESCRIPTION) * (count - 1));
    }
    _DELARRAY(oldnd);

    nd[count - 1].Name = "A network\0";
    nd[count - 1].ID = _NETWORK;
  #endif
}

FactoryNetwork::~FactoryNetwork()
{
  _DELARRAY(nd);
}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  FactoryNetwork::SetSetup(void *setup)
{

}

short FactoryNetwork::GetDependencyCount()
{
  return 0;
}

void  FactoryNetwork::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short FactoryNetwork::SetDependencyData(short Index, void* Data)
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

void  FactoryNetwork::GetSetup(void*setup)
{

}

short FactoryNetwork::CheckStatus()
{
  return 0;
}


// ------------------------------------------
//  Lets build us some platforms
// ------------------------------------------
short FactoryNetwork::GetCount()
{

  return count;

}

short FactoryNetwork::GetDescription(short Index, _NETWORK_DESCRIPTION **Description)
{

  if(Index < 0 || Index > count || count == 0)
  {
    return _INDEX_OUT_OF_RANGE;
  }

  *Description = &nd[Index];

  return 0;
}

short FactoryNetwork::Create(short ID, StandardNetwork** Network)
{
  short x;

  for(x = 0; x < count; x++)
  {
    if(nd[x].ID == ID)
    {
      break;
    }
  }

  if(x > count)
  {
    return _BAD_ID;
  }

  switch(nd[x].ID)
  {
	case _NONETWORK:
		*Network = new NoNetworkClass();
		break;
#ifdef WIN32
    case _NETWORK:	
		*Network = (StandardNetwork *) new WindowsNetwork();
		break;
#endif
#ifdef LINUX
  case _NETWORK:	
    //
    *Network = (StandardNetwork *) new NoNetworkClass();
    break;
#endif
    default:
		*Network = 0;
		return _NOT_IMPLEMENTED;
		break;
  }

  return 0;
}
