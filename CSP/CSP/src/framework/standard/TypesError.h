#include "StandardFramework.h"

// A list of errors
const short _A_OK                             = 0;
const short _GENERIC_ERROR                    = -1;

// -10s are Framework errors.
const short _DEPENDENCY_NOT_SET               = -10;
const short _SETUP_NOT_SET                    = -11;
const short _NO_DEPENDENCY_TO_SET             = -12;

// -50s are Factory errors.
const short _INDEX_OUT_OF_RANGE               = -50;
const short _NOT_IMPLEMENTED                  = -51;
const short _BAD_ID                           = -52;

// -100s are Platform errors.
const short _WINDOW_REGISTER_FAILED           = -100;
const short _WINDOW_CREATION_FAILED           = -101;

// -200s are Input Errors
const short _INPUT_LOST                       = -200;
const short _NO_DIGITAL_JOYSTICK              = -201;
const short _NO_JOYSTICK_FOUND                = -202;
const short _BUFFER_OVERFLOW                  = -203;

// -300s are Graphic errors
const short _CREATING_3DDEVICE_FAILED         = -300;
const short _FUNKY_VIDEO_FORMAT               = -301;
const short _FUNKY_DEPTH_FORMAT               = -302;

// -500s are Sound errors
const short _SOUND_FORMAT                     = -500;
const short _SOUND_LOADING                    = -501;
const short _SOUND_CREATE_SECONDARY           = -502;
const short _SOUND_OUTOFRANGE                 = -503;
const short _SOUND_PLAY_FAILED                = -504;
