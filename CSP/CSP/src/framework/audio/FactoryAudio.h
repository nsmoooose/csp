#ifndef __FACTORYAUDIO_H_
#define __FACTORYAUDIO_H_

// The Factory for Audio.

#include "audio/StandardAudio.h"
#include "standard/StandardFramework.h"
#include "standard/FrameworkError.h"

/** This class checks all sound-modules.
    If a module was selected and created the
    factory can be wasted, its just used
    for creating instances of classes.
    <br>
    <b>IMPORTANT</b> :<br>
    If you add a new audio-module you will have to modify this
    class too. If you do not so, the application will not be
    able to make use of your brandnew module !!!*/
class FactoryAudio: private StandardFramework
{

private:

  short count;

  _AUDIO_DESCRIPTION *ad;

public:

  FactoryAudio();
  ~FactoryAudio();

  /** @name Standard Framework - required for every framework object*/
  //@{
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();
  //@}


  /** Returns the number of available sound-modules.*/
  short GetCount();
  /** Returns the detailed description of a sound-module.
      @param Index Index of the modules (range : 0 .. GetCount() )
      @param [OUT] Description Adress of a struct-pointer. The pointer
      will be set to the modules struct, so DONT modify it, or you will
      be killed.*/
  short GetDescription(short Index, _AUDIO_DESCRIPTION **Description);
  /** Creates an instance of the selected audio-module.
      @param ID Index of the module
      @param Audio [OUT] Adress of an audio-pointer. The pointer will
      point to the sound-module.*/
  short Create(short ID, StandardAudio **Audio);
};

#endif
