#ifndef __LINUX_AUDIO_H__
#define  __LINUX_AUDIO_H__

#include "Framework.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <AL/alext.h>
#include <AL/altypes.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define GP(x)          alGetProcAddress((const ALubyte *) x)

using namespace std;

typedef struct soundstruct{
	ALuint source_id;
	ALuint buffer;
	ALfloat position[3];
	float ALMAXDISTANCE;
	struct soundstruct * next;
	struct soundstruct * prev;
} soundStore;

void micro_sleep(unsigned long int n);

//static ALboolean alutLoadWAVFile(const char *fname, ALsizei *format, ALvoid **wave, ALsizei *size, ALsizei *freq );

//static ALboolean alutUnloadWAV(ALenum format, ALvoid *wave,ALsizei size, ALsizei freq );

class LinuxAudio : public  StandardAudio
{

  private:

                void * context_id; 		//we need a context to the Device used
                void * wave;
                const char * waveFile;
                float ALMAXDISTANCE;   		//Maximum Distance a LinuxAudio-Object can have to the source
                ALfloat lispos[3];              //Position of the Listener
                ALfloat position[3];
                ALfloat zeroes[3];              //Simple help-array of zeroes
                ALfloat front[6];               //help-position: FRONT
                ALfloat back[6];
                ALsizei size, bits, freq, format;       //The variables for size, bits, frequency (per example 22050), and format of the loaded Wave
                ALuint bufferSize;  	    	//created buffer
                ALuint source_id;  		//Array of Source ID's manageable by 1 Object. at time "only" 10 Sources per Project available
                ALuint reverb_sid;
		soundStore * anchor;		//the first element of our sounds
		ALCdevice * dev;		//The Device
		int attrlist[3];
		ALuint stereo;



  public:

  LinuxAudio();
     virtual ~LinuxAudio();

    /** This will initialize the specific sound-module. For Linux it will try to
    get controll of /dev/dsp, Windows will create a DirectSound-access.*/
    
    virtual int Initialize(StandardApp *App, _HANDLE WindowHandle);
    
    /** Shuts the interface. All resources will be released, so some other applications
    or a new Init can access the device. UnCache all sounds before, the interface-related
    memories may become INVALID !!!!!!!!*/
    
    virtual int Uninitialize();

    /** Loads a sound into the memory.
	@param _filename String containing the filename (should be relativ path).
	@return Handle to the sound-data. The returned handle identifies the sound in the system. If
	a value below 0 occured .. then an error occured while loading.*/
	
    virtual int Load_Sound(char *_filename);

    /** Prepares the sound for being played via the device.
    @param _handle Handle of the sound
    @param _static indicates whether the sound shall be stored in RAM of the sound-card (1) or not (0)*/
    
    virtual int Cache_Sound(int _handle, char _static=0);

    /** Plays the sound.
    @param _handle Handle to the loaded sound.
    @param _volume Volume of the sound (range 0..255)
    @param _looped if set to "1" the sound will be played endless*/
    
    virtual int Play_Sound(int _handle, int _volume=255, int _looped=0);
    
    /** Stops the sound (if looped or not finished yet).
    @param _handle*/
    
    virtual int Stop_Sound(int _handle);

    /** Continues to play a stoped sound. Can also be used to play a new sound, in this
    case it starts from position "0".
     @param _handle Handle to the internally stored sound.
     @param _volume Volume of the sound to be played (range 0..255)
     @param _looped if set to "1" the sound will be played in a loop*/
     
    virtual int Continue_Sound(int _handle, int _volume=255, int _looped=0);

    /** Removes the sound from soundcards memory (if <b>_static</b> was "1").
     @param _handle Handle to the internally stored sound*/
     
    virtual int UnCache_Sound(int _handle);

    /** Releases all memory or resources associated with this sound.
     @param _handle Handle to the internally stored sound*/
     
    virtual int Discard_Sound(int _handle);
};


//class LinuxAudio : StandardAudio
//{
//        private:
//
//                void * context_id;
//                void * wave;
//                const char * waveFile;
//                float ALMAXDISTANCE;    //Maximum Distance a LinuxAudio-Object can have to the source
//                ALfloat lispos[2];              //Position of the Listener
//                ALfloat position[2];
//                ALfloat zeroes[2];              //Simple help-array of zeroes
//                ALfloat front[5];               //help-position: FRONT
//                ALfloat back[];
//                ALsizei size, bits, freq, format;       //The variables for size, bits, frequency (per example 22050), and format of the loaded Wave
//                ALuint bufferSize;      //The Size of the created Buffer
//                ALboolean ERROR, is_movingSource, is_reverbSource;   //a handsome Failure-boolean. :)
//                ALuint source_id[2];  //Array of Source ID's manageable by 1 Object. at time "only" 10 Sources per Project available
//                ALuint reverb_sid;
//
//        public:
//
//                LinuxAudio();   //NEVER use the STDContructor, because we NEED a file
//                LinuxAudio(const char * fName); //Loads a File while initializing Object
//                LinuxAudio(const char *fName, ALboolean moving, ALboolean echooe);
//                ~LinuxAudio();
//                void playSound();                //Plays the sound as std-Position 0,0,0
//                void discardSound();            //unloads a sound
//                void setSourcePosition(ALfloat * new_position); //sets the new Source Position
//                void setListenerPosition(ALfloat *new_position);
//                void setListenerPosition(ALfloat x, ALfloat y, ALfloat z);//sets the Sound-position to specified Location
//                void loadSound(char *fName); //Loads a new Sound
//                void stopSound();               //stops the Sound
//                void pauseSound();
//                void pauseSound(unsigned long int duration);
//                void playSound(unsigned long int duration);
//                void * getWave();              //get the Size of used Samples
//                ALfloat * getLisPos();          //get coordinates WHERE the Listener is
//                char * getWaveName();           //returns the Name of the loaded Wave
//                };

#endif // __LINUX_AUDIO_H__
