#include "LinuxWindow3D.h"
//#include "SDL.h"
//#include <GL/gl.h>

LinuxWindow3D::LinuxWindow3D()
{

}


LinuxWindow3D::~LinuxWindow3D()
{

}

  // functions
short LinuxWindow3D::Initialize(StandardApp *App, StandardFramework *Framework,_GRAPHICS_INIT_STRUCT **Init)
{ 

  fprintf(stdout, "LinuxWindow3D::Initialize\n");

  int bpp;
  const SDL_VideoInfo * info = NULL;


  info = SDL_GetVideoInfo();
  if (!info)
    {
      fprintf(stderr, "Unable to get Video info from SDL!!\n");
    }

  fprintf(stdout, "Hardware available: %d\n", info->hw_available);
  fprintf(stdout, "Window manager available: %d\n", info->wm_available);
  fprintf(stdout, "Amount of video memory: %d\n", info->video_mem);
  fprintf(stdout, "Bits Per Pixel: %d\n", info->vfmt->BitsPerPixel);
  fprintf(stdout, "Bytes Per Pixel: %d\n", info->vfmt->BytesPerPixel);

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

void LinuxWindow3D::Uninitialize()
{


}


void LinuxWindow3D::HandleSystemMessages()
{
  fprintf(stdout, "LinuxWindow3D::HandleSystemMessages()\n");

}

//_HANDLE         GetWindowHandle();

bool LinuxWindow3D::GetFocused()
{
  fprintf(stdout, "LinuxWindow3D::GetFocused()\n");
  return true;
}

void LinuxWindow3D::Show()
{
  fprintf(stdout, "LinuxWindow3D::Show()\n");
}

_Point LinuxWindow3D::GetMouse()
{
  fprintf(stdout, "LinuxWindow3D::GetMouse()\n");
  _Point p(0,0);
  return p;
}

void LinuxWindow3D::SetMouse(short x, short y)
{
  fprintf(stdout, "LinuxWindow3D::SetMouse()\n");
}

void LinuxWindow3D:: SetWindowSize(short x, short y)
{
  fprintf(stdout, "LinuxWindow3D::SetWindowSize()\n");
}

