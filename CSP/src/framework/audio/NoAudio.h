// NoAudio specific for StandardAudio

#include "StandardAudio.h"

/** This is a dummy class-implementation for testing
purposes. Will be used if the OS has no sound-support or
a valid audio-module has not been added to the framework.
<br>
All functions should return immedately, without wasting any CPU-cycles :)*/
class NoAudioClass: public StandardAudio
{

private:


public:
  NoAudioClass();
  ~NoAudioClass();

  // Standard Framework - required for every framework object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  // Standard Audio
  int Init() {return 0;}
  int Shutdown() {return 0;}

  int Load_Sound(char *_filename) {return 0;}
  int Cache_Sound(int _handle, char _static) {return 0;}
  int Play_Sound(int _handle, int _volume=255, int _looped=0) {return 0;}
  int Stop_Sound(int _handle) {return 0;}
  int Continue_Sound(int _handle, int _volume=255, int _looped=0) {return 0;}
  int UnCache_Sound(int _handle) {return 0;}
  int Discard_Sound(int _handle) {return 0;}
};
