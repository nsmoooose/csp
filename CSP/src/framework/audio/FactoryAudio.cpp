#include "FactoryAudio.h"
#include "NoAudio.h"
#ifdef DSound
  #include "audio/win/Audio_DX.h"
#endif

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
FactoryAudio::FactoryAudio()
{
  _AUDIO_DESCRIPTION *oldad;
  oldad = 0;

  count = 0;
  ad = 0;

  // add choice of nothing
  count++;
  ad = new _AUDIO_DESCRIPTION[count];
  ad[count - 1].Name = "<no audio>\0";
  ad[count - 1].ID = _NOAUDIO;

  // DSound
  #ifdef DSound
    count++;
    oldad = ad;
    ad = new _AUDIO_DESCRIPTION[count];
    if(count > 1)
    {
      memcpy(ad, oldad, sizeof(_AUDIO_DESCRIPTION) * (count - 1));
    }
    _DELARRAY(oldad);

    ad[count - 1].Name = "DirectSound\0";
    ad[count - 1].ID = _DIRECTSOUND;

  #endif

}

FactoryAudio::~FactoryAudio()
{
  _DELARRAY(ad);
}

// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  FactoryAudio::SetSetup(void *setup)
{

}

short FactoryAudio::GetDependencyCount()
{
  return 0;
}

void  FactoryAudio::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short FactoryAudio::SetDependencyData(short Index, void* Data)
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

void  FactoryAudio::GetSetup(void*setup)
{

}

short FactoryAudio::CheckStatus()
{
  return 0;
}


// ------------------------------------------
//  Lets build us some platforms
// ------------------------------------------
short FactoryAudio::GetCount()
{


  return count;

}

short FactoryAudio::GetDescription(short Index, _AUDIO_DESCRIPTION **Description)
{

  if(Index < 0 || Index > count || count == 0)
  {
    return _INDEX_OUT_OF_RANGE;
  }

  *Description = &ad[Index];

  return 0;
}

short FactoryAudio::Create(short ID, StandardAudio** Audio)
{
  short x;

  for(x = 0; x < count; x++)
  {
    if(ad[x].ID == ID)
    {
      break;
    }
  }

  if(x > count)
  {
    return _BAD_ID;
  }

  switch(ad[x].ID)
  {
    case _NOAUDIO:
      *Audio = new NoAudioClass;
      break;

    #ifdef DSound
    case _DIRECTSOUND:
      *Audio = new Audio_DX;
      break;
    #endif

    default:
      *Audio = 0;
      return _NOT_IMPLEMENTED;
      break;
  }

  return 0;
}
