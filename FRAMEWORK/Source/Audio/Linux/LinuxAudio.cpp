#include "LinuxAudio.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <AL/altypes.h>

void	(*talReverbScale)(ALuint sid, ALfloat param); //Needed for reverb-effect
void	(*talReverbDelay)(ALuint sid, ALfloat param);

void micro_sleep(unsigned long int n){
        struct timeval tv = { 0, 0 };

        tv.tv_usec = n;
        select(0, NULL, NULL, NULL, &tv);
	return;
};

LinuxAudio::LinuxAudio()
{
	fprintf(stdout,"LinuxAudio::LinuxAudio()\n");
	for (int i=0; i<=2; i++){
		lispos[i] = 0.0f;
		zeroes[i] = 0.0f;
		position[i]=0.0f;
	        };
	for (int i=0; i<=5; i++){
	        front[i] =0.0f; 
	        back[i] =0.0f;
		};
	front[2]=-1.0f; front[4]=1.0f; back[2]=1.0f; back[4]=1.0f;
	source_id=0;
	attrlist[0]=ALC_FREQUENCY;
	attrlist[1]=22050;
	attrlist[2]=ALC_INVALID;
	dev=alcOpenDevice(NULL);
	if(dev==NULL){
		fprintf(stderr,"LINUX_AUDIO: Error opening Device\n");
	};
	context_id=alcCreateContext(dev,attrlist);
	if(context_id==NULL){
		fprintf(stderr,"LINUX_AUDIO: Error making Device current\n");
	};
	alcMakeContextCurrent(context_id);
        alListenerfv(AL_POSITION, zeroes);
        alListenerfv(AL_VELOCITY, zeroes);
        alListenerfv(AL_ORIENTATION, front);
	bufferSize=0;
	anchor=(soundStore*)malloc(sizeof(soundStore));
}

LinuxAudio::~LinuxAudio()
{

}

int LinuxAudio::Initialize(StandardApp *App, _HANDLE WindowHandle)
{
	alGetError();
	alGenBuffers( 1, &bufferSize );
	if(alGetError() != AL_NO_ERROR) {
		fprintf(stderr,"LINUX_AUDIO:- Error creating buffers !!\n");
	};
	alGetError(); /* clear error */
	return 0;
}

int LinuxAudio::Uninitialize()
{
  return 0;
}

int LinuxAudio::Load_Sound(char *_filename)
{
	soundStore * help=anchor;
	int i=0;
	while (help!=NULL){
		help=help->next;
		i++;
		};
	alGenBuffers(1,&bufferSize);
	if(alutLoadWAV(_filename, &wave, &format, &size, &bits, &freq) == AL_FALSE) {
		fprintf(stderr, "LINUX_AUDIO: Could not Load %s\n",_filename);
		return(-1);
		}
	else {
		fprintf(stdout, "LINUX_AUDIO: loaded file %s at %i  bits and %i Hz\n", _filename, bits, freq);
		};
	
	alBufferData(bufferSize,format,wave,size,freq);
	fprintf(stdout, "LINUX_AUDIO: buffered Sound\n", _filename, bits, freq);
	free(wave);
	fprintf(stdout, "LINUX_AUDIO: Freed Memory\n", _filename, bits, freq);
	alGenSources(1, &source_id);
	if(alGetError() != AL_NO_ERROR) {
		fprintf(stderr,"LINUX_AUDIO: Error creating source !!\n");
	}
	else {
		fprintf(stdout, "LINUX_AUDIO:Generated Source\n", _filename, bits, freq);
		}

	alSourcef(source_id,AL_PITCH,1.0f);
	alSourcef(source_id,AL_GAIN,1.0f);
	alSourcefv(source_id,AL_POSITION,zeroes);
	alSourcefv(source_id,AL_VELOCITY,zeroes);
	alSourcei(source_id,AL_BUFFER,bufferSize);
	
//	alSourcef(help->source_id,AL_PITCH,1.0f);
//	alSourcef(help->source_id,AL_GAIN,1.0f);
//	alSourcefv(help->source_id,AL_POSITION,zeroes);
//	alSourcefv(help->source_id,AL_VELOCITY,zeroes);
//	alSourcei(help->source_id,AL_BUFFER,bufferSize);
	if(help==(soundStore*)NULL){
		help=(soundStore*)malloc(sizeof(soundStore));		
	}
	else {
		help=help->next;
	};

//	help->next->prev=help;
	return i; //assuming i as _handle
}

int LinuxAudio::Cache_Sound(int _handle, char _static=0)
{
  return 0;
}

int LinuxAudio::Play_Sound(int _handle, int _volume=255, int _looped=0)
{
	soundStore * help=anchor;
//	for(int i=0;i<=_handle;i++){
//		help=help->next;
//		};
	if (_looped){
//		alSourcei(help->source_id,AL_LOOPING,AL_TRUE);}
		alSourcei(source_id,AL_LOOPING,AL_TRUE);}
	else{	
//		alSourcei(help->source_id,AL_LOOPING,AL_FALSE);};
		alSourcei(source_id,AL_LOOPING,AL_FALSE);};
//	alSourcePlay(help->source_id);
	alSourcePlay(source_id);
    return 0;
}
    
int LinuxAudio::Stop_Sound(int _handle)
{
 	soundStore * help=anchor;
//	for(int i=0;i<=_handle;i++){
//		help=help->next;
//		};
//	alSourceStop(help->source_id);
	alSourceStop(source_id);
	return 0;
}

int LinuxAudio::Continue_Sound(int _handle, int _volume=255, int _looped=0)
{
  return 0;
}

int LinuxAudio::UnCache_Sound(int _handle)
{
  return 0;
}

int LinuxAudio::Discard_Sound(int _handle)
{
	return 0;
}


//LinuxAudio::LinuxAudio()
//{
//          for (int i=0; i<=2; i++){
//                lispos[i] = 0.0f;
//                zeroes[i] = 0.0f;
//                position[i]=0.0f;
//        };
//        front[0] =0.0f; front[1]=0.0f; front[2]=-1.0f; front[3]=0.0f; front[4]=1.0f; front[5]=0.0f;
//        back[0] =0.0f; back[1]=0.0f; back[2]=1.0f; back[3]=0.0f; back[4]=1.0f; back[5]=0.0f;
//        source_id[0] =0;
//
//        alListenerfv(AL_POSITION, zeroes);
//        alListenerfv(AL_VELOCITY, zeroes);
//        alListenerfv(AL_ORIENTATION, front);
//
//        alGenBuffers( 1, &bufferSize );
//
//}

//LinuxAudio::LinuxAudio(const char *fName){
//        ALfloat zeroes[] = { 0.0f, 0.0f, 0.0f };
//        ALfloat back[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
//        ALfloat front[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
//
//       source_id[0] =0;
//
//        alListenerfv(AL_POSITION, zeroes);
//        alListenerfv(AL_VELOCITY, zeroes);
//        alListenerfv(AL_ORIENTATION, front);
//
//        alGenBuffers( 1, &bufferSize );
//        waveFile=fName;
//        ERROR = alutLoadWAV(fName, &wave, &format, &size, &bits, &freq);
//        if(ERROR == AL_FALSE) {
//                fprintf(stderr, "LINUX_AUDIO: Could not Load %s\n",fName);
//                }
//        else {
//                fprintf(stdout, "LINUX_AUDIO: loaded file %s at %i  bits and %i Hz\n", fName, bits, freq);
//                };
//
//        alBufferData( bufferSize, format, wave, size, freq);
//        free(wave);
//
//        alGenSources (2, source_id);
//
//         alSourcefv(source_id[0], AL_POSITION, position);
//         alSourcei(source_id[0], AL_BUFFER, bufferSize);
//         alSourcei(source_id[0], AL_LOOPING, AL_TRUE);
//
//        position[0]*=-1.0f;
//
//         alSourcefv(source_id[1], AL_POSITION, position);
//         alSourcei(source_id[1], AL_BUFFER, bufferSize);
//         alSourcei(source_id[1], AL_LOOPING, AL_TRUE);
//         }

//LinuxAudio::LinuxAudio(const char *fName, ALboolean moving, ALboolean echooe){
//        ALfloat zeroes[] = { 0.0f, 0.0f, 0.0f };
//        ALfloat back[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
//        ALfloat front[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
//
//        source_id[0] =0;
//
//        alListenerfv(AL_POSITION, zeroes);
//        alListenerfv(AL_VELOCITY, zeroes);
//        alListenerfv(AL_ORIENTATION, front);
//
//        alGenBuffers( 1, &bufferSize );
//        waveFile=fName;
//        ERROR = alutLoadWAV(fName, &wave, &format, &size, &bits, &freq);
//        if(ERROR == AL_FALSE) {
//                fprintf(stderr, "LINUX_AUDIO: Could not Load %s\n",fName);
//                }
//       else {
//                fprintf(stdout, "LINUX_AUDIO: loaded file %s at %i  bits and %i Hz\n", fName, bits, freq);
//                };
//
//        alBufferData( bufferSize, format, wave, size, freq);
//        free(wave);
//
//        talReverbScale = (void (*)(ALuint sid, ALfloat param))GP("alReverbScale_LOKI");
//        talReverbDelay = (void (*)(ALuint sid, ALfloat param))GP("alReverbDelay_LOKI");
//
//       if (moving){
//                is_movingSource=AL_TRUE;
//                is_reverbSource=AL_FALSE;
//                }
//        else if(echooe){
//                        is_reverbSource=AL_TRUE;
//                        is_movingSource=AL_FALSE;
//                        ALfloat position[] = { 2.0f, 0.0f, 4.0f };
//
//                        alGenSources( 1, &reverb_sid);
//
//                        alSourcefv(reverb_sid, AL_POSITION, position );
//                        alSourcefv(reverb_sid, AL_VELOCITY, zeroes );
//                        alSourcefv(reverb_sid, AL_ORIENTATION, back );
//                        alSourcei(reverb_sid, AL_BUFFER, bufferSize );
//
//                        talReverbScale(reverb_sid, 0.35);
//                        talReverbDelay(reverb_sid, 1);
//                        }
//                else if (echooe&&moving){
//                                is_movingSource=AL_TRUE;
//                                is_reverbSource=AL_TRUE;
//                                }
//                        else{ is_reverbSource=AL_FALSE;
//                                is_movingSource=AL_FALSE;
//
//                                alGenSources (2, source_id);
//
//                                alSourcefv(source_id[0], AL_POSITION, position);
//                                alSourcei(source_id[0], AL_BUFFER, bufferSize);
//                                alSourcei(source_id[0], AL_LOOPING, AL_TRUE);
//                                alSourcePlay(source_id[0]);
//                                position[0]*=-1.0f;
//                                alSourcefv(source_id[1], AL_POSITION, position);
//                                alSourcei(source_id[1], AL_BUFFER, bufferSize);
//                                alSourcei(source_id[1], AL_LOOPING, AL_TRUE);
//                                };
//         }

//LinuxAudio::~LinuxAudio(){
//        }
//
//void LinuxAudio::playSound(){
//        fprintf(stdout, "LINUX_AUDIO: Playing Sound %s \n", waveFile);
//        if (is_movingSource){
//                }
//        else if (is_reverbSource){
//                alSourcePlay(reverb_sid);
//                }
//                else if ((is_reverbSource)&&(is_movingSource)) {
//                        }
//                        else {
//                                alSourcePlay(source_id[0]);
//                                micro_sleep(80000);
//                                alSourcePlay(source_id[1]);
//                                };
//        }
//
//void LinuxAudio::playSound(unsigned long int duration){
//        this->playSound();
//        micro_sleep(duration);
//        this->stopSound();
//        }

//void LinuxAudio::loadSound(char *fName){
//      alGenBuffers( 1, &bufferSize );
//
//       ERROR = alutLoadWAV(fName, &wave, &format, &size, &bits, &freq);
//       if(ERROR == AL_FALSE) {
//                fprintf(stderr, "LINUX_AUDIO: Could not Load %s\n",fName);
//                }
//        else {
//                fprintf(stdout, "LINUX_AUDIO: loaded file %s at %i  bits and %i Hz\n", fName, bits, freq);
//                };

//        alBufferData( bufferSize, format, wave, size, freq);
//        free(wave);
//
//        alSourcefv( source_id[0], AL_POSITION, position);
//        alSourcei( source_id[0], AL_BUFFER, bufferSize);
//        alSourcei(source_id[0], AL_LOOPING, AL_TRUE);
//        }

//void LinuxAudio::stopSound(){
//        alSourceStop(source_id[0]);
//        alSourceStop(source_id[1]);
//        fprintf(stdout, "LINUX_AUDIO: %s : Source %i stopped!\n", waveFile, 0);
//        }

//void LinuxAudio::pauseSound(){
//        alSourcePause(source_id[0]);
//        alSourcePause(source_id[1]);
//        fprintf(stdout, "LINUX_AUDIO: %s : Source %i paused!\n", waveFile, 0);
//        }

//void LinuxAudio::pauseSound(unsigned long int duration){
//        alSourcePause(source_id[0]);
//         alSourcePause(source_id[1]);
//        fprintf(stdout, "LINUX_AUDIO: %s : Source %i paused for %i ms!\n", waveFile, duration);
//        micro_sleep(duration);
//        alSourcePlay(source_id[0]);
//         alSourcePause(source_id[1]);
//        }

//void LinuxAudio::setListenerPosition(ALfloat * new_position){
//        memcpy(new_position, lispos, 12);
//        alListenerfv(AL_POSITION, lispos);
//        fprintf(stderr, "LINUX_AUDIO: %s :New Listener Position = (%f %f %f)\n", waveFile, lispos[0], lispos[1], lispos[2]);
//        }

//void LinuxAudio::setListenerPosition(ALfloat x, ALfloat y, ALfloat z){
//        lispos[0]=x; lispos[1]=y; lispos[2]=z;
//        alListenerfv(AL_POSITION, lispos);
//        fprintf(stderr, "LINUX_AUDIO: %s :New Listener Position = (%f %f %f)\n", waveFile, lispos[0], lispos[1], lispos[2]);
//        }

//void LinuxAudio::setSourcePosition(ALfloat * new_position){
//        memcpy(new_position, position, 3*sizeof(ALfloat));
//        alSourcefv( source_id[0], AL_POSITION, position);
//        fprintf(stderr, "LINUX_AUDIO: %s: Source %i Position changed to (%f %f %f)\n", waveFile, 0, position[0], position[1], position[2]);
//        }

//void LinuxAudio::discardSound(){
//        }

//void * LinuxAudio::getWave(){
//        return wave;
//        }

//ALfloat  * LinuxAudio::getLisPos(){
//        return lispos;
//        }
/*Neccessary only for the Linux-Port*/
//static ALboolean alutLoadWAVFile(	const char *fname, 
//					ALsizei *format, 
//					ALvoid **wave, 
//					ALsizei *size, 
//					ALsizei *freq ) {
//	ALsizei bits; /* unused */
//	return alutLoadWAV(fname, wave, format, size, &bits, freq);
//}

//static ALboolean alutUnloadWAV(	ALenum format,
//				ALvoid *wave,
//			       	ALsizei size,
//			       	ALsizei freq ) {
//	free(wave);
//	return AL_TRUE;
//}
