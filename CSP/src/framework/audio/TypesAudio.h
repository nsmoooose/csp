#include "standard/StandardFramework.h"

// Available Preprocessors for Audio
// 

// List of Audio API's
/// Define used for identifying the "NoAudio"-module.
const short _NOAUDIO     = -1;
/// Define used for identifying the "NoAudio"-module.
const short _DIRECTSOUND = 0;


// If no API's found then default to no audio.
const short DEFAULT_AUDIO = _NOAUDIO;


/** Description of Audio APIs. Will be used for each
    sound-implementation to return its identity.*/
struct _AUDIO_DESCRIPTION
{
  /// a string containing the audios human-readabla name
  char   *Name;
  /// just another ID ;) .. use one of the DEFINES above (_NOAUDIO, ....)
  short   ID;
};

struct _AUDIO_INIT_STRUCT
{
  void *this_inst;
};
