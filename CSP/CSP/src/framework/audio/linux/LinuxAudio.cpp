#include "LinuxAudio.h"

LinuxAudio::LinuxAudio(){
          for (int i=0; i<=2; i++){
                lispos[i] = 0.0f;
                zeroes[i] = 0.0f;
                position[i]=0.0f;
        };
        front[0] =0.0f; front[1]=0.0f; front[2]=-1.0f; front[3]=0.0f; front[4]=1.0f; front[5]=0.0f;
        back[0] =0.0f; back[1]=0.0f; back[2]=1.0f; back[3]=0.0f; back[4]=1.0f; back[5]=0.0f;
        source_id[0] =0;

        alListenerfv(AL_POSITION, zeroes);
        alListenerfv(AL_VELOCITY, zeroes);
        alListenerfv(AL_ORIENTATION, front);

        alGenBuffers( 1, &bufferSize );
        }
LinuxAudio::LinuxAudio(const char *fName){
        ALfloat zeroes[] = { 0.0f, 0.0f, 0.0f };
        ALfloat back[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
        ALfloat front[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

        source_id[0] =0;

        alListenerfv(AL_POSITION, zeroes);
        alListenerfv(AL_VELOCITY, zeroes);
        alListenerfv(AL_ORIENTATION, front);

        alGenBuffers( 1, &bufferSize );
        waveFile=fName;
        ERROR = alutLoadWAV(fName, &wave, &format, &size, &bits, &freq);
        if(ERROR == AL_FALSE) {
                fprintf(stderr, "LINUX_AUDIO: Could not Load %s\n",fName);
                }
        else {
                fprintf(stdout, "LINUX_AUDIO: loaded file %s at %i  bits and %i Hz\n", fName, bits, freq);
                };

        alBufferData( bufferSize, format, wave, size, freq);
        free(wave);

        alGenSources (2, source_id);

         alSourcefv(source_id[0], AL_POSITION, position);
         alSourcei(source_id[0], AL_BUFFER, bufferSize);
         alSourcei(source_id[0], AL_LOOPING, AL_TRUE);

        position[0]*=-1.0f;

         alSourcefv(source_id[1], AL_POSITION, position);
         alSourcei(source_id[1], AL_BUFFER, bufferSize);
         alSourcei(source_id[1], AL_LOOPING, AL_TRUE);
         }

LinuxAudio::LinuxAudio(const char *fName, ALboolean moving, ALboolean echooe){
        ALfloat zeroes[] = { 0.0f, 0.0f, 0.0f };
        ALfloat back[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
        ALfloat front[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

        source_id[0] =0;

        alListenerfv(AL_POSITION, zeroes);
        alListenerfv(AL_VELOCITY, zeroes);
        alListenerfv(AL_ORIENTATION, front);

        alGenBuffers( 1, &bufferSize );
        waveFile=fName;
        ERROR = alutLoadWAV(fName, &wave, &format, &size, &bits, &freq);
        if(ERROR == AL_FALSE) {
                fprintf(stderr, "LINUX_AUDIO: Could not Load %s\n",fName);
                }
        else {
                fprintf(stdout, "LINUX_AUDIO: loaded file %s at %i  bits and %i Hz\n", fName, bits, freq);
                };

        alBufferData( bufferSize, format, wave, size, freq);
        free(wave);

        talReverbScale = (void (*)(ALuint sid, ALfloat param))GP("alReverbScale_LOKI");
        talReverbDelay = (void (*)(ALuint sid, ALfloat param))GP("alReverbDelay_LOKI");

       if (moving){
                is_movingSource=AL_TRUE;
                is_reverbSource=AL_FALSE;
                }
        else if(echooe){
                        is_reverbSource=AL_TRUE;
                        is_movingSource=AL_FALSE;
                        ALfloat position[] = { 2.0f, 0.0f, 4.0f };

                        alGenSources( 1, &reverb_sid);

                        alSourcefv(reverb_sid, AL_POSITION, position );
                        alSourcefv(reverb_sid, AL_VELOCITY, zeroes );
                        alSourcefv(reverb_sid, AL_ORIENTATION, back );
                        alSourcei(reverb_sid, AL_BUFFER, bufferSize );

                        talReverbScale(reverb_sid, 0.35);
                        talReverbDelay(reverb_sid, 1);
                        }
                else if (echooe&&moving){
                                is_movingSource=AL_TRUE;
                                is_reverbSource=AL_TRUE;
                                }
                        else{ is_reverbSource=AL_FALSE;
                                is_movingSource=AL_FALSE;

                                alGenSources (2, source_id);

                                alSourcefv(source_id[0], AL_POSITION, position);
                                alSourcei(source_id[0], AL_BUFFER, bufferSize);
                                alSourcei(source_id[0], AL_LOOPING, AL_TRUE);
                                alSourcePlay(source_id[0]);
                                position[0]*=-1.0f;
                                alSourcefv(source_id[1], AL_POSITION, position);
                                alSourcei(source_id[1], AL_BUFFER, bufferSize);
                                alSourcei(source_id[1], AL_LOOPING, AL_TRUE);
                                };
         }

LinuxAudio::~LinuxAudio(){
        }

void LinuxAudio::playSound(){
        fprintf(stdout, "LINUX_AUDIO: Playing Sound %s \n", waveFile);
        if (is_movingSource){
                }
        else if (is_reverbSource){
                alSourcePlay(reverb_sid);
                }
                else if ((is_reverbSource)&&(is_movingSource)) {
                        }
                        else {
                                alSourcePlay(source_id[0]);
                                micro_sleep(80000);
                                alSourcePlay(source_id[1]);
                                };
        }

void LinuxAudio::playSound(unsigned long int duration){
        this->playSound();
        micro_sleep(duration);
        this->stopSound();
        }

void LinuxAudio::loadSound(char *fName){
      alGenBuffers( 1, &bufferSize );

       ERROR = alutLoadWAV(fName, &wave, &format, &size, &bits, &freq);
       if(ERROR == AL_FALSE) {
                fprintf(stderr, "LINUX_AUDIO: Could not Load %s\n",fName);
                }
        else {
                fprintf(stdout, "LINUX_AUDIO: loaded file %s at %i  bits and %i Hz\n", fName, bits, freq);
                };

        alBufferData( bufferSize, format, wave, size, freq);
        free(wave);

        alSourcefv( source_id[0], AL_POSITION, position);
        alSourcei( source_id[0], AL_BUFFER, bufferSize);
        alSourcei(source_id[0], AL_LOOPING, AL_TRUE);
        }

void LinuxAudio::stopSound(){
        alSourceStop(source_id[0]);
        alSourceStop(source_id[1]);
        fprintf(stdout, "LINUX_AUDIO: %s : Source %i stopped!\n", waveFile, 0);
        }

void LinuxAudio::pauseSound(){
        alSourcePause(source_id[0]);
        alSourcePause(source_id[1]);
        fprintf(stdout, "LINUX_AUDIO: %s : Source %i paused!\n", waveFile, 0);
        }

void LinuxAudio::pauseSound(unsigned long int duration){
        alSourcePause(source_id[0]);
         alSourcePause(source_id[1]);
        fprintf(stdout, "LINUX_AUDIO: %s : Source %i paused for %i ms!\n", waveFile, duration);
        micro_sleep(duration);
        alSourcePlay(source_id[0]);
         alSourcePause(source_id[1]);
        }

void LinuxAudio::setListenerPosition(ALfloat * new_position){
        memcpy(new_position, lispos, 12);
        alListenerfv(AL_POSITION, lispos);
        fprintf(stderr, "LINUX_AUDIO: %s :New Listener Position = (%f %f %f)\n", waveFile, lispos[0], lispos[1], lispos[2]);
        }

void LinuxAudio::setListenerPosition(ALfloat x, ALfloat y, ALfloat z){
        lispos[0]=x; lispos[1]=y; lispos[2]=z;
        alListenerfv(AL_POSITION, lispos);
        fprintf(stderr, "LINUX_AUDIO: %s :New Listener Position = (%f %f %f)\n", waveFile, lispos[0], lispos[1], lispos[2]);
        }

void LinuxAudio::setSourcePosition(ALfloat * new_position){
        memcpy(new_position, position, 3*sizeof(ALfloat));
        alSourcefv( source_id[0], AL_POSITION, position);
        fprintf(stderr, "LINUX_AUDIO: %s: Source %i Position changed to (%f %f %f)\n", waveFile, 0, position[0], position[1], position[2]);
        }

void LinuxAudio::discardSound(){
        }

void * LinuxAudio::getWave(){
        return wave;
        }

ALfloat  * LinuxAudio::getLisPos(){
        return lispos;
        }