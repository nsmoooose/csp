#ifdef __LINUX_AUDIO_H__
#define  __LINUX_AUDIO_H__

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include "AL/altypes.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream.h>

#define GP(x)          alGetProcAddress((const ALubyte *) x)

void	(*talReverbScale)(ALuint sid, ALfloat param); //Needed for reverb-effect
void	(*talReverbDelay)(ALuint sid, ALfloat param);

void micro_sleep(unsigned long int n) {
        struct timeval tv = { 0, 0 };

        tv.tv_usec = n;
        select(0, NULL, NULL, NULL, &tv);
        return;
}

class LinuxAudio
{
        private:

                void * context_id;
                void * wave;
                const char * waveFile;
                float ALMAXDISTANCE;    //Maximum Distance a LinuxAudio-Object can have to the source
                ALfloat lispos[2];              //Position of the Listener
                ALfloat position[2];
                ALfloat zeroes[2];              //Simple help-array of zeroes
                ALfloat front[5];               //help-position: FRONT
                ALfloat back[];
                ALsizei size, bits, freq, format;       //The variables for size, bits, frequency (per example 22050), and format of the loaded Wave
                ALuint bufferSize;      //The Size of the created Buffer
                ALboolean ERROR, is_movingSource, is_reverbSource;   //a handsome Failure-boolean. :)
                ALuint source_id[2];  //Array of Source ID's manageable by 1 Object. at time "only" 10 Sources per Project available
                ALuint reverb_sid;

        public:

                LinuxAudio();   //NEVER use the STDContructor, because we NEED a file
                LinuxAudio(const char * fName); //Loads a File while initializing Object
                LinuxAudio(const char *fName, ALboolean moving, ALboolean echooe);
                ~LinuxAudio();
                void playSound();                //Plays the sound as std-Position 0,0,0
                void discardSound();            //unloads a sound
                void setSourcePosition(ALfloat * new_position); //sets the new Source Position
                void setListenerPosition(ALfloat *new_position);
                void setListenerPosition(ALfloat x, ALfloat y, ALfloat z);//sets the Sound-position to specified Location
                void loadSound(char *fName); //Loads a new Sound
                void stopSound();               //stops the Sound
                void pauseSound();
                void pauseSound(unsigned long int duration);
                void playSound(unsigned long int duration);
                void * getWave();              //get the Size of used Samples
                ALfloat * getLisPos();          //get coordinates WHERE the Listener is
                char * getWaveName();           //returns the Name of the loaded Wave
                };

#endif // __LINUX_AUDIO_H__