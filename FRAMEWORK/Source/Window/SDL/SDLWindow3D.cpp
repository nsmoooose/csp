
#ifdef SDL

#include "SDLWindow3D.h"
//#include "SDL.h"
//#include <GL/gl.h>

SDLWindow3D::SDLWindow3D()
{

}


SDLWindow3D::~SDLWindow3D()
{

}

  // functions
short SDLWindow3D::Initialize(StandardApp *App, StandardFramework *Framework,_GRAPHICS_INIT_STRUCT **Init)
{ 
#ifdef TRACE_WINDOW
  fprintf(stdout, "SDLWindow3D::Initialize\n");
#endif

  int bpp;
  const SDL_VideoInfo * info = NULL;

  p_Framework = Framework;

  info = SDL_GetVideoInfo();
  if (!info)
    {
      fprintf(stderr, "Unable to get Video info from SDL!!\n");
    }

#ifdef TRACE_WINDOW
  fprintf(stdout, "Hardware available: %d\n", info->hw_available);
  fprintf(stdout, "Window manager available: %d\n", info->wm_available);
  fprintf(stdout, "Amount of video memory: %d\n", info->video_mem);
  fprintf(stdout, "Bits Per Pixel: %d\n", info->vfmt->BitsPerPixel);
  fprintf(stdout, "Bytes Per Pixel: %d\n", info->vfmt->BytesPerPixel);
#endif

  bpp = info->vfmt->BitsPerPixel;

  

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  int result;


  if (  SDL_SetVideoMode((*Init)->Width,(*Init)->Height , bpp, SDL_OPENGL) == 0)
    {
      fprintf(stderr, "Unable to setup SDL-OPENGL video\n");
    }
  else
    {
      fprintf(stdout, "Set GL video mode\n");
    }

  fprintf(stdout, "Screen BPP: %d\n", SDL_GetVideoSurface()->format->BitsPerPixel);
  fprintf(stdout, "\n");
  fprintf(stdout,  "Vendor     : %s\n", glGetString( GL_VENDOR ) );
  fprintf(stdout,  "Renderer   : %s\n", glGetString( GL_RENDERER ) );
  fprintf(stdout,  "Version    : %s\n", glGetString( GL_VERSION ) );



  SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &result);
  fprintf(stdout, "Red Size: %d\n", result);

  SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &result);
  fprintf(stdout, "Green Size: %d\n", result);
 
  SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &result);
  fprintf(stdout, "Blue Size: %d\n", result);

  SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &result);
  fprintf(stdout, "Depth Size: %d\n", result);

  SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &result);
  fprintf(stdout, "Double Buffer: %d\n", result);
		
  return 0;
}

void SDLWindow3D::Uninitialize()
{


}


void SDLWindow3D::HandleSystemMessages()
{
#ifdef TRACE_WINDOW
  fprintf(stdout, "SDLWindow3D::HandleSystemMessages()\n");
#endif
  SDL_Event event;
  int numrtn = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_QUITMASK);
  if (numrtn == 1)
        p_Framework->TriggerShutdown();



}

//_HANDLE         GetWindowHandle();

bool SDLWindow3D::GetFocused()
{
#ifdef TRACE_WINDOW
  fprintf(stdout, "SDLWindow3D::GetFocused()\n");
#endif
  return true;
}

void SDLWindow3D::Show()
{
#ifdef TRACE_WINDOW
  fprintf(stdout, "SDLWindow3D::Show()\n");
#endif
}

_Point SDLWindow3D::GetMouse()
{
#ifdef TRACE_WINDOW
  fprintf(stdout, "SDLWindow3D::GetMouse()\n");
#endif
  _Point p(0,0);
  return p;
}

void SDLWindow3D::SetMouse(short x, short y)
{
#ifdef TRACE_WINDOW
  fprintf(stdout, "SDLWindow3D::SetMouse()\n");
#endif
}

void SDLWindow3D:: SetWindowSize(short x, short y)
{
#ifdef TRACE_WINDOW
  fprintf(stdout, "SDLWindow3D::SetWindowSize()\n");
#endif
}



#endif
