// The pure abstract class for Audio.

#ifndef SATag

  #define SATag

  #include "TypesAudio.h"
  
  /** Pure virtual class for all Sound-Interfaces. Just functions which do appear
  in this class can be called from the application.
  @page Sound
  @section Sound Sound Interface
  The sound interface provides the application for now with a basic functionality
  to play single sounds. The sounds are loaded from a file, the only supported file-format
  is the uncompressed WAV-format. All sound-datas should be in the same format (samplerate, bitrate)
  to make the mixing more efficient !
  <p>If multiple sounds are played at one time the sounds are mixed. Mixing can take some time.
  So its a good idea to store the sounds in the hardware-buffer of the soundcard. How to do ?
  Read on ! ;)
  <p>This are the steps for loading, playing and discarding a sound:
  <ul>
  <li>"Load_Sound" will load a sound and return a handle to this sound (handle is a positive value)
  <li>"Cache_Sound" will load a sound into a more soundinterface-friendly format (if available).
  This is adviced if the sound will be played more than once. If not, this function does not need to be called !
  <li>"Play_Sound" will finally play the sound. If it was not "cached" before the sound will be
  converted into the interface-specific format.
  <li>"Stop_Sound" willstop the sound. The position if not resetted afterwards, so a "Continue_Play" will
  resume at stopped position.
  <li>"Continue_Sound" continues at the stopped position.
  <li>"UnCache_Sound" will remove the sound-interface specific storage of the sound (if available). It should be called
  if the sound is played seldom, but should still remain in memory.
  <li>"Discard_Sound" Removes all resources and allocated memory-blocks of this sound.
  </ul>
  */
  class StandardAudio: public StandardFramework
  {

  private:


  public:
    /** This will initialize the specific sound-module. For Linux it will try to
    get controll of /dev/dsp, Windows will create a DirectSound-access.*/
    virtual int Init() = 0;
    /** Shuts the interface. All resources will be released, so some other applications
    or a new Init can access the device. UnCache all sounds before, the interface-related
    memories may become INVALID !!!!!!!!*/
    virtual int Shutdown() = 0;

    /** Loads a sound into the memory.
	@param _filename String containing the filename (should be relativ path).
	@return Handle to the sound-data. The returned handle identifies the sound in the system. If
	a value below 0 occured .. then an error occured while loading.*/
    virtual int Load_Sound(char *_filename) = 0;

    /** Prepares the sound for being played via the device.
    @param _handle Handle of the sound
    @param _static indicates whether the sound shall be stored in RAM of the sound-card (1) or not (0)*/
    virtual int Cache_Sound(int _handle, char _static=0) = 0;

    /** Plays the sound.
    @param _handle Handle to the loaded sound.
    @param _volume Volume of the sound (range 0..255)
    @param _looped if set to "1" the sound will be played endless*/
    virtual int Play_Sound(int _handle, int _volume=255, int _looped=0)=0;
    
    /** Stops the sound (if looped or not finished yet).
    @param _handle*/
    virtual int Stop_Sound(int _handle) = 0;

    /** Continues to play a stoped sound. Can also be used to play a new sound, in this
    case it starts from position "0".
     @param _handle Handle to the internally stored sound.
     @param _volume Volume of the sound to be played (range 0..255)
     @param _looped if set to "1" the sound will be played in a loop*/
    virtual int Continue_Sound(int _handle, int _volume=255, int _looped=0)=0;

    /** Removes the sound from soundcards memory (if <b>_static</b> was "1").
     @param _handle Handle to the internally stored sound*/
    virtual int UnCache_Sound(int _handle) = 0;

    /** Releases all memory or resources associated with this sound.
     @param _handle Handle to the internally stored sound*/
    virtual int Discard_Sound(int _handle) = 0;
  };

#endif
