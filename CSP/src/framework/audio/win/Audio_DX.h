// DirectSound available .. COOL !

#include "platform/StandardPlatform.h"
#include "../StandardAudio.h"
#include "../AudioData.h"
#include <vector>
#include <dsound.h>

using namespace std;

/// Defines the Data-list-type for easy use of the STL-list.
typedef vector<AudioData *>         LISTDATA;
/// Defines the Data-list-type for easy use of the STL-list.
typedef vector<LPDIRECTSOUNDBUFFER> LISTBUFFER;


class Audio_DX: public StandardAudio
{

private:
  StandardPlatform    *platform;
  HRESULT              dsrval;
  _AUDIO_INIT_STRUCT  *globals;
  
  /**@name Storage*/
  //@{
  /// This list contains the pointers to the Audio-Datas.
  LISTDATA             m_listdata;
  /// This list contains the list of DirectSoundBuffers used.
  LISTBUFFER           m_listbuffer;
  //@}

  /**@name DirectSound-related internal variables*/
  //@{
  DSCAPS               dscaps;
  DSBUFFERDESC         dsbd;
  /// Pointer to the DirectSound-Interface/Device.
  LPDIRECTSOUND        lpds;
  /** Pointer to the Primary SoundBuffer .. all sounds will be mixed into this buffer,
      which will be in loop.*/
  LPDIRECTSOUNDBUFFER  lpdsbPrimary;
  //@}

  /**@name Private functions*/
  //@{
  LPDIRECTSOUNDBUFFER  CreateSB(AudioData *_data, int _static);
  int                  ReleaseSB(int _handle);
  //@}

public:
  /**@name Constructor, Destructor*/
  //@{
  Audio_DX();
  ~Audio_DX();
  //@}

  /**@name Init-Functions
	For more details look in StandardAudio.*/
  //@{
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();
  //@}

  /**@name DirectSound */
  //@{
  /** For DirectX this call will initialize the most important functions.
    Additional the PrimarySoundBuffer will be allocated.
    @return 0 for success, error or warning else*/
  int Init();

  /** This will close the PrimarySoundBuffer. Its like a reset. After calling Init
  (or Init3D) the Shutdown must be executed before any other "Init" will succeed.
  @return 0 on success*/
  int Shutdown();

  /** This will load the sound and return a handle to the stored data.*/
  int Load_Sound(char *_filename);

  /** This will create a DirectSoundBuffer. If you want to play the sound once, this
  function is not needed. But if this sound will occur several times, then I advise
  you to cache it before.
  @param _handle handle of the loaded sound-data
  @param _static In DirectSound WAV-datas can be stored in the hardware. Setting _static to "1"
  will try to store the sound in the hardware. Mixing is done faster in Hardware,
  so please cache your often used sounds.
  @return 0 if success*/
  int Cache_Sound(int _handle, char _static=0);
  /** Plays the sound from the start (byte 0).*/
  int Play_Sound(int _handle, int _volume=255, int _looped=0);
  int Stop_Sound(int _handle);
  /** Plays the sound. If it was stopped it will continue at this position.*/
  int Continue_Sound(int _handle, int _volume=255, int _looped=0);
  /** Deletes the Hardware-buffer of a sound. This gives some memory free, so dont hestitate to
  use it, after a rarely used sound was played.*/
  int UnCache_Sound(int _handle);
  /** Completely discards the sound from memory.*/
  int Discard_Sound(int _handle);
  //@}
};
